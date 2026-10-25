#ifndef MUSIC_H
#define MUSIC_H

#include <ncurses.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wchar.h>

typedef struct {
  pid_t pid;
  int playing;
} MusicPlayer;

int load_music(char **buffer, const char *path_to_file, int max_entries);
const char *music_last_error(void);
wchar_t *convert_utf8_to_wc(const char *utf8_string);
void play_music(MusicPlayer *music, const char *audio_file_buffer,
                int selected);
void music_cleanup(MusicPlayer *music);

#endif
