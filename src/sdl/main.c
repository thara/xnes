#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <xnes.h>

// TODO DRY
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 224

const int windowScale = 3;

typedef struct {
  uint32_t pixels[SCREEN_WIDTH * 240];
} RenderingState;

void update_frame(void *state, uint8_t *buf, uint64_t len);
void render_frame(RenderingState *renderingState, SDL_Renderer *ren,
                  SDL_Texture *frameTex, SDL_Rect *screenRect);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <rom filename>\n", argv[0]);
    return 1;
  }

  char *filename = argv[1];
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    fprintf(stderr, "fail to open ROM file: %s\n", filename);
    return 2;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    fprintf(stderr, "fail to seek %s\n", filename);
    fclose(file);
    return 3;
  }
  size_t rom_size = ftell(file);
  if (fseek(file, 0, SEEK_SET) != 0) {
    fprintf(stderr, "fail to seek %s\n", filename);
    fclose(file);
    return 4;
  }

  unsigned char *rom_data = malloc(rom_size);
  if (rom_data == NULL) {
    fprintf(stderr, "fail to malloc(%zu)\n", rom_size);
    fclose(file);
    return 5;
  }

  if (fread(rom_data, 1, rom_size, file) != rom_size) {
    fprintf(stderr, "fail to fread\n");
    free(rom_data);
    fclose(file);
    return 6;
  }

  fclose(file);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
    free(rom_data);
    return EXIT_FAILURE;
  }

  int winWidth = SCREEN_WIDTH * windowScale;
  int winHeight = SCREEN_HEIGHT * windowScale;

  SDL_Window *win = SDL_CreateWindow(
      "XNES", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winWidth,
      winHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
  if (win == NULL) {
    fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
    free(rom_data);
    return EXIT_FAILURE;
  }

  SDL_Renderer *ren = SDL_CreateRenderer(
      win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (ren == NULL) {
    fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    free(rom_data);
    return EXIT_FAILURE;
  }
  SDL_RenderSetLogicalSize(ren, winWidth, winHeight);

  SDL_Rect screenRect = {0, 0, winWidth, winHeight};

  SDL_Texture *frameTex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            SCREEN_WIDTH, SCREEN_HEIGHT);
  if (frameTex == NULL) {
    fprintf(stderr, "SDL_CreateTexture Error: %s\n", SDL_GetError());
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    free(rom_data);
    return EXIT_FAILURE;
  }

  RenderingState renderingState = {{0}};

  XNESFrameRenderer frameRenderer = {&renderingState, update_frame, NULL};

  XNES *xnes = xnes_new(&frameRenderer);

  XNESROMParseError error = xnes_insert_cartridge(xnes, rom_data, rom_size);
  if (error != XNES_ROM_PARSE_ERROR_NONE) {
    fprintf(stderr, "xnes_insert_cartridge Error: %d\n", error);
    xnes_release(xnes);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    free(rom_data);
    return EXIT_FAILURE;
  }

  xnes_init(xnes);

  bool quit = false;
  SDL_Event event;
  while (!quit) {
    uint32_t startTicks = SDL_GetTicks();
    uint64_t startPerf = SDL_GetPerformanceCounter();

    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
      case SDL_QUIT:
      case SDL_APP_TERMINATING:
        quit = true;
      }
    }

    xnes_run_frame(xnes);

    render_frame(&renderingState, ren, frameTex, &screenRect);

    uint64_t endPerf = SDL_GetPerformanceCounter();
    uint64_t framePerf = ((double)(endPerf - startPerf)) /
                         (double)SDL_GetPerformanceFrequency() * 1000;
    if (0 < 16.666 - framePerf) {
      // Capping 60 FPS
      SDL_Delay(16.666 - framePerf);
    }
  }

  xnes_release(xnes);

  SDL_DestroyTexture(frameTex);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return EXIT_SUCCESS;
}

uint32_t pallete[255] = {
    0x7C7C7CFF, 0x0000FCFF, 0x0000BCFF, 0x4428BCFF, 0x940084FF, 0xA80020FF,
    0xA81000FF, 0x881400FF, 0x503000FF, 0x007800FF, 0x006800FF, 0x005800FF,
    0x004058FF, 0x000000FF, 0x000000FF, 0x000000FF, 0xBCBCBCFF, 0x0078F8FF,
    0x0058F8FF, 0x6844FCFF, 0xD800CCFF, 0xE40058FF, 0xF83800FF, 0xE45C10FF,
    0xAC7C00FF, 0x00B800FF, 0x00A800FF, 0x00A844FF, 0x008888FF, 0x000000FF,
    0x000000FF, 0x000000FF, 0xF8F8F8FF, 0x3CBCFCFF, 0x6888FCFF, 0x9878F8FF,
    0xF878F8FF, 0xF85898FF, 0xF87858FF, 0xFCA044FF, 0xF8B800FF, 0xB8F818FF,
    0x58D854FF, 0x58F898FF, 0x00E8D8FF, 0x787878FF, 0x000000FF, 0x000000FF,
    0xFCFCFCFF, 0xA4E4FCFF, 0xB8B8F8FF, 0xD8B8F8FF, 0xF8B8F8FF, 0xF8A4C0FF,
    0xF0D0B0FF, 0xFCE0A8FF, 0xF8D878FF, 0xD8F878FF, 0xB8F8B8FF, 0xB8F8D8FF,
    0x00FCFCFF, 0xF8D8F8FF, 0x000000FF, 0x000000FF};

void update_frame(void *state, uint8_t *buf, uint64_t len) {
  RenderingState *renderingState = (RenderingState *)state;

  for (int i = 0; i < len; i++) {
    renderingState->pixels[i] = pallete[buf[i]];
  }
}

const int pitch = 256 * sizeof(uint32_t);

void render_frame(RenderingState *renderingState, SDL_Renderer *ren,
                  SDL_Texture *frameTex, SDL_Rect *screenRect) {
  SDL_UpdateTexture(frameTex, NULL, renderingState->pixels, pitch);

  SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(ren);
  SDL_RenderCopy(ren, frameTex, NULL, screenRect);
  SDL_RenderPresent(ren);
}
