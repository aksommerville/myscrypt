#include "../myscrypt.h"

/* Delete.
 */
 
void sprite_del(struct sprite *sprite) {
  if (!sprite) return;
  if (sprite->type->del) sprite->type->del(sprite);
  free(sprite);
}

/* New.
 */
 
struct sprite *sprite_new(const struct sprite_type *type,double x,double y,uint32_t arg,int rid,const void *cmd,int cmdc) {
  if (!type) return 0;
  struct sprite *sprite=calloc(1,type->objlen);
  if (!sprite) return 0;
  
  sprite->type=type;
  sprite->x=x;
  sprite->y=y;
  sprite->arg=arg;
  sprite->rid=rid;
  sprite->cmd=cmd;
  sprite->cmdc=cmdc;
  sprite->layer=100;
  
  struct cmdlist_reader reader;
  if (cmdlist_reader_init(&reader,cmd,cmdc)>=0) {
    struct cmdlist_entry cmd;
    while (cmdlist_reader_next(&cmd,&reader)>0) {
      switch (cmd.opcode) {
        case CMD_sprite_solid: sprite->solid=1; break;
        case CMD_sprite_tile: sprite->tileid=cmd.arg[0]; sprite->xform=cmd.arg[1]; break;
        case CMD_sprite_layer: sprite->layer=(cmd.arg[0]<<8)|cmd.arg[1]; break;
      }
    }
  }
  
  if (type->init) {
    if ((type->init(sprite)<0)||sprite->defunct) {
      sprite_del(sprite);
      return 0;
    }
  }
  
  return sprite;
}

/* Add sprite to global list. HANDOFF.
 */
 
static int sprite_list(struct sprite *sprite) {
  if (g.spritec>=g.spritea) {
    int na=g.spritea+32;
    if (na>INT_MAX/sizeof(void*)) return -1;
    void *nv=realloc(g.spritev,sizeof(void*)*na);
    if (!nv) return -1;
    g.spritev=nv;
    g.spritea=na;
  }
  g.spritev[g.spritec++]=sprite;
  return 0;
}

/* Spawn with type (programmatic).
 */
 
struct sprite *sprite_spawn_type(const struct sprite_type *type,double x,double y,uint32_t arg) {
  struct sprite *sprite=sprite_new(type,x,y,arg,0,0,0);
  if (!sprite) return 0;
  if (sprite_list(sprite)<0) {
    sprite_del(sprite);
    return 0;
  }
  return sprite;
}

/* Spawn with resource ID (from map).
 */
 
struct sprite *sprite_spawn_rid(int rid,double x,double y,uint32_t arg) {

  // Read resource just to get type.
  const struct sprite_type *type=0;
  const void *serial=0;
  int serialc=res_get(&serial,EGG_TID_sprite,rid);
  struct cmdlist_reader reader;
  if (sprite_reader_init(&reader,serial,serialc)<0) return 0;
  struct cmdlist_entry cmd;
  while (cmdlist_reader_next(&cmd,&reader)>0) {
    switch (cmd.opcode) {
      case CMD_sprite_type: type=sprite_type_by_id((cmd.arg[0]<<8)|cmd.arg[1]); goto _got_type_;
    }
  }
 _got_type_:;
  if (!type) return 0;
  
  // Spawn it.
  struct sprite *sprite=sprite_new(type,x,y,arg,rid,(uint8_t*)serial+4,serialc-4);
  if (!sprite) return 0;
  if (sprite_list(sprite)<0) {
    sprite_del(sprite);
    return 0;
  }
  return sprite;
}

/* Type by id.
 */
 
const struct sprite_type *sprite_type_by_id(int sprtype) {
  switch (sprtype) {
    #define _(tag) case NS_sprtype_##tag: return &sprite_type_##tag;
    FOR_EACH_SPRTYPE
    #undef _
  }
  return 0;
}
