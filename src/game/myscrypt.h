#ifndef EGG_GAME_MAIN_H
#define EGG_GAME_MAIN_H

#include "egg/egg.h"
#include "opt/stdlib/egg-stdlib.h"
#include "opt/graf/graf.h"
#include "opt/res/res.h"
#include "egg_res_toc.h"
#include "shared_symbols.h"
#include "modal/modal.h"
#include "sprite/sprite.h"
#include "session.h"

/* Nonzero to print everything plaintext, and an indicator of which encryption would have been used.
 * For testing only.
 */
#define DISABLE_ENCRYPTION 0

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

// All strings for display will be shorter than this.
#define MESSAGE_LIMIT 256

#define FLAGV_SIZE 32 /* bytes. Max flag count is n*8. */

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
  const char *vigenere_key,*playfair_key;
  int vigenere_keyc,playfair_keyc;
  
  struct graf graf;
  int pvinput;
  struct session session;
  struct sprite **spritev;
  int spritec,spritea;
  struct sprite *hero; // WEAK
  uint8_t flagv[FLAGV_SIZE];
  
  struct modal *modalv[MODAL_LIMIT];
  int modalc;
  struct modal *modal_focus; // WEAK
} g;

int res_init();
int res_search(int tid,int rid);
int res_get(void *dstpp,int tid,int rid);
struct map *map_by_id(int rid);
struct map *map_by_position(int lng,int lat);
int get_string(void *dstpp,int rid,int ix);

int encrypt_text(char *dst,int dsta,int cipher,const char *src,int srcc);

/* Produce an array of tile vertices for the given text.
 * We not not produce vertices for 0x20 or below (though technically we could).
 * (x0,y0) is the center of the first tile.
 * We assume a tile size of 8x8.
 */
int break_lines(struct egg_render_tile *vtxv,int vtxa,const char *src,int srcc,int x0,int y0,int wlimit);

int flag_get(int flagid); // => 0|1
int flag_set(int flagid,int v); // => nonzero if changed

/* The disposition of gameover is either WIN or the thing that ate you.
 */
#define GAME_OVER_WIN 1
#define GAME_OVER_NAMED_BEAST 2
#define GAME_OVER_CHANGELING 3
void game_over(int disposition);

#endif
