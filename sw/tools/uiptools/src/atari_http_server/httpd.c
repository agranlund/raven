/*
 * Copyright (c) 2004, Adam Dunkels.
 * Copyright (c) 2015-19, Mariusz Buras.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: atarid.c,v 1.2 2006/06/11 21:46:38 adam Exp $
 */

#ifndef WITHOUT_HTTP

#include "uip.h"
#include "httpd.h"
#include "../logging.h"
#include "../ioredirect.h"
#include "../common.h"

#include "psock.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>

#include <osbind.h>
#include <errno.h>

#include <ctype.h>
#include <stdbool.h>

// static content
#include "ui/index.html"

#define ISO_nl      0x0a
#define ISO_space   0x20
#define ISO_bang    0x21
#define ISO_percent 0x25
#define ISO_period  0x2e
#define ISO_slash   0x2f
#define ISO_colon   0x3a

#define HEAP_SIZE 4096
#define INPUTBUF_SIZE (0x8000)

/*---------------------------------------------------------------------------*/

struct DataSource;

typedef struct httpd_state {
  const char* http_request_type;
  int http_result_code;
  const char* http_result_string;
  struct psock sin;
  uint8_t* inputbuf;
  uint32_t inputbuf_size;
  char query[256];
  char filename[256];
  char original_filename[256];
  FILE* file;
  size_t expected_file_length;
  uint32_t expected_100_continue;

  char multipart_encoded;

  int32_t tosFileDateTime;
  int32_t tosFileDateTimeData;

  struct pt worker[4];
  size_t temp_file_length;

  void (*idle_run_handler)(struct httpd_state *s);
  char storeCurrentPath[256];
  char run_path[256];
  int16_t storeCurrentDrive;
  uip_ipaddr_t remote_ipaddr;

  char(*handler_func)(struct pt* worker,struct httpd_state *s);

  struct DataSource* handler_datasrc;

  int16_t  fd;

  uint8_t heap[HEAP_SIZE];
};

/*---------------------------------------------------------------------------*/


ssize_t file_size(const char* path)
{
  Fsetdta (&dta);
  if (Fsfirst (path, 0) == 0) {
    return dta.dta_size;
  }
  return -1;
}

static
PT_THREAD(receive_file(struct pt* worker, struct httpd_state *s, const char* filename, const size_t filelen))
{
  PT_BEGIN(worker);

  LOG_TRACE("receive_file: %s\r\n", filename);
  //(void)LOG("\033K");

  // make sure folder exists
  if (!ensureFolderExists(filename, true)) {
    s->http_result_code = 400;
    LOG_TRACE(" -> failed to create folder!\r\n");
    PT_EXIT(worker);
  }

  s->fd = Fcreate(filename, 0);

  if (s->fd < 0) {
    s->http_result_code = 400;
    LOG_TRACE(" -> failed to open!\r\n");
    PT_EXIT(worker);
  }

  s->temp_file_length = filelen;

  while (s->temp_file_length > 0) {
    int32_t write_ret = 0;
    PSOCK_READBUF_LEN2(worker, &s->sin,
      s->temp_file_length > s->inputbuf_size ?
        s->inputbuf_size : s->temp_file_length);

    write_ret = Fwrite(s->fd, PSOCK_DATALEN(&s->sin), s->inputbuf);

    if (write_ret < 0 || write_ret != PSOCK_DATALEN(&s->sin) || uip_closed()) {
      s->http_result_code = 500;
      LOG_TRACE("Fwrite failed!\r\n");
      Fclose_safe(&s->fd);
      PT_EXIT(worker);
    }

    s->temp_file_length-=PSOCK_DATALEN(&s->sin);
  }

  if (s->tosFileDateTime) {
    _DOSTIME dostime;
    dostime.time = s->tosFileDateTimeData >> 16;
    dostime.date = s->tosFileDateTimeData & 0xffff;
    Fdatime (&dostime, s->fd, 1); // set date/time
  }

  Fclose_safe(&s->fd);
  s->http_result_code = 201;

  PT_END(worker);
}

/*---------------------------------------------------------------------------*/

static
PT_THREAD(handle_post(struct pt* worker, struct httpd_state *s))
{
  PT_BEGIN(worker);

  if (s->expected_file_length == -1) {
    s->http_result_code = 411;
    PT_EXIT(worker);
  } else if (s->multipart_encoded == 1) {
    s->http_result_code = 401;
    PT_EXIT(worker);
  } else {
    PT_INIT(&worker[1]);
    PT_WAIT_THREAD(worker, receive_file(&worker[1], s, s->filename, s->expected_file_length));
  }

  PT_END(worker);
}

