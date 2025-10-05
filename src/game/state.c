#include "myscrypt.h"

/* Get flag.
 */
 
int flag_get(int flagid) {
  if (flagid<0) return 0;
  int p=flagid>>3;
  if (p>=FLAGV_SIZE) return 0;
  return (g.flagv[p]&(1<<(flagid&7)))?1:0;
}

/* Set flag.
 */
 
int flag_set(int flagid,int v) {
  if (flagid<2) return 0; // sic 2: Flags 0 and 1 are special and read-only.
  int p=flagid>>3;
  if (p>=FLAGV_SIZE) return 0;
  uint8_t mask=1<<(flagid&7);
  if (v) {
    if (g.flagv[p]&mask) return 0;
    g.flagv[p]|=mask;
    v=1;
  } else {
    if (!(g.flagv[p]&mask)) return 0;
    g.flagv[p]&=~mask;
  }
  // We don't have a publish-and-subscribe model like you'd think.
  // A sprite can subscribe to all flag changes, or nothing.
  int i=g.spritec;
  struct sprite **sp=g.spritev;
  for (;i-->0;sp++) {
    struct sprite *sprite=*sp;
    if (sprite->defunct) continue;
    if (!sprite->type->flag) continue;
    sprite->type->flag(sprite,flagid,v);
  }
  return 1;
}
