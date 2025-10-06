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
  
  /* Encryption parameters.
   * (sub_alphabet) is always 26 bytes long, containing 'A'..'Z' once each.
   * Or null or empty to disable substitution.
   * (vigenere_key,playfair_key) disable the cipher if empty.
   * (*_name) aren't relevant to encryption but they're being configured similarly.
   */
  char sub_alphabet[26]; // One each of 'A'..'Z', or a NUL in the first position to disable.
  const char *vigenere_key,*playfair_key;
  int vigenere_keyc,playfair_keyc;
  const char *vulture_name,*penguin_name,*eyeball_first_name,*eyeball_last_name;
  int vulture_namec,penguin_namec,eyeball_first_namec,eyeball_last_namec;
  char extratext[1024]; // For storing text off the saved game, for the strings above.
  int extratextc;
  
  struct graf graf;
  int pvinput;
  struct session session;
  struct sprite **spritev;
  int spritec,spritea;
  struct sprite *hero; // WEAK
  uint8_t flagv[FLAGV_SIZE];
  int save_dirty; // Set nonzero and main will save at the next reasonable opportunity.
  
  struct modal *modalv[MODAL_LIMIT];
  int modalc;
  struct modal *modal_focus; // WEAK, just for detecting focus changes. May be dead already.
} g;

int res_init();
int res_search(int tid,int rid);
int res_get(void *dstpp,int tid,int rid);
struct map *map_by_id(int rid);
struct map *map_by_position(int lng,int lat);
int get_string(void *dstpp,int rid,int ix);

int encrypt_text(char *dst,int dsta,int cipher,const char *src,int srcc);

/* Force uppercase and insert variables ("{KEY}" becomes "VALUE").
 * Literal curlies are not legal in our text.
 */
int digest_plaintext(char *dst,int dsta,const char *src,int srcc);

/* Produce an array of tile vertices for the given text.
 * We not not produce vertices for 0x20 or below (though technically we could).
 * (x0,y0) is the center of the first tile.
 * We assume a tile size of 8x8.
 */
int break_lines(struct egg_render_tile *vtxv,int vtxa,const char *src,int srcc,int x0,int y0,int wlimit);

int flag_get(int flagid); // => 0|1
int flag_set(int flagid,int v); // => nonzero if changed
void load_saved_game();
void save_game();

/* The disposition of gameover is either WIN or the thing that ate you.
 */
#define GAME_OVER_WIN 1
#define GAME_OVER_NAMED_BEAST 2
#define GAME_OVER_CHANGELING 3
#define GAME_OVER_DEAD 4 /* unspecified assailant */
void game_over(int disposition);

/* Saved game:
 * Key "save".
 * Content is semicolon-delimited fields: FLAGS ; ALPHABET ; VIG_KEY ; PF_KEY ; VULTURE ; PENGUIN ; EYEBALL1 ; EYEBALL2
 * FLAGS is hexadecimal with trailing zeroes omitted.
 * Everything else is verbatim text.
 *
 * We will eagerly load the saved game at init, and save it at unspecified intervals during play.
 * So the Hello modal can just check NS_flag_valid to know whether a save exists, and truely saved games are equivalent to "the last session, before i died".
 * The only time Continue should *not* be an option is when the program first loads and there was no save.
 */

#endif
