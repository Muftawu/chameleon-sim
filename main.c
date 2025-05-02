#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int width = 800;
int height = 800;
const char *title = "Chameleon Color Change Simulation";
const char *target_color_text = "Target Color";

#define MIN_PID_VALUE 0
#define MAX_PID_VALUE 255

Color TARGET_COLOR = GREEN;

int correlation_threshold = 5;
int ENABLE_MOTION = 0;
int PRINT_OUTPUT = 0;
int FLIP_IMAGE = 0;
int IMG_WIDTH = 0;
int IMG_HEIGHT = 0;

// (10, 10, 255, 100) => 0.66, 0.039, 0.0029

#define Kp 0.66
#define Ki 0.039
#define Kd 0.0029

// proportional component
int proportional_r;
int proportional_g;
int proportional_b;

// integral component
int integral_r;
int integral_g;
int integral_b;

// derivative component
int derivative_r;
int derivative_g;
int derivative_b;

// error change
int previous_proportional_r;
int previous_proportional_g;
int previous_proportional_b;

typedef struct Chameleon {
  float x, y;
  float speed_x, speed_y;
  Color color;
  char *image_path;
  Texture2D texture;
  Image image;
} Chameleon;

void render_chameleon(Chameleon cam) {
    if (FLIP_IMAGE) ImageFlipHorizontal(&cam.image);
    IMG_WIDTH = cam.image.width;
    IMG_HEIGHT = 0.69*cam.image.height;
    cam.texture = LoadTextureFromImage(cam.image);
    DrawTexture(cam.texture, cam.x-cam.image.width/2.0, cam.y-cam.image.width/2.0, cam.color);
}

int Clamp(int min, int max, int value) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

Color map_input_color (const char *input)
{
    if (strcmp(input, "gray") == 0) return GRAY;
    if (strcmp(input, "yellow") == 0) return YELLOW;
    if (strcmp(input, "gold") == 0) return GOLD;
    if (strcmp(input, "orange") == 0) return ORANGE;
    if (strcmp(input, "pink") == 0) return PINK;
    if (strcmp(input, "red") == 0) return RED;
    if (strcmp(input, "maroon") == 0) return MAROON;
    if (strcmp(input, "green") == 0) return GREEN;
    if (strcmp(input, "lime") == 0) return LIME;
    if (strcmp(input, "skyblue") == 0) return SKYBLUE;
    if (strcmp(input, "blue") == 0) return BLUE;
    if (strcmp(input, "purple") == 0) return PURPLE;
    if (strcmp(input, "violet") == 0) return VIOLET;
    if (strcmp(input, "beige") == 0) return BEIGE;
    if (strcmp(input, "brown") == 0) return BROWN;
    if (strcmp(input, "white") == 0) return WHITE;
    if (strcmp(input, "black") == 0) return BLACK;
    if (strcmp(input, "magenta") == 0) return MAGENTA;
    return TARGET_COLOR;
}

