#include "toolbox.h"
#include <stdlib.h>

// --- missing from your toolbox.h: the VCOUNT register 0x04000006
#define REG_VCOUNT (*(volatile u16 *)(MEM_IO + 0x0006))

// INLINE is already "static inline"
INLINE void wait_vblank(void)
{
  // wait until VCOUNT rolls over past 159 (i.e. into VBlank)
  while (REG_VCOUNT >= SCREEN_HEIGHT)
    ;
  while (REG_VCOUNT < SCREEN_HEIGHT)
    ;
}

int main(void)
{
  // Mode 3, BG2 on
  REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

  // Seed (use a fixed seed for reproducible “random” or better yet hook
  // into a free-running timer if you want true variability)
  srand(1234);

  // Starting position
  int x = 120, y = 80;

  // Draw the first pixel
  m3_plot(x, y, CLR_RED);

  while (1)
  {
    wait_vblank();

    // Erase old pixel (draw black)
    m3_plot(x, y, CLR_BLACK);

    // Move by -1, 0 or +1 in each axis
    int dx = (rand() % 3) - 1;
    int dy = (rand() % 3) - 1;
    x += dx;
    y += dy;

    // Clamp to [0..SCREEN_WIDTH-1], [0..SCREEN_HEIGHT-1]
    if (x < 0)
      x = 0;
    else if (x >= SCREEN_WIDTH)
      x = SCREEN_WIDTH - 1;
    if (y < 0)
      y = 0;
    else if (y >= SCREEN_HEIGHT)
      y = SCREEN_HEIGHT - 1;

    // Draw new pixel
    m3_plot(x, y, CLR_RED);
  }

  return 0;
}
