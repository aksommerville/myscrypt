/* modal_hello.c
 * Keeping it simple, just a bunch of text.
 * We'll do a cheesy hollywood-cryptography kind of effect: Random letters and when they match the intended letter they stick.
 * It's a cool look.
 */
 
#include "game/myscrypt.h"

#define VTX_LIMIT 256
#define ANIMATION_TIME_LIMIT 1.500
#define OPTION_LIMIT 5
#define CURSOR_PERIOD 1.000

#define STRIX_CONTINUE 59
#define STRIX_CONSTANT 60 /* "jam mode" */
#define STRIX_VARIABLE 61
#define STRIX_STUPID   62
#define STRIX_QUIT     63

struct modal_hello {
  struct modal hdr;
  struct egg_render_tile vtxv[VTX_LIMIT];
  int vtxc;
  uint8_t tileidv[VTX_LIMIT]; // The correct tileid for each.
  double animclock;
  int all_correct;
  double stopanimclock; // Counts up from construction. If it exceeds ANIMATION_TIME_LIMIT, forcibly end the animation.
  
  struct option {
    int strix; // Identifies the behavior for us too
    int y; // Center of display
  } optionv[OPTION_LIMIT];
  int optionc;
  int optionp;
  double cursorclock;
  int cursorax,cursorzx;
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
    if ((strix<STRIX_CONTINUE)&&(ch>=0x41)&&(ch<=0x5a)) {
      vtx->tileid=0x41+rand()%26;
    } else {
      vtx->tileid=ch;
    }
    vtx->xform=0;
  }
}

// Ensure (optionp) in range, and gather (cursorax,cursorzx).
static void hello_validate_optionp(struct modal *modal) {
  if (MODAL->optionc<1) return;
  if (MODAL->optionp<0) MODAL->optionp=MODAL->optionc-1;
  else if (MODAL->optionp>=MODAL->optionc) MODAL->optionp=0;
  
  int y=MODAL->optionv[MODAL->optionp].y;
  MODAL->cursorax=MODAL->cursorzx=FBW>>1;
  const struct egg_render_tile *vtx=MODAL->vtxv;
  int i=MODAL->vtxc;
  for (;i-->0;vtx++) {
    if (vtx->y!=y) continue;
    if (vtx->x<MODAL->cursorax) MODAL->cursorax=vtx->x;
    else if (vtx->x>MODAL->cursorzx) MODAL->cursorzx=vtx->x;
  }
  MODAL->cursorax-=16;
  MODAL->cursorzx+=16;
}

static void hello_add_option(struct modal *modal,int strix,int y) {
  if (MODAL->optionc>=OPTION_LIMIT) return;
  struct option *option=MODAL->optionv+MODAL->optionc++;
  option->strix=strix;
  option->y=y;
}

static int _hello_init(struct modal *modal) {
  egg_play_song(RID_song_lets_brew_potions,0,1);
  
  hello_add_line(modal, 2, 30);
  hello_add_line(modal,25, 70);
  hello_add_line(modal,26, 78);
  hello_add_line(modal,27, 86);
  
  int y=120;
  #define OPTIONLINE(strix) { \
    hello_add_line(modal,strix,y); \
    hello_add_option(modal,strix,y); \
    y+=8; \
  }
  if (flag_get(NS_flag_valid)) { // Have a session in memory, can "Continue"
    OPTIONLINE(STRIX_CONTINUE)
  }
  OPTIONLINE(STRIX_CONSTANT)
  OPTIONLINE(STRIX_VARIABLE)
  OPTIONLINE(STRIX_STUPID)
  OPTIONLINE(STRIX_QUIT)
  #undef OPTIONLINE
  
  hello_validate_optionp(modal);
  
  return 0;
}

static void _hello_focus(struct modal *modal,int focus) {
}

static void hello_finish_animation(struct modal *modal) {
  MODAL->all_correct=1;
  const uint8_t *correct=MODAL->tileidv;
  struct egg_render_tile *vtx=MODAL->vtxv;
  int i=MODAL->vtxc;
  for (;i-->0;correct++,vtx++) {
    vtx->tileid=*correct;
  }
}

static void hello_advance_animation(struct modal *modal) {
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

static void hello_move(struct modal *modal,int d) {
  egg_play_sound(RID_sound_ui_motion,1.0,0.0);
  MODAL->optionp+=d;
  hello_validate_optionp(modal);
}

static void hello_begin_game(struct modal *modal,int start_mode) {
  modal_defunct_all();
  session_reset(&g.session,start_mode);
  modal_spawn(&modal_type_play);
}

static void hello_activate(struct modal *modal) {
  if (MODAL->all_correct) {
    switch (MODAL->optionv[MODAL->optionp].strix) {
      case STRIX_CONTINUE: hello_begin_game(modal,SESSION_START_RESTORE); break;
      case STRIX_CONSTANT: hello_begin_game(modal,SESSION_START_CONSTANT); break;
      case STRIX_VARIABLE: hello_begin_game(modal,SESSION_START_VARIABLE); break;
      case STRIX_STUPID: hello_begin_game(modal,SESSION_START_STUPID); break;
      // Not used: SESSION_START_NOCRYPT
      case STRIX_QUIT: egg_terminate(0); break;
    }
  } else {
    hello_finish_animation(modal);
  }
}

static void _hello_update(struct modal *modal,double elapsed,int input,int pvinput) {

  if (input!=pvinput) {
    if ((input&EGG_BTN_UP)&&!(pvinput&EGG_BTN_UP)) hello_move(modal,-1);
    if ((input&EGG_BTN_DOWN)&&!(pvinput&EGG_BTN_DOWN)) hello_move(modal,1);
    if ((input&EGG_BTN_SOUTH)&&!(pvinput&EGG_BTN_SOUTH)) hello_activate(modal);
  }
  
  if ((MODAL->cursorclock-=elapsed)<=0.0) {
    MODAL->cursorclock+=CURSOR_PERIOD;
  }
  MODAL->stopanimclock+=elapsed;
  if (!MODAL->all_correct&&((MODAL->animclock-=elapsed)<=0.0)) {
    MODAL->animclock+=0.030;
    if (MODAL->stopanimclock>ANIMATION_TIME_LIMIT) {
      hello_finish_animation(modal);
    } else {
      hello_advance_animation(modal);
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
  
  double displacement=MODAL->cursorclock/CURSOR_PERIOD;
  if (displacement>=0.5) displacement=1.0-displacement;
  displacement=displacement*4.0-1.0; // -1..1
  int dx=(int)(displacement*3.0);
  int y=MODAL->optionv[MODAL->optionp].y;
  struct egg_render_tile vtxv[2]={
    {MODAL->cursorax+dx,y,'>',0},
    {MODAL->cursorzx-dx,y,'<',0},
  };
  egg_render(&un,vtxv,sizeof(vtxv));
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
