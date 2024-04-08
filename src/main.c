// vim:fileencoding=utf-8:foldmethod=marker

// Defines and includes {{{
#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// TODO: Add player, art and music, score system;

#define SCR_WIDTH 600
#define SCR_HEIGHT 700
#define CAPTION "Catcher"
#define FPS 60
#define SPRITE_SIZE 64
#define NUM_ITEMS 10
#define MIN_Y_SPEED 200
#define MAX_Y_SPEED 400
#define SPAWN_TIME 1.5f
#define SPAWN_POS -50
#define PLAYER_LIVES 3
// }}}

// Declarations {{{
typedef enum State { SPLASH, TITLE, PLAYING, ENDGAME } State;

typedef enum ItemType {
  BUMNOTE,
  NOTE,
  VIOLIN,
  VIOLA,
  CELLO,
  DBLBASS,
  NUMOFTYPES,
} ItemType;

typedef struct Item {
  Vector2 pos;
  int ySpeed;
  bool active;
  enum ItemType type;
} Item;

typedef struct Player {
  Vector2 pos;
  int lives;
  int score;
} Player;

typedef struct Game {
  int numItems;
  Item items[NUM_ITEMS];
  Player player;
  bool gameOver;
} Game;

// Function declarations
void init(Game *game);
void spawn(Game *game);
void update(Game *game);
void draw(Game *game);
void drawItems(Game *game);
int getRandom(int min, int max);
// }}}

