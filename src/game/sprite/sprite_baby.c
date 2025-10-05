/* sprite_baby.c
 * arg: (u32)reserved
 * Walks around randomly.
 * One is selected randomly at startup, and appointed the True Human Child.
 * All others are Deadly Changelings.
 */
 
#include "game/myscrypt.h"

#define ROLE_UNSET 0
#define ROLE_HUMAN 1
#define ROLE_DEMON 2

#define WALK_SPEED 2.0
#define WALK_TIME_MIN 0.500
#define WALK_TIME_MAX 2.000
#define HOLD_TIME 0.500

struct sprite_baby {
  struct sprite hdr;
  uint8_t tileid0;
  int role;
  double animclock;
  int animframe;
  int dx,dy;
  double walkclock;
  double holdclock;
};

#define SPRITE ((struct sprite_baby*)sprite)

static int _baby_init(struct sprite *sprite) {
  SPRITE->tileid0=sprite->tileid;
  return 0;
}

static void baby_select_roles() {

  // Find all the babies.
  #define BABY_LIMIT 16
  struct sprite *babyv[BABY_LIMIT];
  int babyc=0;
  struct sprite **p=g.spritev;
  int i=g.spritec;
  for (;i-->0;p++) {
    struct sprite *sprite=*p;
    if (sprite->defunct) continue;
    if (sprite->type!=&sprite_type_baby) continue;
    babyv[babyc++]=sprite;
    if (babyc>=BABY_LIMIT) break;
  }
  #undef BABY_LIMIT
  if (babyc<1) return; // huh? how did we get called?
  
  // First, set them all to DEMON.
  for (i=babyc,p=babyv;i-->0;p++) {
    struct sprite *sprite=*p;
    SPRITE->role=ROLE_DEMON;
  }
  
  // Then pick one at random for the honoring of being HUMAN.
  int humanp=rand()%babyc;
  struct sprite *sprite=babyv[humanp];
  SPRITE->role=ROLE_HUMAN;
}

static void _baby_update(struct sprite *sprite,double elapsed,int input,int pvinput) {

  /* First time a baby updates, we'll select roles for everybody.
   * We don't do this during init because we need the whole set of babies.
   */
  if (SPRITE->role==ROLE_UNSET) baby_select_roles();
  
  // Human child interrupts activity to walk away from the hero.
  if (g.hero&&(SPRITE->role==ROLE_HUMAN)) {
    int hdx,hdy,peekaboo=0;
    sprite_hero_get_face_direction(&hdx,&hdy,g.hero);
    double dx=g.hero->x-sprite->x,dy=g.hero->y-sprite->y;
    const double barrel_radius=0.500;
    if (hdx) {
      if ((dy>=-barrel_radius)&&(dy<barrel_radius)) {
        if ((hdx<0)&&(dx>0.0)) peekaboo=1;
        else if ((hdx>0)&&(dx<0.0)) peekaboo=1;
      }
    } else if (hdy) {
      if ((dx>=-barrel_radius)&&(dx<barrel_radius)) {
        if ((hdy<0)&&(dy>0.0)) peekaboo=1;
        else if ((hdy>0)&&(dy<0.0)) peekaboo=1;
      }
    }
    if (peekaboo) {
      SPRITE->dx=hdx;
      SPRITE->dy=hdy;
      SPRITE->walkclock=0.500;
    }
  }
  
  // If we have a hold, wait it out.
  if (SPRITE->holdclock>0.0) {
    SPRITE->holdclock-=elapsed;
    SPRITE->animclock=0.0;
    SPRITE->animframe=0;
    
  // Not holding but walking, do that. It's entirely based on a random clock, if we hit a wall just keep going.
  } else if (SPRITE->walkclock>0.0) {
    SPRITE->walkclock-=elapsed;
    sprite_move(sprite,SPRITE->dx*WALK_SPEED*elapsed,SPRITE->dy*WALK_SPEED*elapsed);
    
  // Pick a random direction and walk time, face that way, and set a brief hold.
  } else {
    switch (rand()&3) {
      case 0: SPRITE->dx=-1; SPRITE->dy=0; break;
      case 1: SPRITE->dx=1; SPRITE->dy=0; break;
      case 2: SPRITE->dx=0; SPRITE->dy=-1; break;
      case 3: SPRITE->dx=0; SPRITE->dy=1; break;
    }
    SPRITE->walkclock=WALK_TIME_MIN+((rand()&0x7fff)*(WALK_TIME_MAX-WALK_TIME_MIN))/32768.0;
    SPRITE->holdclock=HOLD_TIME;
  }
  
  // Animate if walking.
  if ((SPRITE->walkclock>0.0)&&(SPRITE->holdclock<=0.0)) {
    if ((SPRITE->animclock-=elapsed)<=0.0) {
      SPRITE->animclock+=0.200;
      if (++(SPRITE->animframe)>=4) SPRITE->animframe=0;
    }
    switch (SPRITE->animframe) {
      case 0: case 2: sprite->tileid=SPRITE->tileid0; break;
      case 1: sprite->tileid=SPRITE->tileid0+0x10; break;
      case 3: sprite->tileid=SPRITE->tileid0+0x20; break;
    }
  } else {
    sprite->tileid=SPRITE->tileid0;
  }
  if (SPRITE->dx<0) { sprite->tileid+=2; sprite->xform=0; }
  else if (SPRITE->dx>0) { sprite->tileid+=2; sprite->xform=EGG_XFORM_XREV; }
  else if (SPRITE->dy<0) { sprite->tileid+=1; sprite->xform=0; }
  else if (SPRITE->dy>0) { sprite->xform=0; }
}

static void _baby_bump(struct sprite *sprite,struct sprite *bumper) {
  switch (SPRITE->role) {
    case ROLE_HUMAN: game_over(1); break;
    case ROLE_DEMON: game_over(0); break;
  }
}

const struct sprite_type sprite_type_baby={
  .name="baby",
  .objlen=sizeof(struct sprite_baby),
  .init=_baby_init,
  .update=_baby_update,
  .bump=_baby_bump,
};
