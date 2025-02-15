#include <osbind.h>
#include <stdio.h>
#include <stdint.h>

uint8_t packet_out[1500];

void readPacket()
{
    uint8_t* localBuffer = packet_out;
    size_t length = sizeof(packet_out);
    asm volatile
    (
        "   move.l  %0,a0               \n"
        "   move.l  #0xfa2000,a1        \n"
        "   move.l  %1,d0               \n"
        "   move.l  d0,d2               \n"
        "   lsr.w   #4,d0               \n"
        "   and.w   #0xf,d2             \n"
        "   add.w   d2,d2               \n"
        "   neg.w   d2                  \n"
        "   jmp     2f(pc,d2.w)         \n"
        "1: move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "   move.b  (a1),(a0)+          \n"
        "2: dbf     d0,1b               \n"
    :
    :   "g"( localBuffer ),
        "g"( length )
    :    "a0","a1","d0","d2"
    );
}

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

int
main(int argc, char *argv[])
{
    Super(0);
    printf("testing...\r\n");

    uint64_t start = getMicroseconds();
    uint32_t numPackets = 10000;

    for(size_t i = 0; i < numPackets; ++i) {
        readPacket();
    }

    uint32_t packets_per_second = ((uint64_t)numPackets * 1000000LL) / (getMicroseconds() - start);

    printf("done: %u packets/sec, %u\r\n",  packets_per_second, packets_per_second * 1500);
}