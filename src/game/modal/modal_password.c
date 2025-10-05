/* modal_password.c
 * Enter the name of a Named Beast.
 */

#include "game/myscrypt.h"

#define ENTRY_COLC 6
#define ENTRY_ROWC 5
#define ENTRY_SPACING 10 /* Glyphs are 7x7. Three spaces between gives tasteful room for the cursor. */
#define NAME_LIMIT 16

struct modal_password {
  struct modal hdr;
  uint8_t beast_tileid;
  uint8_t name_stringix;
  uint8_t flagid0; // +(0,1,2) = (first fail, second fail, defeated)
  int selx,sely;
  struct egg_render_tile entry_vtxv[ENTRY_COLC*ENTRY_ROWC];
  double animclock;
  int animframe;
  char name[NAME_LIMIT];
  int namec;
};

#define MODAL ((struct modal_password*)modal)

/* Delete.
 */
 
static void _password_del(struct modal *modal) {
}

/* Init.
 */
 
static int _password_init(struct modal *modal) {
  return 0;
}

/* Focus.
 */
 
static void _password_focus(struct modal *modal,int focus) {
}

/* Submit name.
 */
 
static void password_submit_name(struct modal *modal) {
  
  // Trim spaces. It's easy to enough to add them by accident and the true name will never have outer space.
  const char *guess=MODAL->name;
  int guessc=MODAL->namec;
  while (guessc&&((unsigned char)guess[guessc-1]<=0x20)) guessc--;
  while (guessc&&((unsigned char)guess[0]<=0x20)) { guess++; guessc--; }
  
  // Acquire the True Name. Note that these are mixed-case in the source, intentionally, but we must force upper-case.
  const char *truename_pre=0;
  int truenamec=get_string(&truename_pre,1,MODAL->name_stringix);
  char truename[NAME_LIMIT];
  if (truenamec>sizeof(truename)) {
    fprintf(stderr,"Beast name '%.*s' is longer than the allowed %d.\n",truenamec,truename_pre,NAME_LIMIT);
    return;
  }
  int i=truenamec; while (i-->0) {
    char ch=truename_pre[i];
    if ((ch>=0x61)&&(ch<=0x7a)) truename[i]=ch-0x20;
    else truename[i]=ch;
  }
  
  // TODO Would like some livelier animation after a guess, both wrong and right.
  // If she guessed right, set the "gone" flag and dismiss the modal.
  if ((truenamec==guessc)&&!memcmp(truename,guess,guessc)) {
    egg_play_sound(RID_sound_ui_activate,1.0,0.0);
    flag_set(MODAL->flagid0+2,1);
    modal->defunct=1;
  
  // If she guessed wrong, either set the next flag or kill her.
  } else {
    egg_play_sound(RID_sound_ui_reject,1.0,0.0);
    if (!flag_get(MODAL->flagid0)) {
      flag_set(MODAL->flagid0,1);
    } else if (!flag_get(MODAL->flagid0+1)) {
      flag_set(MODAL->flagid0+1,1);
    } else {
      game_over(0);
    }
  }
}

/* Activate cursor.
 */
 
static void password_activate(struct modal *modal) {
  if ((MODAL->selx<0)||(MODAL->selx>=ENTRY_COLC)||(MODAL->sely<0)||(MODAL->sely>=ENTRY_ROWC)) return;
  int p=MODAL->sely*ENTRY_COLC+MODAL->selx;

  if (p<28) { // Letter (26 and 27 are space)
    char ch;
    if (p>=26) ch=0x20; else ch=0x41+p;
    if (MODAL->namec>=NAME_LIMIT) {
      egg_play_sound(RID_sound_ui_reject,1.0,0.0);
    } else {
      egg_play_sound(RID_sound_typewriter,1.0,0.0);
      MODAL->name[MODAL->namec++]=ch;
    }

  } else if (p==28) { // Backspace or exit.
    if (MODAL->namec) {
      egg_play_sound(RID_sound_untype,1.0,0.0);
      MODAL->namec--;
    } else {
      egg_play_sound(RID_sound_ui_dismiss,1.0,0.0);
      modal->defunct=1;
    }

  } else if (p==29) { // Enter.
    if (!MODAL->namec) {
      egg_play_sound(RID_sound_ui_reject,1.0,0.0);
    } else {
      password_submit_name(modal);
    }
  }
}

