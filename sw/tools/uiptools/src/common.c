#include "common.h"
#include "logging.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>

_DTA  dta;  /* global! */

/*---------------------------------------------------------------------------*/

void fstrcat(struct Repsonse* response, char* format, ...)
{
  char formated[256];
  va_list args;
  size_t formated_len = 0;

  va_start (args, format);
  formated_len = vsnprintf (formated, sizeof(formated), format, args);

  if (response->size  <= (response->current-response->malloc_block) + formated_len) {
    size_t current_offset = response->current-response->malloc_block;
    LOG_TRACE("realloc: %zu->%zu\r\n", response->size, response->size*2);
    response->size = response->size * 2;
    response->malloc_block = realloc(response->malloc_block,response->size);
    response->current = &response->malloc_block[current_offset];
    LOG_TRACE("realloc: %p\r\n", response->malloc_block);
  }

  strcat(response->current, formated);
  response->current = &response->current[ formated_len ];
  va_end (args);
}

/*---------------------------------------------------------------------------*/

/*
 * Combine two paths into one. Note that the function
 * will write to the specified buffer, which has to
 * be allocated beforehand.
 *
 * @pth1: Part one of the path
 * @pth2: Part two of the path
*/

void joinpath(char *pth1, const char *pth2)
{
    if(pth2 == NULL) {
        strcpy(pth1, "");
    } else if(*pth2 == '/') {
        // pth2 is absolute so replace pth1
        strcpy(pth1, pth2);
    } else {
        char directory_separator[] = "/";
        const char *last_char = pth1;
        while(*last_char != '\0')
            last_char++;
        if (pth1 != last_char)
          --last_char; // back track one character so last_char isn't pointing to null termination

        if(*last_char != '/') {
            strcat(pth1, directory_separator);
        }
        strcat(pth1, pth2);
    }

    // colapse all '..' in the path
    char *rm, *fn;
    while((rm = strstr (pth1, "/..")) != NULL) {
        for(fn = (rm - 1); fn >= pth1; fn--) {
            if(*fn == '/') {
                int l = strlen(rm + 3);
                memcpy(fn + 1, rm + 3, l);
                *(fn + l + 1) = 0;
                break;
            }
        }
    }

    // colapse all instances of double delimiters '//'
    while((rm = strstr (pth1, "//")) != NULL) {
      int l = strlen(rm + 1);
      memcpy(rm, rm + 1, l);
      *(rm + l) = 0;
    }

    // remove trailing '/'
    int l = strlen(pth1);
    if (l > 1 && *(pth1 + l - 1) == '/') {
      *(pth1 + l - 1) = '\0';
    }
}

/*---------------------------------------------------------------------------*/

bool createOrCheckForFolder(const char* path)
{
  if (strlen(path) == 2) {
    // This is bare drive (ie. "c:")
    uint32_t drv_map = Drvmap();
    uint32_t drive_num = toupper(path[0]) - 'A';
    return (drv_map>>drive_num)&1;
  }
  Fsetdta (&dta);
  if (0 == Fsfirst(path, FA_DIR|FA_HIDDEN|FA_SYSTEM) && dta.dta_attribute&FA_DIR) {
    return true;
  }
  return Dcreate(path) == 0;
}

bool ensureFolderExists(const char* path, bool stripFileName)
{
  char temp_path[256];
  size_t len = strlen(path);
  strncpy(temp_path, path, sizeof(temp_path) - 1);
  bool ret = true;

  // remove file name from the path file path base
  if (stripFileName) {
    for (; len != 0; --len) {
      if (temp_path[len] == '\\') {
        temp_path[len] = '\0';
        break;
      }
    }
  }
  // remove trailing backslash
  if (temp_path[len - 1] == '\\') {
    temp_path[len - 1] = '\0';
  }
  // skip the drive letter in the path
  for (size_t i = 3; i < len + 1; ++i) {
    if (temp_path[i] == '\\') {
      temp_path[i] = '\0';
      ret &= createOrCheckForFolder(temp_path);
      temp_path[i] = '\\';
    } else if (temp_path[i] == '\0') {
      ret &= createOrCheckForFolder(temp_path);
    }
  }

  return ret;
}

/*---------------------------------------------------------------------------*/

bool deletePath(const char* path)
{
  Fsetdta (&dta);

  if (0 == Fsfirst(path, FA_DIR|FA_HIDDEN|FA_SYSTEM)) {
    if(dta.dta_attribute&FA_DIR) {
      return 0 == Ddelete(path);
    } else {
      return 0 == Fdelete(path);
    }
  }
  return false;
}


