#include "music.h"

#include <dirent.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wchar.h>

static const char *last_error = NULL;

const char *music_last_error(void) {
  return last_error == NULL ? "No music error." : last_error;
}

/* Codex wrote this debug helper because I'm lazy 😂 */
wchar_t *convert_utf8_to_wc(const char *utf8_string) {
  if (utf8_string == NULL) {
    printw("Invalid UTF-8 string.\n");
    return NULL;
  }

  size_t len = mbstowcs(NULL, utf8_string, 0);
  if (len == (size_t)-1) {
    printw("UTF-8 conversion error.\n");
    return NULL;
  }

  wchar_t *wc = malloc((len + 1) * sizeof(*wc));
  if (wc == NULL) {
    printw("Unicode memory error.\n");
    return NULL;
  }

  if (mbstowcs(wc, utf8_string, len + 1) == (size_t)-1) {
    printw("UTF-8 conversion error.\n");
    free(wc);
    return NULL;
  }

  return wc;
}

int load_music(char **buffer, const char *path_to_file, int max_entries) {
  last_error = NULL;

  if (buffer == NULL || path_to_file == NULL || max_entries < 0) {
    last_error = "Invalid arguments passed to load_music().";
    return -1;
  }

  DIR *dir = opendir(path_to_file);

  if (dir == NULL) {
    last_error = "Unable to open the assets directory.";
    return -1;
  }

  struct dirent *entry;
  int num_entries = 0;

  while (num_entries < max_entries && (entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    if (entry->d_type == DT_REG) {
      /* read each entry to buffer */
      size_t len = strlen(entry->d_name);
      buffer[num_entries] = malloc(len + 1);

      if (buffer[num_entries] == NULL) {
        last_error = "Unable to allocate memory for a song name.";
        closedir(dir);

        for (int i = 0; i < num_entries; i++) {
          free(buffer[i]);
          buffer[i] = NULL;
        }

        return -1;
      }

      memcpy(buffer[num_entries], entry->d_name, len + 1);
      num_entries++;
    }
  }
  closedir(dir);
  return num_entries;
}

static void stop_music(MusicPlayer *music) {
  if (music->pid > 0) {
    kill(music->pid, SIGINT);
    waitpid(music->pid, NULL, 0);
    music->pid = -1;
  }
}

void music_cleanup(MusicPlayer *music) { stop_music(music); }

void play_music(MusicPlayer *music, const char *audio_file_buffer,
                int selected) {

  stop_music(music);

  music->pid = fork();
  if (music->pid == 0) {
    execlp("afplay", "afplay", audio_file_buffer, (char *)0);
    perror("execlp failed");
    exit(EXIT_FAILURE);
  } else if (music->pid > 0) {
    music->playing = selected;
  } else {
    mvprintw(LINES - 2, 2, "Unable to start music process");
  }
}
