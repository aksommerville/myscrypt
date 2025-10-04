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
 * TODO Is this my problem or session's?
 */
 
static void play_nav(struct modal *modal,int dx,int dy) {
  struct map *nmap=map_by_position(g.session.map->lng+dx,g.session.map->lat+dy);
  if (!nmap) return;
  g.session.map=nmap;
  play_render_bgbits(modal,g.session.map->v);
}

/* Update.
 */
 
static void _play_update(struct modal *modal,double elapsed,int input,int pvinput) {
  /*XXX TEMP try loading neighbors on dpad
  if ((input&EGG_BTN_LEFT)&&!(pvinput&EGG_BTN_LEFT)) play_nav(modal,-1,0);
  if ((input&EGG_BTN_RIGHT)&&!(pvinput&EGG_BTN_RIGHT)) play_nav(modal,1,0);
  if ((input&EGG_BTN_UP)&&!(pvinput&EGG_BTN_UP)) play_nav(modal,0,-1);
  if ((input&EGG_BTN_DOWN)&&!(pvinput&EGG_BTN_DOWN)) play_nav(modal,0,1);
  /**/
  
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