bool getFileSize(const char* path, int* size)
{
  Fsetdta (&dta);

  if (0 == Fsfirst(path, FA_DIR|FA_HIDDEN|FA_SYSTEM)) {
    *size = dta.dta_size;
    return true;
  }
  return false;
}

/*---------------------------------------------------------------------------*/

void convertToGemDosPath(char* path)
{
  char *fn = path;
  int l = strlen(path);

  /* convert path to dos/atari format */

  for (int i = 0; i < l; i++) {
    if (path[i] == '/') {
      path[i] = '\\';
    }
  }

  path[0] = path[1]&0x7f;
  path[1] = ':';
  path[2] = '\\';
}

/*---------------------------------------------------------------------------*/
/*
  Valid GEMDOS characters according to Atari Compendium:

  A-Z, a-z, 0-9
  ! @ # $ % ^ & ( )
  + - = ~ ` ; ‘ “ ,
  < > | [ ] ( ) _
*/

bool isValidGemDosNameChar[128] = {
  false, //0  NUL (null)
  false, //1  SOH (start of heading)
  false, //2  STX (start of text)
  false, //3  ETX (end of text)
  false, //4  EOT (end of transmission)
  false, //5  ENQ (enquiry)
  false, //6  ACK (acknowledge)
  false, //7  BEL (bell)
  false, //8  BS  (backspace)
  false, //9  TAB (horizontal tab)
  false, //10  LF  (NL line feed, new line)
  false, //11  VT  (vertical tab)
  false, //12  FF  (NP form feed, new page)
  false, //13  CR  (carriage return)
  false, //14  SO  (shift out)
  false, //15  SI  (shift in)
  false, //16  DLE (data link escape)
  false, //17  DC1 (device control 1)
  false, //18  DC2 (device control 2)
  false, //19  DC3 (device control 3)
  false, //20  DC4 (device control 4)
  false, //21  NAK (negative acknowledge)
  false, //22  SYN (synchronous idle)
  false, //23  ETB (end of trans. block)
  false, //24  CAN (cancel)
  false, //25  EM  (end of medium)
  false, //26  SUB (substitute)
  false, //27  ESC (escape)
  false, //28  FS  (file separator)
  false, //29  GS  (group separator)
  false, //30  RS  (record separator)
  false, //31  US  (unit separator)
  false, //32  SPACE
  true, //33  !
  true, //34  "
  true, //35  #
  true, //36  $
  true, //37  %
  true, //38  &
  true, //39  '
  true, //40  (
  true, //41  )
  false, //42  *
  true, //43  +
  true, //44  ,
  true, //45  -
  true, //46  .
  false, //47  /
  true, //48  0
  true, //49  1
  true, //50  2
  true, //51  3
  true, //52  4
  true, //53  5
  true, //54  6
  true, //55  7
  true, //56  8
  true, //57  9
  false, //58  :
  true, //59  ;
  true, //60  <
  true, //61  =
  true, //62  >
  false, //63  ?
  true, //64  @
  true, //65  A
  true, //66  B
  true, //67  C
  true, //68  D
  true, //69  E
  true, //70  F
  true, //71  G
  true, //72  H
  true, //73  I
  true, //74  J
  true, //75  K
  true, //76  L
  true, //77  M
  true, //78  N
  true, //79  O
  true, //80  P
  true, //81  Q
  true, //82  R
  true, //83  S
  true, //84  T
  true, //85  U
  true, //86  V
  true, //87  W
  true, //88  X
  true, //89  Y
  true, //90  Z
  true, //91  [
  false, //92  \    - not allowed in file name, but allowed in a path!
  true, //93  ]
  true, //94  ^
  true, //95  _
  true, //96  `
  true, //97  a
  true, //98  b
  true, //99  c
  true, //100  d
  true, //101  e
  true, //102  f
  true, //103  g
  true, //104  h
  true, //105  i
  true, //106  j
  true, //107  k
  true, //108  l
  true, //109  m
  true, //110  n
  true, //111  o
  true, //112  p
  true, //113  q
  true, //114  r
  true, //115  s
  true, //116  t
  true, //117  u
  true, //118  v
  true, //119  w
  true, //120  x
  true, //121  y
  true, //122  z
  false, //123  {
  true, //124  |
  false, //125  }
  true, //126  ~
  false//127  DEL
};

void sanitiseGemDosName(char* path)
{
  while(*path != '\0') {
    if (*path > 127 || !isValidGemDosNameChar[(int)*path]) {
      *path = '_';
    }
    path++;
  }
}