/*---------------------------------------------------------------------------*/

static void file_stat_single(struct Repsonse* response)
{
  char json_name[256] = {"\0"};

  fstrcat(response," {\r\n"
    "    \"type\" : \"%s\",\r\n",
    dta.dta_attribute&FA_DIR ? "d" : "f");

  if (!(dta.dta_attribute&FA_DIR)) {
    fstrcat(response, "    \"size\" : \"%u\",\r\n", dta.dta_size);
  }

  strncpy (json_name, dta.dta_name, sizeof(json_name));

  for(size_t i=0; json_name[i] != 0; i++)
  {
    if (json_name[i] == '\\') {
      json_name[i] = '_';
    }
  }

  fstrcat(response, "    \"name\" : \"%s\",\r\n", json_name);
  fstrcat(response, "    \"date\" : \"%d/%d/%d\",\r\n",
          (dta.dta_date&0x1f), ((dta.dta_date>>5)&0xf), ((dta.dta_date>>9) + 1980));
  fstrcat(response, "    \"time\" : \"%d:%d:%d\"\r\n",
          (dta.dta_time>>11), (dta.dta_time>>5)&0x3f, (dta.dta_time&0x1f)*2);
  fstrcat(response,"  }\r\n");
}

const char* file_stat_json(const char* path)
{
  struct Repsonse response = { NULL, NULL, 8192 };
  char dos_path[512] = { '\0' };
  char dos_path_helper[2] = { '\0', '\0' };

  Fsetdta (&dta);
  memset(&dta, 0, sizeof(dta));
  response.malloc_block = (char*)malloc (response.size);
  *response.malloc_block = 0;
  response.current = response.malloc_block;

  strncpy (dos_path, path, sizeof(dos_path));

  fstrcat(&response, " [\r\n");

  // this is a bit dodgy, I'm not sure why I need to do this:
  // it turns out that to scan the root of the drive I need to
  // supply a path that ends with a backslash otherwise it won't work
  // But for forlders it accually can't end with backslash because
  // it won't work.. madness!!!!!
  if (strlen(dos_path) > 3 && dos_path[strlen(dos_path)-1] == '\\') {
    dos_path[strlen(dos_path)-1] = 0;
  }

  if (dos_path[0] == '\0') {
    // list drivers
    uint32_t drv_map = Drvmap();
    char i = 0;
    int first = 1;
    while (drv_map) {
      if (drv_map&1) {
        if (!first){
          fstrcat(&response, ",\r\n");
        }
        first = 0;
        fstrcat(&response, " {\r\n"
          "    \"type\" : \"d\",\r\n");
        fstrcat(&response,
           "    \"name\" : \"%c\"\r\n", 'a' + i);
        fstrcat(&response, "  }\r\n");
      }
      i++;
      drv_map >>=1;
    }
  }
    /* if we're at the root of the drive or at a folder */
  else if (strlen(dos_path) == 3 || 0 == Fsfirst(dos_path, FA_DIR|FA_HIDDEN|FA_SYSTEM)) {
    // ok so this is a folder
    if (strlen(dos_path) == 3 || (dta.dta_attribute&FA_DIR)) {
      if (dos_path[strlen(dos_path)-1] != '\\') {
        strcat(dos_path, "\\");
      }
      strcat(dos_path, "*.*");
      if (0 == Fsfirst(dos_path, FA_DIR|FA_HIDDEN|FA_SYSTEM)) {
        int first = 1;
        do {
          // skip .. and . pseudo folders
          if (strcmp(dta.dta_name, "..") != 0
              && strcmp(dta.dta_name, ".") != 0
          // && !dta.dta_attribute&FA_SYSTEM
              && !(dta.dta_attribute&FA_LABEL)
           ) {
            if (!first) {
              fstrcat(&response, ",\r\n");
            }
            file_stat_single(&response);
            first = 0;
          }
        } while (0 == Fsnext());
      } else {
        /* Error */
        LOG_TRACE("path not found 2\r\n");
      }
    } else {
      // it's a file
      file_stat_single(&response);
    }
  } else {
      LOG_TRACE("path not found\r\n");
  }

  fstrcat(&response, "]\r\n");

  return response.malloc_block;

error:
  free(response.malloc_block);
  return NULL;
}

