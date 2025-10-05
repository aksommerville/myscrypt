/* sprite_switch.c
 * arg: u8:flagid, u24:reserved
 * Reacts to hero stepping on us; toggles a flag on each press.
 */
 
#include "game/myscrypt.h"

struct sprite_switch {
  struct sprite hdr;
  uint8_t flagid;
  uint8_t tileid0;
  int pressed;
};

#define SPRITE ((struct sprite_switch*)sprite)

static int _switch_init(struct sprite *sprite) {
  SPRITE->flagid=sprite->arg>>24;
  SPRITE->tileid0=sprite->tileid;
  return 0;
}

static void _switch_update(struct sprite *sprite,double elapsed,int input,int pvinput) {
  int pressed=0;
  if (g.hero) {
    double dx=g.hero->x-sprite->x;
    double dy=g.hero->y-sprite->y;
    if ((dx>-0.5)&&(dx<0.5)&&(dy>-0.5)&&(dy<0.5)) pressed=1;
  }
  if (pressed!=SPRITE->pressed) {
    if (SPRITE->pressed=pressed) {
      egg_play_sound(RID_sound_treadle,1.0,0.0);
      sprite->tileid=SPRITE->tileid0+1;
      flag_set(SPRITE->flagid,flag_get(SPRITE->flagid)^1);
    } else {
      egg_play_sound(RID_sound_treadle_release,1.0,0.0);
      sprite->tileid=SPRITE->tileid0;
    }
  }
}

const struct sprite_type sprite_type_switch={
  .name="switch",
  .objlen=sizeof(struct sprite_switch),
  .init=_switch_init,
  .update=_switch_update,
};
