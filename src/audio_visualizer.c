#include "audio_visualizer.h"
#include "ui_colors.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int audio_visualizer_bars[AUDIO_VISUALIZER_BAR_COUNT] = {0};
static FILE* cava_fp; 

void set_pipeline(void) {
  cava_fp = popen("cava -p config/cava.conf", "r");
  if (cava_fp == NULL) {
    perror("popen failed");
    exit(EXIT_FAILURE);
  }
}

void close_pipeline(void) {
  int status = pclose(cava_fp);

  if (status == -1) {
    perror("Error closing pipe");
    exit(EXIT_FAILURE);
  } else {
    printf("Process exited with code: %d\n", status);
  }

}

int extract_process_pipe(void) {

  char cava_buffer[256]; 
  if (fgets(cava_buffer, sizeof(cava_buffer), cava_fp) == NULL) {
    return 0; 
  }
  
  size_t len = strlen(cava_buffer);
  if (len > 0 && cava_buffer[len - 1] == '\n') {
    cava_buffer[len - 1] = '\0';
  }

  int bar_count = 0; 
  char* token = strtok(cava_buffer, ";");
  while (token != NULL && bar_count < AUDIO_VISUALIZER_BAR_COUNT) {

    audio_visualizer_bars[bar_count] = atoi(token);
    bar_count++; 

    token = strtok(NULL, ";");
  }

  return 1; 
}
void draw_visualizer(WINDOW *window) {
   int height;
   int width;
   float sensitivity = 2.5;
   getmaxyx(window, height, width);

   /* Visualizer math:
      usable height = window height - top and bottom borders
      usable width  = window width - left and right borders

      total gap width = (number of bars - 1) * gap
      bar width = (usable width - total gap width) / number of bars
      total visualizer width = number of bars * bar width + total gap width

      first x = 1 + (usable width - total visualizer width) / 2
      bar x = first x + bar index * (bar width + gap)

      bar height = sensitivity * CAVA value * usable height / 100
      bar height is clamped so it cannot cross the top border
      bar y = bottom usable row - current bar level
            = window height - 2 - level */
   int usable_height = height - 2;
   int usable_width = width - 2;
   int gap = 1;

   int bar_width = (usable_width - ((AUDIO_VISUALIZER_BAR_COUNT - 1) * gap)) / AUDIO_VISUALIZER_BAR_COUNT;
   if (bar_width < 1) {
     bar_width = 1;
   }

   int total_bar = (AUDIO_VISUALIZER_BAR_COUNT * bar_width) + ((AUDIO_VISUALIZER_BAR_COUNT - 1) * gap);

   for (int i = 0; i < AUDIO_VISUALIZER_BAR_COUNT; i++) {
     int value = audio_visualizer_bars[i];

     if (value < 0) {
       value = 0;
     } else if (value > 100) {
       value = 100;
     }

     int bar_height =
         (int)(sensitivity * (float)value * (float)usable_height / 100.0f);

     if (bar_height > usable_height) {
       bar_height = usable_height;
     }

     if (value > 0 && bar_height == 0) {
       bar_height = 1;
     }

     int x = (1 + (usable_width - total_bar) / 2) +
             i * (bar_width + gap);

     for (int level = 0; level < bar_height; level++) {
       int y = height - 2 - level;
       
       wattron(window, COLOR_PAIR(VIOLET_PAIR));
       mvwhline(window, y, x, ACS_CKBOARD, bar_width);
        wattroff(window, COLOR_PAIR(VIOLET_PAIR));
     }
   }
} 

void audio_visualizer(WINDOW *window) {
  if (extract_process_pipe()) {
    draw_visualizer(window);
  }
}
