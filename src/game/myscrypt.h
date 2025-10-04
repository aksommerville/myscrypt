#ifndef EGG_GAME_MAIN_H
#define EGG_GAME_MAIN_H

#include "egg/egg.h"
#include "opt/stdlib/egg-stdlib.h"
#include "opt/graf/graf.h"
#include "opt/res/res.h"
#include "egg_res_toc.h"
#include "shared_symbols.h"
#include "modal/modal.h"
#include "session.h"

#define FBW 320
#define FBH 176

/* Maximum extent for map positions, must cover all resources.
 * It's fine to overshoot here.
 */
#define LAT_MIN 120
#define LAT_MAX 136
#define LNG_MIN 120
#define LNG_MAX 136
#define WORLD_MAP_SIZE ((LAT_MAX-LAT_MIN+1)*(LNG_MAX-LNG_MIN+1))

#define MODAL_LIMIT 8

struct map {
  int rid,lng,lat;
  const uint8_t *v;
  const uint8_t *cmd;
  int cmdc;
};

extern struct g {
  int texid_font;
  int texid_tiles;
  
  void *rom;
  int romc;
  struct rom_entry *resv;
  int resc,resa;
  struct map mapv[WORLD_MAP_SIZE];
  uint8_t physics[256];
  
  struct graf graf;
  int pvinput;
  struct session session;
  
  struct modal *modalv[MODAL_LIMIT];
  int modalc;
  struct modal *modal_focus; // WEAK
} g;

int res_init();
int res_search(int tid,int rid);
int res_get(void *dstpp,int tid,int rid);
struct map *map_by_id(int rid);
struct map *map_by_position(int lng,int lat);

#endif
