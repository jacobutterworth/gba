#include "toolbox.h"
#include <stdlib.h>

// --- missing from your toolbox.h: the VCOUNT register 0x04000006
#define REG_VCOUNT (*(volatile u16 *)(MEM_IO + 0x0006))
#define REG_KEYINPUT (*(volatile u16 *)0x4000130)

// Key masks
#define KEY_RIGHT 0x0010
#define KEY_LEFT 0x0020
#define KEY_UP 0x0040
#define KEY_DOWN 0x0080
#define KEY_MASK 0x03FF

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

// Function to read key input (returns true if key is pressed)
INLINE int key_hit(int key)
{
  return (~REG_KEYINPUT & key);
}

int main(void)
{
  // Mode 3, BG2 on
  REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

  // Starting position in the middle of the screen
  int x = SCREEN_WIDTH / 2;
  int y = SCREEN_HEIGHT / 2;
  int dir = 1; // Start facing upward

  // Variables to track previous key state
  int prev_keys = 0;

  while (1)
  {
    wait_vblank();

    // Get current key state
    int curr_keys = ~REG_KEYINPUT & KEY_MASK;
    int new_presses = curr_keys & ~prev_keys;

    // Handle manual movement
    if (new_presses & KEY_RIGHT)
    {
      x++;
    }
    else if (new_presses & KEY_LEFT)
    {
      x--;
    }
    else if (new_presses & KEY_UP)
    {
      y--;
    }
    else if (new_presses & KEY_DOWN)
    {
      y++;
    }
    else
    {
      // No key pressed - do automatic Langton's Ant movement
      u16 current_color = m3_get(x, y);

      if (current_color == CLR_BLACK)
      {
        // On black: turn left
        dir = (dir + 1) % 4;
        m3_plot(x, y, CLR_WHITE);
      }
      else
      {
        // On white/unvisited: turn right
        dir = (dir + 3) % 4; // same as (dir - 1 + 4) % 4
        m3_plot(x, y, CLR_BLACK);
      }

      // Move forward in current direction
      x += dx[dir];
      y += dy[dir];
    }

    // Wrap around screen edges
    if (x < 0)
      x = SCREEN_WIDTH - 1;
    if (x >= SCREEN_WIDTH)
      x = 0;
    if (y < 0)
      y = SCREEN_HEIGHT - 1;
    if (y >= SCREEN_HEIGHT)
      y = 0;

    // If we moved by key press, flip the color
    if (new_presses & (KEY_RIGHT | KEY_LEFT | KEY_UP | KEY_DOWN))
    {
      u16 current_color = m3_get(x, y);
      m3_plot(x, y, current_color == CLR_BLACK ? CLR_WHITE : CLR_BLACK);
    }

    // Store current key state for next frame
    prev_keys = curr_keys;
  }

  return 0;
}