int main(void) {
  InitWindow(SCR_WIDTH, SCR_HEIGHT, CAPTION); // Create Window
  SetTargetFPS(FPS);                          // Set FPS
  State currentState = SPLASH;
  srand(time(NULL));
  Game game = {};
  double spawnTimer = 0;
  int framesCounter = 0;

  while (!WindowShouldClose()) {

    switch (currentState) {
    case SPLASH:
      framesCounter++; // Count frames
      // Wait for 2 seconds (120 frames) before jumping to TITLE screen
      if (framesCounter > 120) {
        currentState = TITLE;
      }
      break;

    case TITLE:
      if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        currentState = PLAYING;
        init(&game);
      }
      break;

    case PLAYING:
      if (game.gameOver) {
        currentState = ENDGAME;
      }

      if (IsKeyPressed(KEY_X)) {
        currentState = ENDGAME;
      }
      break;

    case ENDGAME:
      if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        currentState = TITLE;
      }
      break;
    }

    update(&game);

    BeginDrawing();
    ClearBackground(BLACK);

    switch (currentState) {
    case SPLASH:
      DrawText(CAPTION, (int)(GetScreenWidth() - MeasureText(CAPTION, 30)) / 2,
               350, 30, SKYBLUE);
      break;

    case TITLE:
      DrawText(CAPTION, (int)(GetScreenWidth() - MeasureText(CAPTION, 30)) / 2,
               10, 30, SKYBLUE);
      DrawText(
          "Press Enter To Start",
          (int)(GetScreenWidth() - MeasureText("Press Enter To Start", 30)) / 2,
          600, 30, SKYBLUE);
      break;

    case PLAYING:

      draw(&game);

      spawnTimer = spawnTimer + GetFrameTime();
      if (spawnTimer >= SPAWN_TIME) {
        spawn(&game);
        spawnTimer = 0;
      }
      break;

    case ENDGAME:
      DrawText(CAPTION, (int)(GetScreenWidth() - MeasureText(CAPTION, 30)) / 2,
               10, 30, SKYBLUE);
      DrawText("Game Over",
               (int)(GetScreenWidth() - MeasureText("Game Over", 30)) / 2, 150,
               30, SKYBLUE);
      DrawText("Score", (int)(GetScreenWidth() - MeasureText("Score", 30)) / 2,
               300, 30, SKYBLUE);
      DrawText(TextFormat("%d", game.player.score),
               (int)(GetScreenWidth() -
                     MeasureText(TextFormat("%d", game.player.score), 30)) /
                   2,
               340, 30, SKYBLUE);
      DrawText(
          "Press Enter To Continue",
          (int)(GetScreenWidth() - MeasureText("Press Enter To Continue", 30)) /
              2,
          600, 30, SKYBLUE);

      break;

    default:
      break;
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

// init {{{
void init(Game *game) {
  game->numItems = NUM_ITEMS;
  game->gameOver = false;
  game->player.pos.x = (GetScreenWidth() - SPRITE_SIZE) / 2.0f;
  game->player.pos.y = (GetScreenHeight() - SPRITE_SIZE * 1.1f);
  game->player.lives = PLAYER_LIVES;

  // Initialize game items
  for (int i = 0; i < game->numItems; ++i) {
    game->items[i].pos.x = rand() % (SCR_WIDTH - SPRITE_SIZE);
    game->items[i].pos.y = SPAWN_POS;
    game->items[i].type = rand() % NUMOFTYPES;
    game->items[i].ySpeed = getRandom(MIN_Y_SPEED, MAX_Y_SPEED);
    game->items[i].active = false;
  }
}
// }}}

// spawn {{{
void spawn(Game *game) {
  for (int i = 0; i < game->numItems; ++i) {
    if (game->items[i].active)
      continue;
    else {
      game->items[i].pos.y = SPAWN_POS;
      game->items[i].active = true;
      break;
    }
  }
}
// }}}

// update {{{
void update(Game *game) {

  // Update Player
  int dir = 0;
  if (IsKeyDown(KEY_LEFT)) {
    dir = -1;
  }

  if (IsKeyDown(KEY_RIGHT)) {
    dir = 1;
  }

  game->player.pos.x += dir * 100 * GetFrameTime();

  // Update Items
  for (int i = 0; i < game->numItems; ++i) {
    if (game->items[i].active) {
      game->items[i].pos.y += game->items[i].ySpeed * GetFrameTime();

      if (game->items[i].pos.y > GetScreenHeight()) {
        game->items[i].pos.y = SPAWN_POS; // TODO: replace this magic number
        continue;
      }

      // Check collisions
      bool collision = false;
      Rectangle itemBox = {game->items[i].pos.x, game->items[i].pos.y, SPRITE_SIZE, SPRITE_SIZE};
      Rectangle playerBox = {game->player.pos.x, game->player.pos.y, SPRITE_SIZE, SPRITE_SIZE};

      collision = CheckCollisionRecs(itemBox, playerBox);

      if (collision) {
        if (game->items[i].type == BUMNOTE) {
          game->player.lives--;
        } else {
          game->player.score++;
        }

        printf("%i", game->items[i].type);
        game->items[i].type = rand() % NUMOFTYPES;
        game->items[i].active = false;
      }

      if (game->player.lives == 0) {
        game->gameOver = true;
      }
    }
  }
}
// }}}

// draw {{{
void draw(Game *game) {
  drawItems(game);

  // Draw Score
  DrawText(TextFormat("Score: %d", game->player.score), 10, 10, 30, RAYWHITE);

  // Draw Lives
  int txtWidth = MeasureText(TextFormat("Score: %d", game->player.lives), 30);
  DrawText(TextFormat("Lives: %d", game->player.lives), GetScreenWidth() - (txtWidth + 10), 10, 30, RAYWHITE);

  // Draw Player
  DrawRectangleV(game->player.pos, (Vector2){SPRITE_SIZE, SPRITE_SIZE},
                 DARKBLUE);
}
// }}}

// drawItems {{{
void drawItems(Game *game) {
  for (int i = 0; i < game->numItems; ++i) {
    if (!game->items[i].active)
      continue;

    Color color;

    switch (game->items[i].type) {
    case BUMNOTE:
      color = RED;
      break;
    case NOTE:
      color = GREEN;
      break;
    case VIOLIN:
      color = YELLOW;
      break;
    case VIOLA:
      color = ORANGE;
      break;
    case CELLO:
      color = PINK;
    case DBLBASS:
      color = BLUE;
    default:
      break;
    }

    DrawRectangleV(game->items[i].pos, (Vector2){SPRITE_SIZE, SPRITE_SIZE},
                   color);
  }
}
// }}}

int getRandom(int min, int max) {
  return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}
