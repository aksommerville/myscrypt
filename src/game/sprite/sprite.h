/* sprite.h
 */
 
#ifndef SPRITE_H
#define SPRITE_H

struct sprite;
struct sprite_type;

struct sprite {
  const struct sprite_type *type;
  int defunct;
  double x,y; // in map meters
  uint8_t tileid,xform; // sprites only render as single tiles
  int layer;
  int rid;
  const void *cmd;
  int cmdc;
  uint32_t arg;
  int solid;
};

struct sprite_type {
  const char *name;
  int objlen;
  void (*del)(struct sprite *sprite);
  int (*init)(struct sprite *sprite);
  void (*update)(struct sprite *sprite,double elapsed,int input,int pvinput);
  void (*bump)(struct sprite *sprite,struct sprite *bumper/*hero*/);
};

/* Don't use these directly.
 */
void sprite_del(struct sprite *sprite);
struct sprite *sprite_new(const struct sprite_type *type,double x,double y,uint32_t arg,int rid,const void *cmd,int cmdc);

/* Use these to create a sprite.
 * Both return WEAK.
 */
struct sprite *sprite_spawn_type(const struct sprite_type *type,double x,double y,uint32_t arg);
struct sprite *sprite_spawn_rid(int rid,double x,double y,uint32_t arg);

int sprite_move(struct sprite *sprite,double dx,double dy);

const struct sprite_type *sprite_type_by_id(int sprtype);

#define _(tag) extern const struct sprite_type sprite_type_##tag;
FOR_EACH_SPRTYPE
#undef _

#endif
