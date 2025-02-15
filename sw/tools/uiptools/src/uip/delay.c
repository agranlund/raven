#include <stdint.h>
#include "delay.h"

static int64_t getMicroseconds()
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


void Delay_microsec(long delay)
{
    uint64_t future = getMicroseconds() + delay;
    while(future > getMicroseconds());
}