int main(int argc, char *argv[]) {
    
  Color TARGET_COLOR = map_input_color(argv[1]);
  if (strcmp(argv[2], "true") == 0) ENABLE_MOTION = 1;

  Chameleon cam;
  cam.x = 0.2 * width;
  cam.y = 0.4 * height;
  cam.speed_x = 4.0;
  cam.speed_y = 4.0;
  cam.color = GREEN;
  cam.image_path = "./resources/chameleon.png";
  cam.image = LoadImage(cam.image_path);

  InitWindow(width, height, title);

  while (!WindowShouldClose()) {
      float frameTime = GetFrameTime();

    if (ENABLE_MOTION) {
      cam.x += cam.speed_x * frameTime;
      cam.y += cam.speed_y * frameTime;

      if (cam.x - IMG_WIDTH/2.0 <= 0 ) {
        cam.speed_x *= -1.0005;
        FLIP_IMAGE = 0;
      }
      
      if (cam.x + IMG_WIDTH/2.0 >= width) {
        cam.speed_x *= -1.0005;
        FLIP_IMAGE = 1;
      }

      if ((cam.y - IMG_HEIGHT/2.0) <= 0 || (cam.y + IMG_HEIGHT/2.0) >= height) {
        cam.speed_y *= -1.0005;
      }
    }

    // proportional
    proportional_r = TARGET_COLOR.r - cam.color.r;
    proportional_g = TARGET_COLOR.g - cam.color.g;
    proportional_b = TARGET_COLOR.b - cam.color.b;

    // integral
    integral_r += proportional_r * frameTime;
    integral_g += proportional_g * frameTime;
    integral_b += proportional_b * frameTime;
    
    /*
    integral_r = Clamp(MIN_PID_VALUE, MAX_PID_VALUE, integral_r);
    integral_g = Clamp(MIN_PID_VALUE, MAX_PID_VALUE, integral_g);
    integral_b = Clamp(MIN_PID_VALUE, MAX_PID_VALUE, integral_b);
    */

    // derivative
    derivative_r = (proportional_r - previous_proportional_r) / frameTime;
    derivative_g = (proportional_g - previous_proportional_g) / frameTime;
    derivative_b = (proportional_b - previous_proportional_b) / frameTime;

    // pid values
    int pid_color_r = Kp * proportional_r + Ki * integral_r + Kd * derivative_r;
    int pid_color_g = Kp * proportional_g + Ki * integral_g + Kd * derivative_b;
    int pid_color_b = Kp * proportional_b + Ki * integral_b + Kd * derivative_b;

    // update color values
    cam.color.r = Clamp(MIN_PID_VALUE, MAX_PID_VALUE, (cam.color.r + pid_color_r * frameTime));
    cam.color.g = Clamp(MIN_PID_VALUE, MAX_PID_VALUE, (cam.color.g + pid_color_g * frameTime));
    cam.color.b = Clamp(MIN_PID_VALUE, MAX_PID_VALUE, (cam.color.b + pid_color_b * frameTime));

    // save previous errors
    previous_proportional_r = proportional_r;
    previous_proportional_g = proportional_g;
    previous_proportional_b = proportional_b;

    if (PRINT_OUTPUT) printf("chameleon {%d %d %d} <==> PID {%d %d %d} \n", cam.color.r, cam.color.g, cam.color.b, TARGET_COLOR.r, TARGET_COLOR.g, TARGET_COLOR.b);

    if (abs(cam.color.r - TARGET_COLOR.r) < correlation_threshold && 
        abs(cam.color.g - TARGET_COLOR.g) < correlation_threshold  && 
        abs(cam.color.b - TARGET_COLOR.b) < correlation_threshold) 
        {
          printf("=====================================================================================\n");
          printf("...TRANSFORMATION DONE...\n");
          printf("transformed color  => %d %d %d \n", cam.color.r, cam.color.g, cam.color.g);
          printf("input target color => %d %d %d \n", TARGET_COLOR.r, TARGET_COLOR.g, TARGET_COLOR.b);
          printf("======================================================================================\n");
          break;
    }
    
    BeginDrawing();
    ClearBackground(BLACK);

    DrawText(title, 0.1 * width, 0.02 * height, 30, WHITE);
    // DrawRectangle(cam.x - IMG_WIDTH/2, cam.y - IMG_HEIGHT/2, IMG_WIDTH, IMG_HEIGHT, WHITE);
    // DrawCircle(cam.x, 0.9*cam.y, 10, GREEN);

    // chameleon and target env
    DrawCircle(0.8 * width, height / 2.0, 30.0, TARGET_COLOR);
    DrawText(target_color_text, 0.7 * width,  height / 2.0 + 40, 20, WHITE);
    render_chameleon(cam);

    EndDrawing();
  }

  UnloadTexture(cam.texture);
  CloseWindow();
  return 0;
}