/*---------------------------------------------------------------------------*/

struct DataSource
{
  ssize_t (*read)(struct DataSource*, size_t, void*);
  void (*close)(struct DataSource*);
  ssize_t (*size)(struct DataSource*);
  const char* mime_type;
  const char* encoding_type;
};

struct FsSource
{
  struct DataSource src;
  int16_t fd;
  size_t size;
};

ssize_t fileSourceSize(struct DataSource* ds)
{
  struct FsSource* fs = (struct FsSource*) ds;
  return fs->size;
}

ssize_t fileSourceRead(struct DataSource* ds, size_t size, void* ptr)
{
  struct FsSource* fs = (struct FsSource*) ds;
  return (size_t)Fread(fs->fd, size, ptr);
}

void fileSourceClose(struct DataSource* ds)
{
  struct FsSource* fs = (struct FsSource*) ds;
  Fclose_safe(&fs->fd);
  free ((void*) fs);
}

struct DataSource* fileSourceCreate(
  const char* fname,
  const char* mime_type,
  const char* encoding_type)
{
  struct FsSource* src = NULL;
  int16_t fd = Fopen(fname, 0);

  if (fd > 0) {
    src = (struct FsSource*)malloc(sizeof(struct FsSource));
    src->size = file_size(fname);
    src->src.read = fileSourceRead;
    src->src.close = fileSourceClose;
    src->src.size = fileSourceSize;
    src->fd = fd;
    src->src.mime_type = mime_type;
    src->src.encoding_type = encoding_type;
  }

  return (struct DataSource*)src;
}

struct MemSource
{
  struct DataSource src;
  const char* ptr;
  ssize_t size;
  ssize_t current;
  char ownership;
};

ssize_t memSourceRead(struct DataSource* ds, size_t size, void* ptr)
{
  struct MemSource* mem = (struct MemSource*) ds;
  size_t actual_size = size + mem->current > mem->size ? mem->size-mem->current : size;

  if (mem->current >= mem->size) {
    return 0;
  }

  memcpy (ptr, &mem->ptr[mem->current], actual_size);

  mem->current += actual_size;

  return actual_size;
}

void memSourceClose (struct DataSource* ds)
{
  struct MemSource* mem = (struct MemSource*) ds;
  if (mem->ownership) {
    free ((void*) mem->ptr);
  }
  free ((void*) mem);
  LOG_TRACE("memSourceClose %p\r\n", mem);
}

ssize_t memSourceSize(struct DataSource* ds)
{
  struct MemSource* mem = (struct MemSource*) ds;
  return mem->size;
}

struct DataSource* memSourceCreate (
    const char* ptr,
    size_t size,
    const char* mime_type,
    const char* encoding_type,
    char ownership)
{
  struct MemSource* src = NULL;
  src = (struct MemSource*)malloc(sizeof(struct MemSource));
  src->src.read = memSourceRead;
  src->src.close = memSourceClose;
  src->src.size = memSourceSize;
  src->current = 0;
  src->ptr = ptr;
  src->size = size;
  src->src.mime_type = mime_type;
  src->src.encoding_type = encoding_type;
  src->ownership = ownership;
  LOG_TRACE("memSourceCreate %p\r\n", src);
  return (struct DataSource*)src;
}

struct GetState {
  int bytes_read;
  size_t buffer_start_offset;
  int file_len;
  struct DataSource* source;
  uint32_t send_buffer_size;
};

