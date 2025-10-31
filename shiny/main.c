#include "ini.h"
#include "shiny.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_IS(x) (strcmp(key, (x)) == 0)

static const char *const CONFIG_FILE_NAME = "config.ini";
extern basepkmn_t all_pkmn[];
// INI parser handler for CONFIG_FILE_NAME
static int config_handler(void *user, const char *section, const char *key,
                          const char *val) {
  config_t *conf = (config_t *)user;

  if (KEY_IS("SID_ADD")) {
    sscanf(val, "%hhd", &(conf->a_f_add));

  } else if (KEY_IS("PID_ADD")) {
    sscanf(val, "%hhd", &(conf->b_f_add));

  } else if (KEY_IS("BETWEEN_TID_SID")) {
    sscanf(val, "%u", &(conf->a));

  } else if (KEY_IS("LOWER")) {
    sscanf(val, "%u", &(conf->l));

  } else if (KEY_IS("UPPER")) {
    sscanf(val, "%u", &(conf->u));

  } else if (KEY_IS("ROWS")) {
    sscanf(val, "%hhu", &(conf->pid_r_threshold));

  } else if (KEY_IS("SHINY_ONLY")) {
    if (strcmp(val, "yes") == 0) {
      conf->search_shiny = 1;
    }

  } else if (KEY_IS("GENDER")) {
    if (strcmp(val, "male") == 0) {
      conf->search_gender = MALE;
    } else if (strcmp(val, "female") == 0) {
      conf->search_gender = FEMALE;
    } else {
      conf->search_gender = IGNORE_GENDER;
    }

  } else if (KEY_IS("MIN_IV_DEF")) {
    sscanf(val, "%hhu", &(conf->search_min_ivs.stats[2]));
    conf->search_min_ivs.stats[2] %= 32;

  } else if (KEY_IS("MIN_IV_ATK")) {
    sscanf(val, "%hhu", &(conf->search_min_ivs.stats[1]));
    conf->search_min_ivs.stats[1] %= 32;

  } else if (KEY_IS("MIN_IV_HP")) {
    sscanf(val, "%hhu", &(conf->search_min_ivs.stats[2]));
    conf->search_min_ivs.stats[2] %= 32;

  } else if (KEY_IS("MIN_IV_SDEF")) {
    sscanf(val, "%hhu", &(conf->search_min_ivs.stats[4]));
    conf->search_min_ivs.stats[4] %= 32;

  } else if (KEY_IS("MIN_IV_SATK")) {
    sscanf(val, "%hhu", &(conf->search_min_ivs.stats[3]));
    conf->search_min_ivs.stats[3] %= 32;

  } else if (KEY_IS("MIN_IV_SPE")) {
    sscanf(val, "%hhu", &(conf->search_min_ivs.stats[5]));
    conf->search_min_ivs.stats[5] %= 32;

  } else if (KEY_IS("NATURES")) {
    if (strcmp(val, "") == 0) {
      conf->search_natures = 0xFFFFFFFF;
    } else {
      char *value = (char *)malloc(strlen(val) + 1);
      memcpy(value, val, strlen(val) + 1);

      char *nature = strtok(value, " ");
      while (nature != NULL) {
        // search for nature in NATURES and set flag in conf->search_natures
        // accordingly
        uint32_t flag = 1;
        for (uint8_t i = 0; i < 25; i++) {
          if (strcmp(nature, NATURES[i]) == 0) {
            conf->search_natures |= flag;
            break;
          }
          flag = flag << 1;
        }
        nature = strtok(NULL, " ");
      }
      free(value);
    }
  } else if (KEY_IS("STARTER")) {
    for (int i = 1; i < 386; i++) {
      if (strcmp(val, all_pkmn[i - 1].name) == 0) {
        conf->starter = all_pkmn[i - 1].stats;
      }
    }
  }
  return 1;
}

int main() {
  config_t conf = {0};

  // try to parse CONFIG_FILE_NAME, abort on failure
  if (ini_parse(CONFIG_FILE_NAME, &config_handler, &conf))
    return 1;

  shiny(&conf);

  return 0;
}
