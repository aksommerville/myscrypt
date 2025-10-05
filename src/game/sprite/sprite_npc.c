/* sprite_npc.c
 * arg: u16:unused, u8:cipher, u8:stringix (strings:1)
 * Motionless sprite that talks when you bump it.
 * Optionally flop to face the hero -- we have a fixed list of tileid that do NOT do that.
 */

#include "../myscrypt.h"

struct sprite_npc {
  struct sprite hdr;
  double bump_cooldown;
  uint8_t cipher;
  uint8_t stringix;
};

#define SPRITE ((struct sprite_npc*)sprite)

static int _npc_init(struct sprite *sprite) {
  SPRITE->cipher=sprite->arg>>8;
  SPRITE->stringix=sprite->arg;
  return 0;
}

static int npc_floppable(uint8_t tileid) {
  switch (tileid) {
    // Call out those sprites which should not face the hero, eg the signpost.
    case 0x83:
    case 0x84:
      return 0;
  }
  // Everything else is oriented left, and flops to face the hero on x.
  return 1;
}

static void _npc_update(struct sprite *sprite,double elapsed,int input,int pvinput) {
  if (g.hero&&npc_floppable(sprite->tileid)) {
    if (g.hero->x<sprite->x) sprite->xform=0;
    else sprite->xform=EGG_XFORM_XREV;
  }
  if (SPRITE->bump_cooldown>0.0) SPRITE->bump_cooldown-=elapsed;
}

static void _npc_bump(struct sprite *sprite,struct sprite *bumper) {
  if (!SPRITE->stringix) return;
  if (SPRITE->bump_cooldown>0.0) return;
  SPRITE->bump_cooldown=0.5;
  modal_spawn_dialogue(SPRITE->cipher,SPRITE->stringix);
}

const struct sprite_type sprite_type_npc={
  .name="npc",
  .objlen=sizeof(struct sprite_npc),
  .init=_npc_init,
  .update=_npc_update,
  .bump=_npc_bump,
};