static
PT_THREAD(handle_get(struct pt* worker, struct httpd_state *s))
{
  struct GetState* this = (struct GetState*)s->heap;
  struct DataSource* src = s->handler_datasrc;

  PT_BEGIN(worker);

  if (!src) {
    s->http_result_code = 400;
    PT_EXIT(worker);
  }

  this->buffer_start_offset = 0;

  if (src) {
    this->file_len = src->size(src);

    this->buffer_start_offset = snprintf(
      s->inputbuf, UIP_TCP_MSS,
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: %s\r\n"
      "Content-Encoding: %s\r\n"
      "Connection: Keep-Alive\r\n"
      "Content-Length: %u\r\n\r\n",
      src->mime_type,
      src->encoding_type,
      this->file_len);

  } else {
    s->http_result_code = 404;
    src->close(src);
    s->handler_datasrc = NULL;
    LOG("\r\n");
    PT_EXIT(worker);
  }

  this->send_buffer_size = UIP_TCP_MSS * 1;

  while (1) {
    this->bytes_read = src->read(src, this->send_buffer_size-this->buffer_start_offset,
            &s->inputbuf[this->buffer_start_offset]);

    if (this->bytes_read == 0)
      break;

    if (this->bytes_read < 0) {
      s->http_result_code = 400;
      break;
    }

    PSOCK_SEND2(worker, &s->sin, s->inputbuf, this->bytes_read+this->buffer_start_offset);
    this->buffer_start_offset = 0;

    if(this->send_buffer_size != UIP_TCP_MSS * 16) {
      this->send_buffer_size <<= 1;
    }
  }

  src->close(src);
  s->handler_datasrc = NULL;
  PT_END(worker);
}

/*---------------------------------------------------------------------------*/

static
void handle_idle_run(struct httpd_state *s)
{
  /* Process arguments */
  s->idle_run_handler = NULL;

  uint8_t* args = strchr(s->query, '=');

  if (args) {
    LOG_TRACE("args: %s\r\n", args);
    const size_t max_args_len = 124;
    size_t args_len = strlen(args+1);
    if (args_len > max_args_len) {
      LOG_WARN("Command line too long!");
      args_len = max_args_len;
    }
    *args = (uint8_t)args_len;
    LOG_TRACE("args len: %d\r\n");
  }

  void* basepage = (void*)Pexec(PE_LOAD, s->run_path, args, NULL);

  if (basepage) {
    ioredirect_start(uip_ipaddr1(&s->remote_ipaddr), uip_ipaddr2(&s->remote_ipaddr),
      uip_ipaddr3(&s->remote_ipaddr), uip_ipaddr4(&s->remote_ipaddr));

    int16_t pexec_ret = Pexec(PE_GO_FREE, 0, basepage, 0);

    ioredirect_stop();
  }

  while( -1 == Cconis() ) Cconin ();
  Dsetdrv (s->storeCurrentDrive);
  Dsetpath(s->storeCurrentPath);
}

static
PT_THREAD(handle_run(struct pt* worker, struct httpd_state *s))
{
  char temp_path[256] = {"\0"};
  bool path_ok = false;

  PT_BEGIN(worker);

  strncpy(temp_path, s->filename, sizeof(temp_path));
  // remove file name from the path
  for (size_t i = strlen(s->filename); i != 0; --i) {
    if (temp_path[i] == '\\') {
      temp_path[i] = '\0';
      break;
    }
    temp_path[i] = '\0';
  }

  LOG_TRACE("handle_run: %s in %s\r\n", s->filename, temp_path);

  if (file_size(s->filename) == -1) {
    LOG_WARN("File not found: %s\r\n", s->filename);
    s->http_result_code = 1404;
    PSOCK_EXIT2(worker, &s->sin);
  }

  Dgetpath (s->storeCurrentPath, 0);
  s->storeCurrentDrive = Dgetdrv ();

  /* Set the current drive and path */
  if (tolower(temp_path[0]) >= 'a'
      && tolower(temp_path[0]) <= 'z') {
    uint32_t drv_map = Dsetdrv (Dgetdrv ());
    uint32_t drv_num = tolower(temp_path[0]) - 'a';
    uint32_t drv_bit = 1 << drv_num;

    if (drv_bit&drv_map) {
      Dsetdrv (drv_num);
    }

    {
      char cwd[256] = {"\0"};
      Dgetpath (cwd, 0);
      /* We want to convert to lower case and skip first backslash */
      for (int i = 0; i < strlen(cwd); i++) {
        cwd[i] = tolower (cwd[i]);
      }

      const char* path_no_drive = &temp_path[2];

      path_ok = true;

      if (strcmp(cwd, path_no_drive) != 0) {
        // set cwd
        int ret = Dsetpath(path_no_drive);
        path_ok = ret == 0 ? true : false;
        LOG_TRACE("handle_run (%d): cwd: %s, path_no_drive: %s\r\n",
          ret, cwd, path_no_drive);
      }
    }
  }

  if (!path_ok) {
    LOG_WARN("Path set failed (%d) for: %s\r\n", path_ok, s->filename);
    s->http_result_code = 400;
    PSOCK_EXIT2(worker, &s->sin);
  }

  /* Close connection first */
  PSOCK_SEND_STR2(worker, &s->sin, "HTTP/1.0 200 OK\r\nConnection: close\r\n");
  PSOCK_CLOSE(&s->sin);
  s->idle_run_handler = handle_idle_run;
  uip_ipaddr_copy(s->remote_ipaddr, uip_conn->ripaddr);
  strcpy(s->run_path, s->filename);
  PT_END(worker);
}

