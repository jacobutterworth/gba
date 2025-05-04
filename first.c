#include "toolbox.h"
#include <stdlib.h>

// --- missing from your toolbox.h: the VCOUNT register 0x04000006
#define REG_VCOUNT (*(volatile u16 *)(MEM_IO + 0x0006))

// Direction vectors for the ant (right, up, left, down)
const int dx[] = {1, 0, -1, 0};
const int dy[] = {0, -1, 0, 1};

// INLINE is already "static inline"
INLINE void wait_vblank(void)
{
  // wait until VCOUNT rolls over past 159 (i.e. into VBlank)
  while (REG_VCOUNT >= SCREEN_HEIGHT)
    ;
  while (REG_VCOUNT < SCREEN_HEIGHT)
    ;
}

// Function to get pixel color at (x,y)
INLINE u16 m3_get(int x, int y)
{
  return vid_mem[y * SCREEN_WIDTH + x];
}

int main(void)
{
  // Mode 3, BG2 on
  REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

  // Starting position in the middle of the screen
  int x = SCREEN_WIDTH / 2;
  int y = SCREEN_HEIGHT / 2;
  // Start facing upward (index 1 in our direction arrays)
  int dir = 1;

  while (1)
  {
    wait_vblank();

    // Get current cell color
    u16 current_color = m3_get(x, y);

    if (current_color == CLR_BLACK)
    {
      // On black: turn left (add 1 to direction)
      dir = (dir + 1) % 4;
      m3_plot(x, y, CLR_WHITE);
    }
    else
    {
      // On white/unvisited: turn right (subtract 1 from direction)
      dir = (dir + 3) % 4; // same as (dir - 1 + 4) % 4
      m3_plot(x, y, CLR_BLACK);
    }

    // Move forward in current direction
    x += dx[dir];
    y += dy[dir];

    // Wrap around screen edges
    if (x < 0)
      x = SCREEN_WIDTH - 1;
    if (x >= SCREEN_WIDTH)
      x = 0;
    if (y < 0)
      y = SCREEN_HEIGHT - 1;
    if (y >= SCREEN_HEIGHT)
      y = 0;
  }

  return 0;
}
