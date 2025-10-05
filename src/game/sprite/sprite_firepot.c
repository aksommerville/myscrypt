/* sprite_firepot.c
 * arg: none
 * Sits still, animating, and occasionally lobs a fireball at the hero.
 */
 
#include "game/myscrypt.h"

struct sprite_firepot {
  struct sprite hdr;
  uint8_t tileid0;
  double animclock;
  int animframe;
  double fireclock;
};

#define SPRITE ((struct sprite_firepot*)sprite)

static void firepot_reset_fireclock(struct sprite *sprite) {
  SPRITE->fireclock=2.000+((rand()&0x7fff)*2.000)/32768.0;
}

static int _firepot_init(struct sprite *sprite) {
  SPRITE->tileid0=sprite->tileid;
  firepot_reset_fireclock(sprite);
  return 0;
}

static void firepot_fire(struct sprite *sprite) {
  if (!g.hero) return;
  struct sprite *fireball=sprite_spawn_rid(RID_sprite_fireball,sprite->x,sprite->y,0);
}

static void _firepot_update(struct sprite *sprite,double elapsed,int input,int pvinput) {
  if ((SPRITE->animclock-=elapsed)<=0.0) {
    SPRITE->animclock+=0.200;
    if (++(SPRITE->animframe)>=4) SPRITE->animframe=0;
    switch (SPRITE->animframe) {
      case 0: sprite->tileid=SPRITE->tileid0; break;
      case 1: case 3: sprite->tileid=SPRITE->tileid0+1; break;
      case 2: sprite->tileid=SPRITE->tileid0+2; break;
    }
  }
  if ((SPRITE->fireclock-=elapsed)<=0.0) {
    firepot_fire(sprite);
    firepot_reset_fireclock(sprite);
  }
}

const struct sprite_type sprite_type_firepot={
  .name="firepot",
  .objlen=sizeof(struct sprite_firepot),
  .init=_firepot_init,
  .update=_firepot_update,
};
