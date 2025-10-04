#include "../myscrypt.h"

struct modal_play {
  struct modal hdr;
  int texid_bgbits;
  int texid_trans; // Previous screen, during a transition.
};

#define MODAL ((struct modal_play*)modal)

/* Delete.
 */
 
static void _play_del(struct modal *modal) {
  egg_texture_del(MODAL->texid_bgbits);
  egg_texture_del(MODAL->texid_trans);
}

/* Render bgbits.
 */
 
static void play_render_bgbits(struct modal *modal,const uint8_t *v) {
  const int halftile=NS_sys_tilesize>>1;
  // This happens during update, not render. So we must reset and flush graf on our own.
  graf_reset(&g.graf);
  graf_set_output(&g.graf,MODAL->texid_bgbits);
  graf_set_input(&g.graf,g.texid_tiles);
  int y=halftile,yi=NS_sys_maph;
  for (;yi-->0;y+=NS_sys_tilesize) {
    int x=halftile,xi=NS_sys_mapw;
    for (;xi-->0;x+=NS_sys_tilesize,v++) {
      graf_tile(&g.graf,x,y,*v,0);
    }
  }
  graf_set_output(&g.graf,1);
  graf_flush(&g.graf);
}

/* Init.
 */
 
static int _play_init(struct modal *modal) {

  if (!g.session.map) return -1;

  if ((MODAL->texid_bgbits=egg_texture_new())<1) return -1;
  if (egg_texture_load_raw(MODAL->texid_bgbits,FBW,FBH,FBW<<2,0,0)<0) return -1;
  
  if ((MODAL->texid_trans=egg_texture_new())<1) return -1;
  if (egg_texture_load_raw(MODAL->texid_trans,FBW,FBH,FBW<<2,0,0)<0) return -1;
  
  play_render_bgbits(modal,g.session.map->v);
  
  return 0;
}

/* Focus.
 */
 
static void _play_focus(struct modal *modal,int focus) {
}

/* Navigate.
 */
 
static void play_nav(struct modal *modal,int dx,int dy) {

  // Does the new map exist? Do nothing if not.
  struct map *nmap=map_by_position(g.session.map->lng+dx,g.session.map->lat+dy);
  if (!nmap) return;
  
  //TODO Prepare transition.
  
  // Delete all sprites except the first hero. (yoink her)
  struct sprite *hero=0;
  while (g.spritec>0) {
    struct sprite *sprite=g.spritev[--g.spritec];
    if (!hero&&(sprite->type==&sprite_type_hero)) hero=sprite;
    else sprite_del(sprite);
  }
  
  // Commit to the new map. Render bgbits.
  g.session.map=nmap;
  play_render_bgbits(modal,g.session.map->v);
  
  // If we have a hero, update her position and readd to the sprites list.
  if (hero) {
    hero->x-=dx*NS_sys_mapw;
    hero->y-=dy*NS_sys_maph;
    sprites_handoff(hero);
  }
  
  // Spawn other sprites, etc.
  session_run_map_commands(&g.session);
}

/* Update.
 */
 
static void _play_update(struct modal *modal,double elapsed,int input,int pvinput) {
  
  // Update sprites.
  int i=g.spritec;
  while (i-->0) {
    struct sprite *sprite=g.spritev[i];
    if (sprite->defunct) continue;
    if (sprite->type->update) sprite->type->update(sprite,elapsed,input,pvinput);
  }
  
  // Drop defunct sprites.
  for (i=g.spritec;i-->0;) {
    struct sprite *sprite=g.spritev[i];
    if (!sprite->defunct) continue;
    g.spritec--;
    memmove(g.spritev+i,g.spritev+i+1,sizeof(void*)*(g.spritec-i));
    sprite_del(sprite);
  }
  
  // If navigation was requested, do it.
  if (g.session.navdx||g.session.navdy) {
    play_nav(modal,g.session.navdx,g.session.navdy);
    g.session.navdx=g.session.navdy=0;
  }
}

/* Render.
 */
 
static void _play_render(struct modal *modal) {
  //TODO transition
  graf_set_input(&g.graf,MODAL->texid_bgbits);
  graf_decal(&g.graf,0,0,0,0,FBW,FBH);
  
  graf_set_input(&g.graf,g.texid_tiles);
  int i=0;
  struct sprite **p=g.spritev;
  for (;i<g.spritec;i++,p++) {
    struct sprite *sprite=*p;
    int dstx=(int)(sprite->x*NS_sys_tilesize);
    int dsty=(int)(sprite->y*NS_sys_tilesize);
    graf_tile(&g.graf,dstx,dsty,sprite->tileid,sprite->xform);
  }
}

/* Type definition.
 */
 
const struct modal_type modal_type_play={
  .name="play",
  .objlen=sizeof(struct modal_play),
  .opaque=1,
  .interactive=1,
  .del=_play_del,
  .init=_play_init,
  .focus=_play_focus,
  .update=_play_update,
  .render=_play_render,
};