/* Cancel: Delete one character, or if empty, dismiss.
 * Exactly equivalent to choosing the "back" character.
 */
 
static void password_cancel(struct modal *modal) {
  if (MODAL->namec) {
    egg_play_sound(RID_sound_untype,1.0,0.0);
    MODAL->namec--;
  } else {
    egg_play_sound(RID_sound_ui_dismiss,1.0,0.0);
    modal->defunct=1;
  }
}

/* Move cursor.
 */
 
static void password_move(struct modal *modal,int dx,int dy) {
  egg_play_sound(RID_sound_ui_motion,1.0,0.0);
  MODAL->selx+=dx;
  if (MODAL->selx<0) MODAL->selx=ENTRY_COLC-1;
  else if (MODAL->selx>=ENTRY_COLC) MODAL->selx=0;
  MODAL->sely+=dy;
  if (MODAL->sely<0) MODAL->sely=ENTRY_ROWC-1;
  else if (MODAL->sely>=ENTRY_ROWC) MODAL->sely=0;
}

/* Update.
 */
 
static void _password_update(struct modal *modal,double elapsed,int input,int pvinput) {
  if (input!=pvinput) {
    if ((input&EGG_BTN_LEFT)&&!(pvinput&EGG_BTN_LEFT)) password_move(modal,-1,0);
    if ((input&EGG_BTN_RIGHT)&&!(pvinput&EGG_BTN_RIGHT)) password_move(modal,1,0);
    if ((input&EGG_BTN_UP)&&!(pvinput&EGG_BTN_UP)) password_move(modal,0,-1);
    if ((input&EGG_BTN_DOWN)&&!(pvinput&EGG_BTN_DOWN)) password_move(modal,0,1);
    if ((input&EGG_BTN_SOUTH)&&!(pvinput&EGG_BTN_SOUTH)) password_activate(modal);
    if ((input&EGG_BTN_WEST)&&!(pvinput&EGG_BTN_WEST)) password_cancel(modal);
  }
  if ((MODAL->animclock-=elapsed)<=0.0) {
    MODAL->animclock+=0.250;
    if (++(MODAL->animframe)>=2) MODAL->animframe=0;
  }
}

/* Render.
 */
 
