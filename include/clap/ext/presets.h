#ifndef CLAP_EXT_PRESETS_H
# define CLAP_EXT_PRESETS_H

# include "../clap.h"

# define CLAP_EXT_PRESETS "clap/presets"

struct clap_preset
{
  char     url[CLAP_URL_SIZE];     // location to the patch
  char     name[CLAP_NAME_SIZE];   // display name
  char     desc[CLAP_DESC_SIZE];   // desc and how to use it
  char     author[CLAP_NAME_SIZE];
  char     tags[CLAP_TAGS_SIZE];   // "tag1;tag2;tag3;..."
  uint8_t  score;                  // 0 = garbage, ..., 5 = favorite
};

struct clap_plugin_presets
{
  /* Returns a newly allocated preset list. The caller has to free it. */
  uint32_t (*get_presets_count)(struct clap_plugin *plugin);
  bool (*get_preset)(struct clap_plugin *plugin,
                     uint32_t            index,
                     struct clap_preset *preset);
};

#endif /* !CLAP_EXT_PRESETS_H */
