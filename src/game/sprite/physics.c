#include "../myscrypt.h"

/* Move sprite.
 */
 
int sprite_move(struct sprite *sprite,double dx,double dy) {

  if (!sprite->solid) {
    sprite->x+=dx;
    sprite->y+=dy;
    return 1;
  }
  
  /* I'm going to play fast and cheesy with the collision detection.
   * Sprites are all the same size and we'll have them move each axis independently.
   * We may assume that the first collision we encounter is the only one that matters.
   */
  
  const double fudge=0.005;
  double l=sprite->x-0.5;
  double r=sprite->x+0.5;
  double t=sprite->y-0.5;
  double b=sprite->y+0.5;
  if ((dx<0.0)||(dx>0.0)) { t+=fudge; b-=fudge; }
  else { l+=fudge; r-=fudge; }
       if (dx<0.0) { l+=dx; r=sprite->x; }
  else if (dx>0.0) { r+=dx; l=sprite->x; }
  else if (dy<0.0) { t+=dy; b=sprite->y; }
  else if (dy>0.0) { b+=dy; t=sprite->y; }
  else return 0;
  
  int cola=(int)l; if (cola<0) cola=0;
  int rowa=(int)t; if (rowa<0) rowa=0;
  int colz=(int)r; if (colz>=NS_sys_mapw) colz=NS_sys_mapw-1;
  int rowz=(int)b; if (rowz>=NS_sys_maph) rowz=NS_sys_maph-1;
  if ((cola<=colz)&&(rowa<=rowz)) {
    const uint8_t *maprow=g.session.map->v+rowa*NS_sys_mapw+cola;
    int row=rowa;
    for (;row<=rowz;row++,maprow+=NS_sys_mapw) {
      const uint8_t *mapp=maprow;
      int col=cola;
      for (;col<=colz;col++,mapp++) {
        uint8_t physics=g.physics[*mapp];
        switch (physics) {
          case NS_physics_solid: {
                   if (dx<0.0) { double n=col+1.5; if (1||n<sprite->x) sprite->x=n; }
              else if (dx>0.0) { double n=col-0.5; if (1||n>sprite->x) sprite->x=n; }
              else if (dy<0.0) { double n=row+1.5; if (1||n<sprite->y) sprite->y=n; }
              else if (dy>0.0) { double n=row-0.5; if (1||n>sprite->y) sprite->y=n; }
              return 0;
            }
        }
      }
    }
  }
  
  int i=g.spritec;
  struct sprite **p=g.spritev;
  for (;i-->0;p++) {
    struct sprite *other=*p;
    if (other==sprite) continue;
    if (other->defunct) continue;
    if (!other->solid) continue;
    if (other->x+0.5<=l) continue;
    if (other->x-0.5>=r) continue;
    if (other->y+0.5<=t) continue;
    if (other->y-0.5>=b) continue;
         if (dx<0.0) sprite->x=other->x+1.0;
    else if (dx>0.0) sprite->x=other->x-1.0;
    else if (dy<0.0) sprite->y=other->y+1.0;
    else if (dy>0.0) sprite->y=other->y-1.0;
    if ((sprite->type==&sprite_type_hero)&&other->type->bump) other->type->bump(other,sprite);
    return 0;
  }

  sprite->x+=dx;
  sprite->y+=dy;
  return 1;
}
