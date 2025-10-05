/* sprite_nameable.c
 * arg: (u8)name(stringix), (u8)flagid(three flags, this is the lowest), (u8)position correction: 1 for half-tile NW, (u8)reserved
 * The three Nameable Beasts: Vulture, Penguin, and Eyeball.
 */
 
#include "game/myscrypt.h"

struct sprite_nameable {
  struct sprite hdr;
  uint8_t stringix;
  uint8_t flagid;
};

#define SPRITE ((struct sprite_nameable*)sprite)

static int _nameable_init(struct sprite *sprite) {
  SPRITE->stringix=sprite->arg>>24;
  SPRITE->flagid=sprite->arg>>16;
  if (flag_get(SPRITE->flagid+2)) return -1; // We're already dismissed.
  if (0)//XXX Suppress the blockage for now so i can slip past Oscar without talking to him
  switch (sprite->arg&0x0000ff00) {
    case 0x0100: sprite->x-=0.5; sprite->y-=0.5; break;
  }
  return 0;
}

static void _nameable_bump(struct sprite *sprite,struct sprite *bumper) {
  modal_spawn_password(sprite->tileid,SPRITE->stringix,SPRITE->flagid);
}

static void _nameable_flag(struct sprite *sprite,int flagid,int v) {
  if ((flagid==SPRITE->flagid+2)&&v) { // gone
    sprite->defunct=1;
  }
}

const struct sprite_type sprite_type_nameable={
  .name="nameable",
  .objlen=sizeof(struct sprite_nameable),
  .init=_nameable_init,
  .bump=_nameable_bump,
  .flag=_nameable_flag,
};