bool isValidGemDosPathChar[128] = {
  false, //0  NUL (null)
  false, //1  SOH (start of heading)
  false, //2  STX (start of text)
  false, //3  ETX (end of text)
  false, //4  EOT (end of transmission)
  false, //5  ENQ (enquiry)
  false, //6  ACK (acknowledge)
  false, //7  BEL (bell)
  false, //8  BS  (backspace)
  false, //9  TAB (horizontal tab)
  false, //10  LF  (NL line feed, new line)
  false, //11  VT  (vertical tab)
  false, //12  FF  (NP form feed, new page)
  false, //13  CR  (carriage return)
  false, //14  SO  (shift out)
  false, //15  SI  (shift in)
  false, //16  DLE (data link escape)
  false, //17  DC1 (device control 1)
  false, //18  DC2 (device control 2)
  false, //19  DC3 (device control 3)
  false, //20  DC4 (device control 4)
  false, //21  NAK (negative acknowledge)
  false, //22  SYN (synchronous idle)
  false, //23  ETB (end of trans. block)
  false, //24  CAN (cancel)
  false, //25  EM  (end of medium)
  false, //26  SUB (substitute)
  false, //27  ESC (escape)
  false, //28  FS  (file separator)
  false, //29  GS  (group separator)
  false, //30  RS  (record separator)
  false, //31  US  (unit separator)
  false, //32  SPACE
  true, //33  !
  true, //34  "
  true, //35  #
  true, //36  $
  true, //37  %
  true, //38  &
  true, //39  '
  true, //40  (
  true, //41  )
  false, //42  *
  true, //43  +
  true, //44  ,
  true, //45  -
  true, //46  .
  false, //47  /
  true, //48  0
  true, //49  1
  true, //50  2
  true, //51  3
  true, //52  4
  true, //53  5
  true, //54  6
  true, //55  7
  true, //56  8
  true, //57  9
  true, //58  :     - for drive name prefix
  true, //59  ;
  true, //60  <
  true, //61  =
  true, //62  >
  false, //63  ?
  true, //64  @
  true, //65  A
  true, //66  B
  true, //67  C
  true, //68  D
  true, //69  E
  true, //70  F
  true, //71  G
  true, //72  H
  true, //73  I
  true, //74  J
  true, //75  K
  true, //76  L
  true, //77  M
  true, //78  N
  true, //79  O
  true, //80  P
  true, //81  Q
  true, //82  R
  true, //83  S
  true, //84  T
  true, //85  U
  true, //86  V
  true, //87  W
  true, //88  X
  true, //89  Y
  true, //90  Z
  true, //91  [
  true, //92  \    - not allowed in file name, but allowed in a path!
  true, //93  ]
  true, //94  ^
  true, //95  _
  true, //96  `
  true, //97  a
  true, //98  b
  true, //99  c
  true, //100  d
  true, //101  e
  true, //102  f
  true, //103  g
  true, //104  h
  true, //105  i
  true, //106  j
  true, //107  k
  true, //108  l
  true, //109  m
  true, //110  n
  true, //111  o
  true, //112  p
  true, //113  q
  true, //114  r
  true, //115  s
  true, //116  t
  true, //117  u
  true, //118  v
  true, //119  w
  true, //120  x
  true, //121  y
  true, //122  z
  false, //123  {
  true, //124  |
  false, //125  }
  true, //126  ~
  false//127  DEL
};

void sanitiseGemDosPath(char* path)
{
  while(*path != '\0') {
    if (*path > 127 || !isValidGemDosPathChar[(int)*path]) {
      *path = '_';
    }
    path++;
  }
  // TODO: shorten dir and file name to 8.3!
}

/*---------------------------------------------------------------------------*/

int Fclose_safe(int16_t* fd)
{
  int16_t _fd = *fd;

  if (_fd > 0) {
    Fclose(_fd);
    *fd = -1;
  }
  return _fd;
}

/*---------------------------------------------------------------------------*/

uint64_t getMicroseconds()
{
  uint64_t timer200hz;
  uint32_t data;
resync:
  timer200hz = *((volatile uint32_t*)0x4BA) ;
  data = *((volatile uint8_t*)0xFFFFFA23);

  if ( *((volatile uint32_t*)0x4BA) != timer200hz )
  {
    goto resync;
  }

  timer200hz*=5000;       // convert to microseconds
  timer200hz+=(uint64_t)(((192-data)*6666)>>8); //26;     // convert data to microseconds
  return timer200hz;
}


/*---------------------------------------------------------------------------*/