/*---------------------------------------------------------------------------*/

static
PT_THREAD(handle_delete(struct pt* worker, struct httpd_state *s))
{
  PT_BEGIN(worker);

  if(Fdelete (s->filename)) {
    s->http_result_code = 400;
  } else {
    s->http_result_code = 200;
  }

  LOG_TRACE("delete: %s %d\r\n", s->filename, s->http_result_code);
  PT_END(worker);
}

/*---------------------------------------------------------------------------*/

void parse_url(struct httpd_state *s)
{
  char* fn_end = s->inputbuf;
  char* fn, *query;

  /* Find the first character of the file path */
  while (*fn_end++ != '/');

  fn = fn_end;

  /* skip to the end of line */
  while (*fn_end != '\r' && *fn_end != '\n' && *fn_end != '\0') {
    fn_end++;
  }

  /* We want to skip the last word if the line which would usually be something like "HTTP/1.0" */
  while (*fn_end != ' ' && fn_end > fn) {
    --fn_end;
  }

  *fn_end = 0;

  /* store original path */
  strncpy(s->original_filename, fn, sizeof(s->original_filename));

  /* extract query string */
  query=fn;
  s->query[0] = 0;

  /* find if there's a query in the URL */
  while (*query != '?' && query != fn_end) query++;

  /* got query string? */
  if (query != fn_end) {
    /* Now copy the query with the unnecesery bit removed */
    strncpy (s->query, &query[1], sizeof(s->query));
    *query=0;
  }

  /* convert path to dos/atari format */
  while (fn != fn_end--) {
    if (*fn_end == '/') {
      *fn_end = '\\';
    }
  }

  s->filename[0] = fn[0]&0x7f;
  s->filename[1] = ':';
  strncpy(&s->filename[2], ++fn, sizeof(s->filename)-2);
}

static int query_dir(struct httpd_state *s)
{
  const char* dir_json = file_stat_json(s->filename);
  if (dir_json) {
    s->handler_datasrc = memSourceCreate(dir_json, strlen(dir_json), "text/javascript", "identity", 1);
    return 1;
  }
  return 0;
}

static int query_run(struct httpd_state *s)
{
  s->handler_func = handle_run;
  return 1;
}

static int query_newfolder(struct httpd_state *s)
{
  s->http_result_code = 1200;
  s->handler_func = NULL;
  if (!ensureFolderExists(s->filename, false)) {
    s->http_result_code = 400;
  }
  return 1;
}

static int query_setfiledate(struct httpd_state *s)
{
  char* dateIntStart = strchr(s->query, '=');;
  if (dateIntStart) {
    dateIntStart++; // skip the '='
    s->tosFileDateTime = 1;
    s->tosFileDateTimeData = strtoul (dateIntStart, NULL, 0);
  }
}

static int parse_query(struct httpd_state *s)
{
  static struct {
    const char* query_string;
    int (*query_func)(struct httpd_state *s);
  } query_mapping [] = {
    {"dir", query_dir},
    {"run", query_run},
    {"newfolder", query_newfolder},
    {"setfiledate", query_setfiledate},
    {NULL,NULL}
  };
  LOG_TRACE("parse query: ");
  if (s->query[0] != 0) {
    for (size_t i = 0; query_mapping[i].query_string != 0 ; i++) {
      if (strncmp(query_mapping[i].query_string, s->query, strlen(query_mapping[i].query_string)) == 0) {
          LOG_TRACE("%s\r\n", query_mapping[i].query_string);
          return query_mapping[i].query_func(s);
      }
    }
  }
  LOG_TRACE("query not supported: %s\r\n", s->query);
  return 0;
}

static void parse_post(struct httpd_state *s)
{
  parse_url(s);
  parse_query(s);
  s->handler_func = handle_post;
}

