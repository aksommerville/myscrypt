/* modal_hello.c
 * Keeping it simple, just a bunch of text.
 * We'll do a cheesy hollywood-cryptography kind of effect: Random letters and when they match the intended letter they stick.
 * It's a cool look.
 */
 
#include "game/myscrypt.h"

#define VTX_LIMIT 256

struct modal_hello {
  struct modal hdr;
  struct egg_render_tile vtxv[VTX_LIMIT];
  int vtxc;
  uint8_t tileidv[VTX_LIMIT]; // The correct tileid for each.
  double animclock;
  int all_correct;
};

#define MODAL ((struct modal_hello*)modal)

static void _hello_del(struct modal *modal) {
}

static void hello_add_line(struct modal *modal,int strix,int y) {
  if (MODAL->vtxc>=VTX_LIMIT) return;
  const char *src=0;
  int srcc=get_string(&src,1,strix);
  if (srcc<1) return;
  int w=srcc*8;
  int x=(FBW>>1)-(w>>1)+4;
  for (;srcc-->0;src++,x+=8) {
    if ((unsigned char)(*src)<=0x20) continue;
    if (MODAL->vtxc>=VTX_LIMIT) return;
    char ch=*src;
    if ((ch>=0x61)&&(ch<=0x7a)) ch-=0x20;
    MODAL->tileidv[MODAL->vtxc]=ch;
    struct egg_render_tile *vtx=MODAL->vtxv+MODAL->vtxc++;
    vtx->x=x;
    vtx->y=y;
    if ((ch>=0x41)&&(ch<=0x5a)) {
      vtx->tileid=0x41+rand()%26;
    } else {
      vtx->tileid=ch;
    }
    vtx->xform=0;
  }
}

static int _hello_init(struct modal *modal) {
  egg_play_song(RID_song_lets_brew_potions,0,1);
  
  hello_add_line(modal, 2, 50);
  hello_add_line(modal,25,120);
  hello_add_line(modal,26,128);
  hello_add_line(modal,27,136);
  
  return 0;
}

static void _hello_focus(struct modal *modal,int focus) {
}

static void _hello_update(struct modal *modal,double elapsed,int input,int pvinput) {
  if ((input&EGG_BTN_SOUTH)&&!(pvinput&EGG_BTN_SOUTH)) {
    modal_defunct_all();
    session_reset(&g.session);
    modal_spawn(&modal_type_play);
  }
  if (!MODAL->all_correct&&((MODAL->animclock-=elapsed)<=0.0)) {
    MODAL->animclock+=0.030;
    MODAL->all_correct=1;
    const uint8_t *correct=MODAL->tileidv;
    struct egg_render_tile *vtx=MODAL->vtxv;
    int i=MODAL->vtxc;
    for (;i-->0;correct++,vtx++) {
      if (vtx->tileid==*correct) continue;
      vtx->tileid=0x41+rand()%26;
      MODAL->all_correct=0;
    }
  }
}

static void _hello_render(struct modal *modal) {
  graf_fill_rect(&g.graf,0,0,FBW,FBH,0x000000ff);
  graf_flush(&g.graf);
  struct egg_render_uniform un={
    .dsttexid=1,
    .srctexid=g.texid_font,
    .mode=EGG_RENDER_TILE,
    .alpha=0xff,
  };
  egg_render(&un,MODAL->vtxv,sizeof(struct egg_render_tile)*MODAL->vtxc);
}

const struct modal_type modal_type_hello={
  .name="hello",
  .objlen=sizeof(struct modal_hello),
  .opaque=1,
  .interactive=1,
  .del=_hello_del,
  .init=_hello_init,
  .focus=_hello_focus,
  .update=_hello_update,
  .render=_hello_render,
};
