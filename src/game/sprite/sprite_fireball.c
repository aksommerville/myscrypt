/* sprite_fireball.c
 * arg: u32 circleid (1..7), only used programmatically right here. Zero to target the hero.
 * Targets the hero at initialization and travels in a straight line until offscreen.
 */
 
#include "game/myscrypt.h"

#define FIREBALL_SPEED 6.0
#define SOULBALL_SPEED 8.0

struct sprite_fireball {
  struct sprite hdr;
  uint8_t tileid0;
  double animclock;
  int animframe;
  double dx,dy;
};

#define SPRITE ((struct sprite_fireball*)sprite)

static int _fireball_init(struct sprite *sprite) {
  SPRITE->tileid0=sprite->tileid;
  
  // Are we actually a soulball? Radiate outward per (arg).
  int circle=sprite->arg;
  if (circle) {
    double t=(circle*M_PI*2.0)/7.0; // one-based or zero-based, it works out the same.
    SPRITE->dx=cos(t)*SOULBALL_SPEED;
    SPRITE->dy=-sin(t)*SOULBALL_SPEED;
    return 0;
  }

  // We're a regular fireball. Target the hero.
  if (!g.hero) return -1;
  SPRITE->dx=g.hero->x-sprite->x;
  SPRITE->dy=g.hero->y-sprite->y;
  // Abort if she's too close. This dodges divide-by-zero, but also we go pretty wide, I don't think it's fair to spawn when she's right next to us.
  if ((SPRITE->dx>-1.5)&&(SPRITE->dx<1.5)&&(SPRITE->dy>-1.5)&&(SPRITE->dy<1.5)) return -1;
  double distance=sqrt(SPRITE->dx*SPRITE->dx+SPRITE->dy*SPRITE->dy);
  SPRITE->dx=(SPRITE->dx*FIREBALL_SPEED)/distance;
  SPRITE->dy=(SPRITE->dy*FIREBALL_SPEED)/distance;
  
  return 0;
}

static void fireball_kill_hero(struct sprite *sprite,struct sprite *hero) {
  g.session.deathclock=1.500;
  hero->defunct=1;
  egg_play_sound(RID_sound_die,1.0,0.0);
  
  // Fireballs are also soulballs. Spawn a bunch more of us, at the hero's heart, radiating outward.
  int i=7; // A witch's soul has seven circles; that's an immutable component of Dot Vine lore.
  while (i>0) sprite_spawn_rid(RID_sprite_fireball,hero->x,hero->y,i--);
}

static void _fireball_update(struct sprite *sprite,double elapsed,int input,int pvinput) {
  if ((SPRITE->animclock-=elapsed)<=0.0) {
    SPRITE->animclock+=0.200;
    if (++(SPRITE->animframe)>=6) SPRITE->animframe=0;
    switch (SPRITE->animframe) {
      case 0: sprite->tileid=SPRITE->tileid0; break;
      case 1: sprite->tileid=SPRITE->tileid0+1; break;
      case 2: sprite->tileid=SPRITE->tileid0+2; break;
      case 3: sprite->tileid=SPRITE->tileid0+3; break;
      case 4: sprite->tileid=SPRITE->tileid0+2; break;
      case 5: sprite->tileid=SPRITE->tileid0+1; break;
    }
  }
  sprite->x+=SPRITE->dx*elapsed;
  sprite->y+=SPRITE->dy*elapsed;
  if ((sprite->x<-1.0)||(sprite->y<-1.0)||(sprite->x>NS_sys_mapw+1.0)||(sprite->y>NS_sys_maph+1.0)) {
    sprite->defunct=1;
    return;
  }
  if (g.hero&&!g.hero->defunct&&!sprite->arg) {
    double dx=g.hero->x-sprite->x;
    double dy=g.hero->y-sprite->y;
    if ((dx>=-0.5)&&(dy>=-0.5)&&(dx<0.5)&&(dy<0.5)) {
      fireball_kill_hero(sprite,g.hero);
    }
  }
}

const struct sprite_type sprite_type_fireball={
  .name="fireball",
  .objlen=sizeof(struct sprite_fireball),
  .init=_fireball_init,
  .update=_fireball_update,
};
