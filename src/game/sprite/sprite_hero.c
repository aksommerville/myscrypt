#include "../myscrypt.h"

struct sprite_hero {
  struct sprite hdr;
  int walkdx,walkdy;
  int facedx,facedy;
  double animclock;
  int animframe;
  uint8_t tileid0;
};

#define SPRITE ((struct sprite_hero*)sprite)

/* Init.
 */
 
static int _hero_init(struct sprite *sprite) {
  SPRITE->tileid0=sprite->tileid;
  SPRITE->facedy=1;
  return 0;
}

/* Motion.
 */
 
static void hero_update_motion(struct sprite *sprite,double elapsed,int input) {
  // Move.
  const double speed=6.0; // m/s
  int walkdx=0,walkdy=0;
  switch (input&(EGG_BTN_LEFT|EGG_BTN_RIGHT)) {
    case EGG_BTN_LEFT: walkdx=-1; sprite_move(sprite,-speed*elapsed,0.0); break;
    case EGG_BTN_RIGHT: walkdx=1; sprite_move(sprite,speed*elapsed,0.0); break;
  }
  switch (input&(EGG_BTN_UP|EGG_BTN_DOWN)) {
    case EGG_BTN_UP: walkdy=-1; sprite_move(sprite,0.0,-speed*elapsed); break;
    case EGG_BTN_DOWN: walkdy=1; sprite_move(sprite,0.0,speed*elapsed); break;
  }
  // Change direction.
  if ((walkdx!=SPRITE->walkdx)||(walkdy!=SPRITE->walkdy)) {
    // If one of these directions is new, face that way.
         if (walkdx&&!SPRITE->walkdx) { SPRITE->facedx=walkdx; SPRITE->facedy=0; }
    else if (walkdy&&!SPRITE->walkdy) { SPRITE->facedx=0; SPRITE->facedy=walkdy; }
    // If only one is currently true, obey it.
    else if (walkdx&&!walkdy) { SPRITE->facedx=walkdx; SPRITE->facedy=0; }
    else if (walkdy&&!walkdx) { SPRITE->facedx=0; SPRITE->facedy=walkdy; }
    // Otherwise, don't change.
    SPRITE->walkdx=walkdx;
    SPRITE->walkdy=walkdy;
  }
  // Animate.
  if (walkdx||walkdy) {
    if ((SPRITE->animclock-=elapsed)<=0.0) {
      SPRITE->animclock+=0.200;
      if (++(SPRITE->animframe)>=4) SPRITE->animframe=0;
    }
  } else {
    SPRITE->animclock=0.0;
    SPRITE->animframe=0;
  }
}

/* Select (tileid,xform), at the end of each update.
 * The third column is both left and right; left naturally.
 */
 
static void hero_select_face(struct sprite *sprite) {
       if (SPRITE->facedx<0) { sprite->tileid=SPRITE->tileid0+0x02; sprite->xform=0; }
  else if (SPRITE->facedx>0) { sprite->tileid=SPRITE->tileid0+0x02; sprite->xform=EGG_XFORM_XREV; }
  else if (SPRITE->facedy<0) { sprite->tileid=SPRITE->tileid0+0x01; sprite->xform=0; }
  else if (SPRITE->facedy>0) { sprite->tileid=SPRITE->tileid0+0x00; sprite->xform=0; }
  switch (SPRITE->animframe) {
    case 1: sprite->tileid+=0x10; break;
    case 3: sprite->tileid+=0x20; break;
  }
}

/* Update.
 */
 
static void _hero_update(struct sprite *sprite,double elapsed,int input,int pvinput) {
  hero_update_motion(sprite,elapsed,input);
  hero_select_face(sprite);
}

/* Type definition.
 */
 
const struct sprite_type sprite_type_hero={
  .name="hero",
  .objlen=sizeof(struct sprite_hero),
  .init=_hero_init,
  .update=_hero_update,
};
