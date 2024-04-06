#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define TITLE "Image Grid"
#define FPS 60
#define SPRITE_SIZE 64
#define NUM_ITEMS 20
#define MIN_Y_SPEED 200
#define MAX_Y_SPEED 400

enum itemType {
  bumNote,
  note,
  violin,
  viola,
  cello,
  dblBass,
  NumOfTypes,
};

typedef struct Item {
  Vector2 pos;
  int ySpeed;
  bool active;
  enum itemType type;
} Item;

typedef struct Game {
  int numItems;
  Item items[NUM_ITEMS];
} Game;

void init(Game *game);
void update(Game *game);
void drawItems(Game *game);
int getRandom(int min, int max);

int main(void) {
  srand(time(NULL));
  InitWindow(SCR_WIDTH, SCR_HEIGHT, TITLE);
  SetTargetFPS(FPS);

  Game game = {};
  init(&game);

  while (!WindowShouldClose()) {

    update(&game);

    BeginDrawing();
    ClearBackground(BLACK);

    drawItems(&game);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

void init(Game *game) {
  game->numItems = NUM_ITEMS;

  for (int i = 0; i < game->numItems; ++i) {
    game->items[i].pos.x = rand() % (SCR_WIDTH - SPRITE_SIZE);
    game->items[i].pos.y = 50;
    game->items[i].type = rand() % NumOfTypes;
    game->items[i].ySpeed = getRandom(MIN_Y_SPEED, MAX_Y_SPEED);
    game->items[i].active = true;
  }
}

void update(Game *game) {
  for (int i = 0; i < game->numItems; ++i) {
    game->items[i].pos.y += game->items[i].ySpeed * GetFrameTime();

    if (game->items[i].pos.y > GetScreenHeight())
      game->items[i].pos.y = -100;
    }
}

void drawItems(Game *game) {
  for (int i = 0; i < game->numItems; ++i) {
    if (!game->items[i].active)
      continue;

    Color color;

    switch (game->items[i].type) {
    case bumNote:
      color = PURPLE;
      break;
    case note:
      color = GREEN;
      break;
    case violin:
      color = YELLOW;
      break;
    case viola:
      color = ORANGE;
      break;
    case cello:
      color = PINK;
    case dblBass:
      color = BLUE;
    default:
      break;
    }

    DrawRectangleV(game->items[i].pos, (Vector2){SPRITE_SIZE, SPRITE_SIZE},
                   color);
  }
}

int getRandom(int min, int max) {
  return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}
