#include "toolbox.h"

// extra stuff, also in tonc_video.h
#define M3_WIDTH SCREEN_WIDTH
// typedef for a whole mode3 line
typedef COLOR M3LINE[M3_WIDTH];
// m3_mem is a matrix; m3_mem[y][x] is pixel (x,y)
#define m3_mem ((M3LINE *)MEM_VRAM)

int main()
{
  REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

  m3_mem[80][120] = CLR_RED;
  m3_mem[80][136] = CLR_LIME;
  m3_mem[96][120] = CLR_BLUE;

  while (1)
    ;
  return 0;
}
