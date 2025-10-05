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

/* Motion. Nonzero if moved or attempted to.
 */
 
static int hero_update_motion(struct sprite *sprite,double elapsed,int input) {

  // Move.
  const double speed=6.0; // m/s
  int walkdx=0,walkdy=0,xok=1,yok=1;
  switch (input&(EGG_BTN_LEFT|EGG_BTN_RIGHT)) {
    case EGG_BTN_LEFT: walkdx=-1; xok=sprite_move(sprite,-speed*elapsed,0.0); break;
    case EGG_BTN_RIGHT: walkdx=1; xok=sprite_move(sprite,speed*elapsed,0.0); break;
  }
  switch (input&(EGG_BTN_UP|EGG_BTN_DOWN)) {
    case EGG_BTN_UP: walkdy=-1; yok=sprite_move(sprite,0.0,-speed*elapsed); break;
    case EGG_BTN_DOWN: walkdy=1; yok=sprite_move(sprite,0.0,speed*elapsed); break;
  }
  
  // If movement was rejected and was on just one axis, cheat the other axis toward a half-meter interval.
  if (walkdx&&!walkdy&&!xok) {
    double slop=sprite->y-(int)sprite->y;
    if (slop>=0.5) slop-=0.5;
    if (slop<0.200) {
      double dy=-2.0*elapsed;
      if (dy<-slop) dy=-slop;
      sprite_move(sprite,0.0,dy);
    } else if (slop>0.300) {
      double dy=2.0*elapsed;
      if (dy>0.5-slop) dy=0.5-slop;
      sprite_move(sprite,0.0,dy);
    }
  } else if (walkdy&&!walkdx&&!yok) {
    double slop=sprite->x-(int)sprite->x;
    if (slop>=0.5) slop-=0.5;
    if (slop<0.200) {
      double dx=-2.0*elapsed;
      if (dx<-slop) dx=-slop;
      sprite_move(sprite,dx,0.0);
    } else if (slop>0.300) {
      double dx=2.0*elapsed;
      if (dx>0.5-slop) dx=0.5-slop;
      sprite_move(sprite,dx,0.0);
    }
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
  return walkdx||walkdy;
}

/* Look for navigation and other motion triggers.
 */
 
static void hero_check_position(struct sprite *sprite) {
  
  /* Walk offscreen, try to navigate.
   * If we navigate to a map that doesn't exist, nothing happens and we keep going.
   * That's a problem to solve by designing maps correctly.
   */
  if (sprite->x<0.0) session_navigate_soon(&g.session,-1,0);
  else if (sprite->x>NS_sys_mapw) session_navigate_soon(&g.session,1,0);
  else if (sprite->y<0.0) session_navigate_soon(&g.session,0,-1);
  else if (sprite->y>NS_sys_maph) session_navigate_soon(&g.session,0,1);
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
  if (hero_update_motion(sprite,elapsed,input)) {
    hero_check_position(sprite);
  }
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

/* Public: Get face direction.
 */

void sprite_hero_get_face_direction(int *dx,int *dy,const struct sprite *sprite) {
  *dx=*dy=0;
  if (!sprite||(sprite->type!=&sprite_type_hero)) return;
  *dx=SPRITE->facedx;
  *dy=SPRITE->facedy;
}