static void _password_render(struct modal *modal) {
  graf_fill_rect(&g.graf,0,0,FBW,FBH,0x000000ff);
  
  // Entry glyphs. We have the vertices prepared in advance; don't use graf.
  graf_flush(&g.graf);
  struct egg_render_uniform un={
    .dsttexid=1,
    .srctexid=g.texid_font,
    .mode=EGG_RENDER_TILE,
    .alpha=0xff,
  };
  egg_render(&un,MODAL->entry_vtxv,sizeof(MODAL->entry_vtxv));
  
  // Cursor.
  graf_set_input(&g.graf,g.texid_tiles);
  if ((MODAL->selx>=0)&&(MODAL->sely>=0)&&(MODAL->selx<ENTRY_COLC)&&(MODAL->sely<ENTRY_ROWC)) {
    const struct egg_render_tile *vtx=MODAL->entry_vtxv+MODAL->sely*ENTRY_COLC+MODAL->selx;
    graf_tile(&g.graf,vtx->x,vtx->y,0x94,(MODAL->animframe&1)?EGG_XFORM_SWAP:0);
  }
  
  // Dot and the Best.
  graf_tile(&g.graf,(FBW>>1)-20,80,0x82,EGG_XFORM_XREV);
  graf_tile(&g.graf,(FBW>>1)+20,80,MODAL->beast_tileid,(MODAL->beast_tileid==0x8d)?EGG_XFORM_XREV:0);
  
  /* Word bubble background.
   * We have two caps, each 15 pixels wide, and a filler 16 pixels wide.
   * Could overlap them, but for simplicity's sake, we'll just use a multiple of 16 and have a variable amount of horizontal margin.
   */
  int bubblex=0,bubblew=0,bubbley=56; // bubbley is the center
  if (MODAL->namec) {
    graf_tile(&g.graf,(FBW>>1)-20,64,0x97,0); // Stem, always directly above Dot's head.
    int stringw=MODAL->namec*8+10; // +10 to ensure a tasteful amount of margin.
    int tilec=(stringw+NS_sys_tilesize-1)/NS_sys_tilesize;
    if (tilec<2) tilec=2; // Need at least 2 tiles, for the caps.
    bubblew=tilec*NS_sys_tilesize;
    bubblex=(FBW>>1)-20-(bubblew>>1); // Center above Dot.
    int x=bubblex+(NS_sys_tilesize>>1);
    graf_tile(&g.graf,x,bubbley,0x95,0);
    x+=NS_sys_tilesize;
    int i=tilec-2;
    for (;i-->0;x+=NS_sys_tilesize) graf_tile(&g.graf,x,bubbley,0x96,0);
    graf_tile(&g.graf,x,bubbley,0x95,EGG_XFORM_XREV);
  }
  
  /* Scoreboard at the top.
   */
  int scorey=30;
  graf_tile(&g.graf,(FBW>>1)-20,scorey,flag_get(MODAL->flagid0)?0x99:0x98,0);
  graf_tile(&g.graf,FBW>>1,scorey,flag_get(MODAL->flagid0+1)?0x99:0x98,0);
  graf_tile(&g.graf,(FBW>>1)+20,scorey,0x9a,0); // The skull is unconditional. It doesn't get checked, you just die.
  
  /* Then text over the bubble.
   * (doing things out of order to promote batching).
   */
  if (MODAL->namec) {
    graf_set_input(&g.graf,g.texid_font);
    graf_set_tint(&g.graf,0x000000ff);
    int stringw=MODAL->namec*8-1;
    int stringx=bubblex+(bubblew>>1)-(stringw>>1)+4;
    int i=0;
    for (;i<MODAL->namec;i++,stringx+=8) {
      graf_tile(&g.graf,stringx,bubbley,MODAL->name[i],0);
    }
    graf_set_tint(&g.graf,0);
  }
}

/* Type definition.
 */
 
const struct modal_type modal_type_password={
  .name="password",
  .objlen=sizeof(struct modal_password),
  .opaque=1,
  .interactive=1,
  .del=_password_del,
  .init=_password_init,
  .focus=_password_focus,
  .update=_password_update,
  .render=_password_render,
};

/* Public ctor.
 */

struct modal *modal_spawn_password(uint8_t tileid,uint8_t stringix,uint8_t flagid) {
  
  struct modal *modal=modal_spawn(&modal_type_password);
  if (!modal) return 0;
  
  MODAL->beast_tileid=tileid;
  MODAL->name_stringix=stringix;
  MODAL->flagid0=flagid;
  MODAL->selx=ENTRY_COLC>>1;
  MODAL->sely=ENTRY_ROWC>>1;
  
  // Prepare the text-entry vertices.
  int entryw=ENTRY_COLC*ENTRY_SPACING;
  int entryh=ENTRY_ROWC*ENTRY_SPACING;
  int entryx0=(FBW>>1)-(entryw>>1)+(ENTRY_SPACING>>1);
  int entryy0=FBH-20-entryh+(ENTRY_SPACING>>1);
  const char *tileidv="ABCDEFGHIJKLMNOPQRSTUVWXYZ  \x01\x02"; // Must be 30 long.
  struct egg_render_tile *vtx=MODAL->entry_vtxv;
  int yi=ENTRY_ROWC,y=entryy0;
  for (;yi-->0;y+=ENTRY_SPACING) {
    int xi=ENTRY_COLC,x=entryx0;
    for (;xi-->0;x+=ENTRY_SPACING,vtx++,tileidv++) {
      vtx->x=x;
      vtx->y=y;
      vtx->tileid=*tileidv;
      vtx->xform=0;
    }
  }
  
  return modal;
}
