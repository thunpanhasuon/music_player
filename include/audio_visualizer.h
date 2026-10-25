#ifndef AUDIO_VISUALIZER_H
#define AUDIO_VISUALIZER_H

#include <ncurses.h>

#define AUDIO_VISUALIZER_BAR_COUNT 32

extern int audio_visualizer_bars[AUDIO_VISUALIZER_BAR_COUNT];

void set_pipeline(void);
void close_pipeline(void);
int extract_process_pipe(void);
void draw_visualizer(WINDOW *window);
void audio_visualizer(WINDOW *window);

#endif
