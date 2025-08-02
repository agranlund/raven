#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define TOC_OFFSET 0x400

typedef struct
{
    uint32_t id;
    uint32_t start;
    uint32_t size;
    uint32_t extra;
} TOC;


static uint8_t tempbuf[64*1024];
static uint8_t fbuf[16 * 1024 * 1024];

uint32_t crc32(uint8_t* data, uint32_t len) {
    return 0;
}

uint32_t swap32(uint32_t d) {
    return  ((d & 0xff000000) >> 24) |
            ((d & 0x00ff0000) >>  8) |
            ((d & 0x0000ff00) <<  8) |
            ((d & 0x000000ff) << 24);
}

uint32_t read32(uint32_t offs) {
    return swap32(*((uint32_t*)&fbuf[offs]));
}

void write32(uint32_t offs, uint32_t data) {
    *((uint32_t*)&fbuf[offs]) = swap32(data);
}

uint32_t loadfile(const char* fname, uint8_t* buf) {
    uint32_t size = 0;
    FILE* f = fopen(fname, "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        if (size > 0) {
            fseek(f, 0, SEEK_SET);
            fread(buf, size, 1, f);
        }
        fclose(f);
    }
    return size;
}

bool savefile(const char* fname, uint8_t* buf, uint32_t size) {
    FILE* f = fopen(fname, "wb");
    if (f) {
        fwrite(buf, size, 1, f);
        fclose(f);
        return true;
    }
    return false;
}

int main(int args, char* argv[])
{
    if (args < 4) {
        return -1;
    }
    const char* fo_name = argv[1];
    const char* fi_name = argv[3];
    const char* id = argv[2];
    if (strlen(id) != 4) {
        return -2;
    }

    uint32_t fosize = loadfile(fo_name, &fbuf[0]);
    uint32_t align = (4 * 1024);
    uint32_t fistart = (fosize + align - 1) & ~(align - 1); 
    uint32_t fisize = loadfile(fi_name, &fbuf[fistart]);
    fosize = fistart + fisize;
    if (fisize == 0) {
        return -3;
    }

    // create data entry
    TOC* t = (TOC*)&fbuf[TOC_OFFSET];
    uint32_t rombase = swap32(t->start);

    while(t->id) { t++; }
    t->id = *((uint32_t*)id);
    t->start = swap32(fistart+rombase);
    t->size = swap32(fisize);
    t->extra = 0;

    // update toc
    t = (TOC*)&fbuf[TOC_OFFSET];
    t->size = swap32(fosize);
    t->extra = 0;
    t->extra = swap32(crc32(fbuf, fosize));

    savefile(fo_name, fbuf, fosize);
    return 0;
}