struct
{
  const char *url;
  const unsigned char *data_ptr;
  const unsigned int data_size;
  const char *content_type;
  const char *encoding_type;
} static static_url_mapping[] = {
    {"", index_html_gz, sizeof(index_html_gz), "text/html; charset=UTF-8", "gzip"},
    {"index.html", index_html_gz, sizeof(index_html_gz), "text/html; charset=UTF-8", "gzip"},
    {NULL, NULL, 0}};

static void parse_get(struct httpd_state *s)
{
  LOG_TRACE("request: %s\r\n", s->inputbuf);

  parse_url(s);

  s->handler_datasrc = NULL;
  s->handler_func = handle_get;

  if (s->query[0] == 0) {
    /* maybe request a static resource */
    for (size_t i = 0; static_url_mapping[i].url!=0 ; i++) {
      LOG_TRACE("original request path: %s\r\n", s->original_filename);
      if (strcmp(static_url_mapping[i].url, s->original_filename) == 0) {
        s->handler_datasrc = memSourceCreate(
                              static_url_mapping[i].data_ptr,
                              static_url_mapping[i].data_size,
                              static_url_mapping[i].content_type,
                              static_url_mapping[i].encoding_type,
                              0);
        LOG_TRACE("%s", static_url_mapping[i].url);
        break;
      }
    }
    if (!s->handler_datasrc) {
      LOG_TRACE("%s", s->filename);
      s->handler_datasrc = fileSourceCreate(s->filename, "application/octet-stream","identity");
      if (!s->handler_datasrc) {
        // Couldn't create file source
        s->http_result_code = 400;
        s->handler_func = NULL;
      }
    }
  } else if (!parse_query(s)) {
    s->http_result_code = 400;
    s->handler_func = NULL;
  }
}

static void parse_content_len(struct httpd_state *s)
{
  s->expected_file_length = atoi(&s->inputbuf[15]);
}

static void parse_delete(struct httpd_state *s)
{
  parse_url(s);
  s->handler_func = handle_delete;
}

static void parse_expect(struct httpd_state *s)
{
  s->expected_100_continue = 1;
}

static void parse_urlencoded(struct httpd_state *s)
{
  s->multipart_encoded = 1;
}

#define HeaderEntry(str,func,type) {str, sizeof(str), func, type}

struct {
  const char* entry;
  const size_t entry_len;
  void (*parse_func)(struct httpd_state *s);
  const char request_type;
} static commands[] = {
  HeaderEntry ("POST", parse_post, 1),
  HeaderEntry ("PUT", parse_post, 1),
  HeaderEntry ("GET", parse_get, 1),
  HeaderEntry ("DELETE", parse_delete, 1),
  HeaderEntry ("Content-Length:", parse_content_len, 0),
  HeaderEntry ("Expect: 100-continue", parse_expect, 0),
  HeaderEntry ("Content-Type: application/x-www-form-urlencoded", parse_urlencoded, 0),
};

struct {
  const int http_result_code;
  const char* http_response_string;
} static http_responses[] = {
  { 200, "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n" },
  { 201, "HTTP/1.1 201 Created\r\nContent-Length: 0\r\n\r\n" },
  { 400, "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n" },
  { 404, "HTTP/1.1 404 Not Found\r\nnContent-Length: 0\r\n\r\n" },
  { 1404, "HTTP/1.1 404 Not Found\r\nnContent-Length: 0\r\nConnection: close\r\n\r\n" },
  { 411, "HTTP/1.1 411 Length Required\r\nConnection: close\r\n\r\n" },
  { 500, "HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\n\r\n" },
  { 1200, "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n" },
};

