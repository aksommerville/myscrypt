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
  void (*flag)(struct sprite *sprite,int flagid,int v);
};

/* Don't use these directly.
 */
void sprite_del(struct sprite *sprite);
struct sprite *sprite_new(const struct sprite_type *type,double x,double y,uint32_t arg,int rid,const void *cmd,int cmdc);
void sprites_handoff(struct sprite *sprite); // Return a sprite that you stole from the global list -- modal_play does this with the hero during navigation.

/* Use these to create a sprite.
 * Both return WEAK.
 */
struct sprite *sprite_spawn_type(const struct sprite_type *type,double x,double y,uint32_t arg);
struct sprite *sprite_spawn_rid(int rid,double x,double y,uint32_t arg);

struct sprite *any_sprite_of_type(const struct sprite_type *type);

int sprite_move(struct sprite *sprite,double dx,double dy);

const struct sprite_type *sprite_type_by_id(int sprtype);

#define _(tag) extern const struct sprite_type sprite_type_##tag;
FOR_EACH_SPRTYPE
#undef _

void sprite_hero_get_face_direction(int *dx,int *dy,const struct sprite *sprite);

#endif
