/* sprite_gate.c
 * arg: u8:flag, u8:off-state, u16:reserved
 * Solid when on, reacts to global flags.
 */
 
#include "game/myscrypt.h"

struct sprite_gate {
  struct sprite hdr;
  uint8_t flagid;
  uint8_t offstate;
  uint8_t tileid0;
};

#define SPRITE ((struct sprite_gate*)sprite)

static int _gate_init(struct sprite *sprite) {
  SPRITE->flagid=sprite->arg>>24;
  SPRITE->offstate=sprite->arg>>16;
  SPRITE->tileid0=sprite->tileid;
  if (flag_get(SPRITE->flagid)!=SPRITE->offstate) {
    sprite->tileid=SPRITE->tileid0+1;
    sprite->solid=1;
  }
  return 0;
}

static void _gate_flag(struct sprite *sprite,int flagid,int v) {
  if (flagid==SPRITE->flagid) {
    if (v!=SPRITE->offstate) {
      sprite->tileid=SPRITE->tileid0+1;
      sprite->solid=1;
    } else {
      sprite->tileid=SPRITE->tileid0;
      sprite->solid=0;
    }
  }
}

const struct sprite_type sprite_type_gate={
  .name="gate",
  .objlen=sizeof(struct sprite_gate),
  .init=_gate_init,
  .flag=_gate_flag,
};