static
PT_THREAD(handle_connection(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sin);

  do {

    s->expected_100_continue = 0;
    s->expected_file_length = -1;
    s->handler_func = NULL;
    s->filename[0] = '\0';
    s->original_filename[0]= '\0';
    s->multipart_encoded = 0;
    s->fd = -1;
    s->http_result_code = 0;
    s->tosFileDateTime = 0;

    while (1) {
      // eat away the header
      PSOCK_READTO(&s->sin, ISO_nl);
      {
        size_t readlen = psock_datalen(&s->sin);
        s->inputbuf[readlen] = 0;
      }

      if (s->inputbuf[0] == '\r') {
        //got header, now get the data
        break;
      }

      for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i) {
        if (0 == memcmp(s->inputbuf, commands[i].entry, commands[i].entry_len - 1)) {
          LOG_TRACE("method: %s\r\n", commands[i].entry);
          commands[i].parse_func(s);
          break;
        }
      }
    }

    if (s->expected_100_continue) {
      PSOCK_SEND_STR(&s->sin, "HTTP/1.1 100 Continue\r\n");
    }

    /* if handler function was set execute it */
    if (s->handler_func) {
      PT_INIT(&s->worker[0]);
      PSOCK_WAIT_THREAD(&s->sin, s->handler_func(&s->worker[0], s));
    }

    LOG("\r\n");

    if (s->http_result_code != 0) {
      // send result code
      for (size_t i = 0; i < sizeof(http_responses) / sizeof(http_responses[0]); ++i) {
        if (s->http_result_code == http_responses[i].http_result_code) {
          s->http_result_string = http_responses[i].http_response_string;
          break;
        }
      }

      if (s->http_result_string) {
        PSOCK_SEND_STR(&s->sin, s->http_result_string);
      } else {
        LOG_WARN("Error: no result string for the code: %d\r\n", s->http_result_code);
      }
      /* Keep the main loop spinning for anything other then 500 */
      if (s->http_result_code < 500) {
        s->http_result_code = 0;
      }
    }
  } while (s->http_result_code < 299);

  PSOCK_CLOSE_EXIT(&s->sin);
  PSOCK_END(&s->sin);
}
/*---------------------------------------------------------------------------*/
static void
handle_error(struct httpd_state *s)
{
  if (!s) {
    return;
  }

  if (Fclose_safe(&s->fd) != -1) {
    LOG_WARN("FClose -> failed\r\n");
  }
}

/*---------------------------------------------------------------------------*/
struct httpd_state* httpd_get_state()
{
  return (struct httpd_state *)(uip_conn->appstate);
}

struct httpd_state* httpd_alloc_state()
{
  struct httpd_state *s = httpd_get_state();

  LOG_TRACE("httpd_alloc_state\r\n");

  if (s) {
    LOG_WARN("httpd_alloc_state: connection already allocated!");
    return s;
  }

  s = (struct httpd_state *)malloc(sizeof(struct httpd_state));
  uip_conn->appstate = s;

  memset(s, 0, sizeof(struct httpd_state));
  s->inputbuf_size = INPUTBUF_SIZE;
  s->inputbuf = malloc(INPUTBUF_SIZE);

  LOG_TRACE("httpd_alloc_state: %p, buf: %p\r\n", s, s->inputbuf);

  PSOCK_INIT(&s->sin, s->inputbuf, s->inputbuf_size);

  return s;
}

static void httpd_free_state()
{
  struct httpd_state *s = httpd_get_state();

  LOG_TRACE("httpd_free_state\r\n");

  if (!s) {
    LOG_WARN("httpd_free_state: connection already freed!");
    return;
  }

  struct DataSource* src = s->handler_datasrc;
  if (src) {
      src->close(src);
  }
  /* free other details */
  free(s->inputbuf);
  free(s);
  uip_conn->appstate = NULL;

}

/*---------------------------------------------------------------------------*/

void
httpd_appcall(void)
{
  struct httpd_state *s = httpd_get_state();

  if (s) {
      void (*idle_run_handler)(struct httpd_state *s);
      idle_run_handler = s->idle_run_handler;
      /* connection is already established */
      if (uip_timedout()) {
        LOG_TRACE("Connection timeout\r\n");
      } else if(uip_aborted()) {
        LOG_TRACE("Connection aborted\r\n");
      } else if(uip_closed()) {
        LOG_TRACE("Connection closed\r\n");
        /* allow connection handler to do it's cleanup if connection was closed while
          calling into UIP which would result in this code being executed and thead
          never resumed again so that it would have no chance of cleaning up after itself.
          */
        handle_connection(s);
      } else {
        /* connection is active, service the connection*/
        handle_connection(s);
        return;
      }
      /* now check if there's and outstanding error */
      handle_error(s);
      httpd_free_state();

      if(idle_run_handler){
        idle_run_handler(s);
      }
      return;
  }

 if(uip_connected()) {
    LOG_TRACE("Connection established\r\n");
    /* new connection */
    httpd_alloc_state();
  } else {
    /* pass */
    /* Unknown condition, do nothing */
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Initialize the web server
 *
 *             This function initializes the web server and should be
 *             called at system boot-up.
 */
void
httpd_init(void)
{
  uip_listen(HTONS(80));
}
/*---------------------------------------------------------------------------*/
/** @} */

#endif /* !WITHOUT_HTTP */
