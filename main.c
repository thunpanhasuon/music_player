#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "music.h"

int main(void) {
  enum { MAX_SONGS = 500 };

  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);

  /* handling up to 500 songs */
  char *songs[MAX_SONGS] = {0};
  int song_count = load_music(songs, "assets", MAX_SONGS);

  if (song_count < 0) {
    box(stdscr, 0, 0);
    mvprintw(2, 2, "MUSIC ERROR");
    mvprintw(4, 2, "%s", music_last_error());
    mvprintw(6, 2, "Press any key to quit.");
    refresh();
    getch();
    endwin();
    return 1;
  }

  int selected = 0;
  MusicPlayer music_player = {.pid = -1, .playing = -1};

  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  if (max_y < 20 || max_x < 45) {
    endwin();
    fprintf(stderr, "Terminal must be at least 45 columns by 20 rows.\n");
    return 1;
  }

  WINDOW *library = newwin(10, max_x - 4, 2, 2);
  WINDOW *player = newwin(7, max_x - 4, 12, 2);

  int ch;
  int running = 1;
  while (running) {

    /* clear old content */
    werase(library);
    werase(player);

    /* drawing ui */
    box(library, 0, 0);
    box(player, 0, 0);

    erase();
    mvprintw(0, 2, " PANHA MUSIC PLAYER ");
    mvwprintw(library, 0, 2, " LIBRARY ");

    for (int i = 0; i < song_count && i < 7; i++) {
      wmove(library, 2 + i, 2);
      wprintw(library, "%c %02d. ", i == selected ? '>' : ' ', i + 1);
      wchar_t *wide_song = convert_utf8_to_wc(songs[i]);
      if (wide_song != NULL) {
        waddwstr(library, wide_song);
        free(wide_song);
      }
    }

    mvwprintw(player, 0, 2, " NOW PLAYING ");
    if (music_player.playing >= 0) {
      mvwprintw(player, 2, 2, "> %02d. ", music_player.playing + 1);
      wchar_t *wide_song = convert_utf8_to_wc(songs[music_player.playing]);
      if (wide_song != NULL) {
        waddwstr(player, wide_song);
        free(wide_song);
      }
    } else {
      mvwprintw(player, 2, 2, "  Nothing playing...");
    }

    mvprintw(max_y - 1, 2, "[Up/Down] Select  [Enter] Play  [Q] Quit");

    wnoutrefresh(stdscr);
    wnoutrefresh(library);
    wnoutrefresh(player);
    doupdate();

    /* getting input */
    ch = getch();
    if (ch == 'Q' || ch == 'q') {
      running = 0;
      break;
    }
    if (song_count > 0 && (ch == KEY_UP || ch == 'k')) {
      selected = (selected - 1 + song_count) % song_count;
    }
    if (song_count > 0 && (ch == KEY_DOWN || ch == 'j')) {
      selected = (selected + 1) % song_count;
    }
    /* record child pid id we will clean this shit leter */
    if (song_count > 0 && (ch == '\n' || ch == KEY_ENTER)) {
      char *prefix = "assets/";
      char *music = songs[selected];

      size_t audio_file_buffer_size = strlen(prefix) + strlen(music) + 1;
      char *audio_path = malloc(audio_file_buffer_size);

      if (audio_path == NULL) {
        exit(EXIT_FAILURE);
      } else {
        snprintf(audio_path, audio_file_buffer_size, "%s%s", prefix, music);

        play_music(&music_player, audio_path, selected);

        free(audio_path);
      }
    }
  }

  music_cleanup(&music_player);
  delwin(library);
  delwin(player);
  endwin();

  for (int i = 0; i < song_count; i++) {
    free(songs[i]);
  }

  return 0;
}
