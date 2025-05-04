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
#define KEY_A 0x0001
#define KEY_MASK 0x03FF

// Maximum number of ants
#define MAX_ANTS 10

// Direction vectors for the ant (right, up, left, down)
const int dx[] = {1, 0, -1, 0};
const int dy[] = {0, -1, 0, 1};

// Structure to hold ant data
typedef struct
{
  int x;
  int y;
  int dir;
  int active;
} Ant;

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

// Function to spawn a new ant at a random location
void spawn_ant(Ant *ants, int index)
{
  ants[index].x = rand() % SCREEN_WIDTH;
  ants[index].y = rand() % SCREEN_HEIGHT;
  ants[index].dir = rand() % 4;
  ants[index].active = 1;
}

int main(void)
{
  // Mode 3, BG2 on
  REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

  // Initialize random number generator
  srand(REG_VCOUNT);

  // Create array of ants
  Ant ants[MAX_ANTS] = {0};
  int num_ants = 1;

  // Initialize first ant in the middle
  ants[0].x = SCREEN_WIDTH / 2;
  ants[0].y = SCREEN_HEIGHT / 2;
  ants[0].dir = 1;
  ants[0].active = 1;

  // Variables to track previous key state
  int prev_keys = 0;

  while (1)
  {
    wait_vblank();

    // Get current key state
    int curr_keys = ~REG_KEYINPUT & KEY_MASK;
    int new_presses = curr_keys & ~prev_keys;

    // Handle A button press - spawn new ant
    if ((new_presses & KEY_A) && num_ants < MAX_ANTS)
    {
      spawn_ant(ants, num_ants);
      num_ants++;
    }

    // Update all active ants
    for (int i = 0; i < num_ants; i++)
    {
      if (!ants[i].active)
        continue;

      // Handle manual movement for the first ant only
      if (i == 0)
      {
        if (new_presses & KEY_RIGHT)
        {
          ants[i].x++;
        }
        else if (new_presses & KEY_LEFT)
        {
          ants[i].x--;
        }
        else if (new_presses & KEY_UP)
        {
          ants[i].y--;
        }
        else if (new_presses & KEY_DOWN)
        {
          ants[i].y++;
        }
        else
        {
          // No key pressed - do automatic Langton's Ant movement
          u16 current_color = m3_get(ants[i].x, ants[i].y);

          if (current_color == CLR_BLACK)
          {
            // On black: turn left
            ants[i].dir = (ants[i].dir + 1) % 4;
            m3_plot(ants[i].x, ants[i].y, CLR_WHITE);
          }
          else
          {
            // On white/unvisited: turn right
            ants[i].dir = (ants[i].dir + 3) % 4;
            m3_plot(ants[i].x, ants[i].y, CLR_BLACK);
          }

          // Move forward in current direction
          ants[i].x += dx[ants[i].dir];
          ants[i].y += dy[ants[i].dir];
        }
      }
      else
      {
        // Automatic movement for all other ants
        u16 current_color = m3_get(ants[i].x, ants[i].y);

        if (current_color == CLR_BLACK)
        {
          ants[i].dir = (ants[i].dir + 1) % 4;
          m3_plot(ants[i].x, ants[i].y, CLR_WHITE);
        }
        else
        {
          ants[i].dir = (ants[i].dir + 3) % 4;
          m3_plot(ants[i].x, ants[i].y, CLR_BLACK);
        }

        ants[i].x += dx[ants[i].dir];
        ants[i].y += dy[ants[i].dir];
      }

      // Wrap around screen edges
      if (ants[i].x < 0)
        ants[i].x = SCREEN_WIDTH - 1;
      if (ants[i].x >= SCREEN_WIDTH)
        ants[i].x = 0;
      if (ants[i].y < 0)
        ants[i].y = SCREEN_HEIGHT - 1;
      if (ants[i].y >= SCREEN_HEIGHT)
        ants[i].y = 0;

      // If we moved first ant by key press, flip the color
      if (i == 0 && (new_presses & (KEY_RIGHT | KEY_LEFT | KEY_UP | KEY_DOWN)))
      {
        u16 current_color = m3_get(ants[i].x, ants[i].y);
        m3_plot(ants[i].x, ants[i].y, current_color == CLR_BLACK ? CLR_WHITE : CLR_BLACK);
      }
    }

    // Store current key state for next frame
    prev_keys = curr_keys;
  }

  return 0;
}
