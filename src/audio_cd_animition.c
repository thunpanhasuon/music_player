#include "audio_cd_animition.h"

static const char *frames[] = {
    "[|]",
    "[/]",
    "[-]",
    "[\\]",
};

const char *audio_cd_animition_frame(size_t frame_number) {
  const size_t frame_count = sizeof(frames) / sizeof(frames[0]);
  return frames[frame_number % frame_count];
}
