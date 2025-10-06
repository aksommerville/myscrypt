#include "game/myscrypt.h"

#define OUTER_MARGIN 8
#define INNER_MARGIN 8

#define TYPEWRITER_PERIOD 0.080

struct modal_dialogue {
  struct modal hdr;
  int x,y,w,h; // Box bounds. Constant size, but may be either top or bottom of the screen.
  struct egg_render_tile vtxv[MESSAGE_LIMIT];
  int vtxc; // Actual vertex count, not the size we pass to egg_render().
  int vtxp; // Counts up from 0 to (vtxc).
  double typewriter_clock;
  int show_caret;
};

#define MODAL ((struct modal_dialogue*)modal)

/* Delete.
 */
 
static void _dialogue_del(struct modal *modal) {
}

/* Init.
 */
 
static int _dialogue_init(struct modal *modal) {
  return 0;
}

/* Focus.
 */
 
static void _dialogue_focus(struct modal *modal,int focus) {
}

/* Update.
 */
 
static void _dialogue_update(struct modal *modal,double elapsed,int input,int pvinput) {

  if ((input&EGG_BTN_SOUTH)&&!(pvinput&EGG_BTN_SOUTH)) {
    if (MODAL->vtxp<MODAL->vtxc) {
      MODAL->vtxp=MODAL->vtxc;
      egg_play_sound(RID_sound_text_skip,1.0,0.0);
    } else {
      modal->defunct=1;
      egg_play_sound(RID_sound_ui_dismiss,1.0,0.0);
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
    if (MODAL->vtxp>=MODAL->vtxc) MODAL->typewriter_clock=0.500;
    MODAL->show_caret=0;
  } else {
    MODAL->typewriter_clock-=elapsed;
    if (MODAL->typewriter_clock<0.0) {
      MODAL->typewriter_clock+=0.500;
      MODAL->show_caret^=1;
    }
  }
}

/* Render.
 */
 
static void _dialogue_render(struct modal *modal) {
  graf_fill_rect(&g.graf,MODAL->x,MODAL->y,MODAL->w,MODAL->h,0x000000ff);
  graf_fill_rect(&g.graf,MODAL->x+1,MODAL->y+1,MODAL->w-2,MODAL->h-2,0xffffffff);
  graf_fill_rect(&g.graf,MODAL->x+2,MODAL->y+2,MODAL->w-4,MODAL->h-4,0x000000ff);
  graf_flush(&g.graf);
  struct egg_render_uniform un={
    .dsttexid=1,
    .srctexid=g.texid_font,
    .mode=EGG_RENDER_TILE,
    .alpha=0xff,
  };
  egg_render(&un,MODAL->vtxv,MODAL->vtxp*sizeof(struct egg_render_tile));
  if (MODAL->vtxp>=MODAL->vtxc) {
    graf_set_input(&g.graf,g.texid_tiles);
    graf_tile(&g.graf,MODAL->x+MODAL->w-16,MODAL->y+MODAL->h,0x87+MODAL->show_caret,0);
  }
}

/* Type definition.
 */
 
const struct modal_type modal_type_dialogue={
  .name="dialogue",
  .objlen=sizeof(struct modal_dialogue),
  .opaque=0,
  .interactive=1,
  .del=_dialogue_del,
  .init=_dialogue_init,
  .focus=_dialogue_focus,
  .update=_dialogue_update,
  .render=_dialogue_render,
};

/* Public ctor.
 */
 
struct modal *modal_spawn_dialogue(uint8_t cipher,uint8_t stringix) {

  const char *plaintext=0;
  int plaintextc=get_string(&plaintext,1,stringix);
  if (plaintextc<1) {
    fprintf(stderr,"String %d not found!\n",stringix);
    return 0;
  }
  
  char digestedpt[MESSAGE_LIMIT];
  int digestedptc=digest_plaintext(digestedpt,sizeof(digestedpt),plaintext,plaintextc);
  if ((digestedptc<0)||(digestedptc>sizeof(digestedpt))) return 0;
  
  char ciphertext[MESSAGE_LIMIT];
  int ciphertextc=encrypt_text(ciphertext,sizeof(ciphertext),cipher,digestedpt,digestedptc);
  if ((ciphertextc<1)||(ciphertextc>sizeof(ciphertext))) return 0;

  struct modal *modal=modal_spawn(&modal_type_dialogue);
  if (!modal) return 0;
  
  /* Top or bottom position? If there's a hero sprite, let its vertical position drive the call.
   * Can't decide? Bottom.
   */
  MODAL->x=OUTER_MARGIN;
  MODAL->w=FBW-(OUTER_MARGIN<<1);
  MODAL->h=64;
  if (g.hero&&(g.hero->y>NS_sys_maph*0.5)) {
    MODAL->y=OUTER_MARGIN;
  } else {
    MODAL->y=FBH-OUTER_MARGIN-MODAL->h;
  }
  
  MODAL->vtxc=break_lines(MODAL->vtxv,MESSAGE_LIMIT,ciphertext,ciphertextc,MODAL->x+INNER_MARGIN,MODAL->y+INNER_MARGIN,MODAL->w-(INNER_MARGIN<<1));
  if ((MODAL->vtxc<1)||(MODAL->vtxc>MESSAGE_LIMIT)) {
    modal->defunct=1;
    return 0;
  }
  
  return modal;
}
