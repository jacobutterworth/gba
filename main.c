#include <gba_video.h>
#include <gba_input.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160
#define RGB15(r, g, b) ((r) | ((g) << 5) | ((b) << 10))
#define BLUE RGB15(0, 0, 31)

int dotX = SCREEN_WIDTH / 2;
int dotY = SCREEN_HEIGHT / 2;

void drawPixel(int x, int y, unsigned short color)
{
  unsigned short *vram = (unsigned short *)0x06000000;
  vram[y * SCREEN_WIDTH + x] = color;
}

int main(void)
{
  irqInit();
  irqEnable(IRQ_VBLANK);

  SetMode(MODE_3 | BG2_ENABLE);

  while (1)
  {
    // Read keys directly (active-low)
    u16 keys = ~REG_KEYINPUT;

    // Clear previous dot
    drawPixel(dotX, dotY, 0);

    // Update position
    if (keys & KEY_RIGHT && dotX < SCREEN_WIDTH - 1)
      dotX++;
    if (keys & KEY_LEFT && dotX > 0)
      dotX--;
    if (keys & KEY_DOWN && dotY < SCREEN_HEIGHT - 1)
      dotY++;
    if (keys & KEY_UP && dotY > 0)
      dotY--;

    // Draw new dot
    drawPixel(dotX, dotY, BLUE);

    VBlankIntrWait();
  }

  return 0;
}
