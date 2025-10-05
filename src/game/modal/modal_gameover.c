/* modal_gameover.c
 * Shown at the end of the session, both win and lose.
 * We do almost exactly the same as modal_dialogue, with some extra end-of-game bells and whistles.
 * If I had more than 2 days to make this game, would definitely want some narrative here, probly credits too.
 */
 
#include "game/myscrypt.h"

#define TYPEWRITER_PERIOD 0.080

struct modal_gameover {
  struct modal hdr;
  int disposition;
  struct egg_render_tile vtxv[MESSAGE_LIMIT];
  int vtxc,vtxp;
  double typewriter_clock;
};

#define MODAL ((struct modal_gameover*)modal)

/* Delete.
 */
 
static void _gameover_del(struct modal *modal) {
}

/* Init.
 */
 
static int _gameover_init(struct modal *modal) {
  return 0;
}

/* Focus.
 */
 
static void _gameover_focus(struct modal *modal,int focus) {
}

/* Update.
 */
 
static void _gameover_update(struct modal *modal,double elapsed,int input,int pvinput) {

  if ((input&EGG_BTN_SOUTH)&&!(pvinput&EGG_BTN_SOUTH)) {
    if (MODAL->vtxp<MODAL->vtxc) {
      MODAL->vtxp=MODAL->vtxc;
      egg_play_sound(RID_sound_text_skip,1.0,0.0);
    } else {
      modal->defunct=1;
      modal_defunct_all();
      modal_spawn(&modal_type_hello);
    }
  }
  
  if (MODAL->vtxp<MODAL->vtxc) {
    MODAL->typewriter_clock+=elapsed;
    int ding=0;
    while (MODAL->typewriter_clock>=TYPEWRITER_PERIOD) {
      MODAL->typewriter_clock-=TYPEWRITER_PERIOD;
      if (MODAL->vtxp>=MODAL->vtxc) break;
      MODAL->vtxp++;
      ding=1;
    }
    if (ding) egg_play_sound(RID_sound_typewriter,1.0,0.0);
  }
}

/* Render.
 */
 
static void _gameover_render(struct modal *modal) {
  graf_fill_rect(&g.graf,0,0,FBW,FBH,0x000000ff);
  graf_flush(&g.graf);
  struct egg_render_uniform un={
    .dsttexid=1,
    .srctexid=g.texid_font,
    .mode=EGG_RENDER_TILE,
    .alpha=0xff,
  };
  egg_render(&un,MODAL->vtxv,MODAL->vtxp*sizeof(struct egg_render_tile));
}

/* Type definition.
 */
 
const struct modal_type modal_type_gameover={
  .name="gameover",
  .objlen=sizeof(struct modal_gameover),
  .opaque=1,
  .interactive=1,
  .del=_gameover_del,
  .init=_gameover_init,
  .focus=_gameover_focus,
  .update=_gameover_update,
  .render=_gameover_render,
};

/* Public ctor.
 */
 
struct modal *modal_spawn_gameover(int disposition) {
  struct modal *modal=modal_spawn(&modal_type_gameover);
  if (!modal) return 0;
  
  MODAL->disposition=disposition;
  if (disposition==GAME_OVER_WIN) {
    egg_play_song(RID_song_frolic_in_the_dew,0,1);
  } else {
    egg_play_song(RID_song_in_tongues,0,1);//TODO need failure music too
  }
  
  /* Select a message and have it split into glyph tile vertices.
   */
  int strix;
  switch (disposition) {
    case GAME_OVER_WIN: strix=21; break;
    case GAME_OVER_NAMED_BEAST: strix=22; break;
    case GAME_OVER_CHANGELING: strix=23; break;
    case GAME_OVER_DEAD: strix=28; break;
    default: strix=24; break;
  }
  const char *src=0;
  int srcc=get_string(&src,1,strix);
  MODAL->vtxc=break_lines(MODAL->vtxv,MESSAGE_LIMIT,src,srcc,0,0,FBW-16);
  if ((MODAL->vtxc<0)||(MODAL->vtxc>MESSAGE_LIMIT)) MODAL->vtxc=0;
  
  // Determine the extents of the split message, then center it.
  int xlo=MODAL->vtxv[0].x;
  int xhi=MODAL->vtxv[0].x;
  int ylo=MODAL->vtxv[0].y;
  int yhi=MODAL->vtxv[0].y;
  struct egg_render_tile *vtx=MODAL->vtxv;
  int i=MODAL->vtxc;
  for (;i-->0;vtx++) {
    if (vtx->x<xlo) xlo=vtx->x; else if (vtx->x>xhi) xhi=vtx->x;
    if (vtx->y<ylo) ylo=vtx->y; else if (vtx->y>yhi) yhi=vtx->y;
  }
  int x0=(FBW>>1)-((xhi-xlo)>>1);
  int y0=(FBH>>1)-((yhi-ylo)>>1);
  for (vtx=MODAL->vtxv,i=MODAL->vtxc;i-->0;vtx++) {
    vtx->x+=x0;
    vtx->y+=y0;
  }
  
  return modal;
}
