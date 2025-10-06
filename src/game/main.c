#include "myscrypt.h"

struct g g={0};

void egg_client_quit(int status) {
}

/* Init.
 */

int egg_client_init() {

  int fbw=0,fbh=0;
  egg_texture_get_size(&fbw,&fbh,1);
  if ((fbw!=FBW)||(fbh!=FBH)) {
    fprintf(stderr,"Framebuffer size mismatch! metadata=%dx%d header=%dx%d\n",fbw,fbh,FBW,FBH);
    return -1;
  }

  if (res_init()<0) return -1;
  
  if (egg_texture_load_image(g.texid_font=egg_texture_new(),RID_image_font)<0) return -1;
  if (egg_texture_load_image(g.texid_tiles=egg_texture_new(),RID_image_tiles)<0) return -1;
  
  load_saved_game();
  
  srand_auto();

  if (!modal_spawn(&modal_type_hello)) return -1;

  return 0;
}

/* Update.
 */

void egg_client_update(double elapsed) {
  
  int input=egg_input_get_one(0);
  int pvinput=g.pvinput;
  g.pvinput=input;
  
  // Acquire the current focus modal from scratch. There must always be one.
  struct modal *nfocus=0;
  int i=g.modalc;
  while (i-->0) {
    struct modal *modal=g.modalv[i];
    if (modal->defunct) continue;
    if (modal->type->interactive) {
      nfocus=modal;
      break;
    }
  }
  if (!nfocus) {
    fprintf(stderr,"No focus modal. Terminating.\n");
    egg_terminate(1);
  }
  
  // If the new focus modal is not the one we had yesterday, notify both of them.
  if (nfocus!=g.modal_focus) {
    if (modal_is_resident(g.modal_focus)) {
      g.modal_focus->type->focus(g.modal_focus,0);
    }
    g.modal_focus=nfocus;
    g.modal_focus->type->focus(g.modal_focus,1);
  }
  
  // Update the focussed modal.
  g.modal_focus->type->update(g.modal_focus,elapsed,input,pvinput);
  
  // Reap defunct modals.
  for (i=g.modalc;i-->0;) {
    struct modal *modal=g.modalv[i];
    if (!modal->defunct) continue;
    g.modalc--;
    memmove(g.modalv+i,g.modalv+i+1,sizeof(void*)*(g.modalc-i));
    if (modal==g.modal_focus) g.modal_focus=0;
    modal_del(modal);
  }
  
  // Save if dirty.
  if (g.save_dirty) {
    g.save_dirty=0;
    save_game();
  }
}

/* Render.
 */

void egg_client_render() {
  graf_reset(&g.graf);
  
  // Find the highest opaque modal.
  int opaquep=-1,i=g.modalc;
  while (i-->0) {
    struct modal *modal=g.modalv[i];
    if (modal->type->opaque) {
      opaquep=i;
      break;
    }
  }
  
  // If nothing is opaque, black out the framebuffer.
  if (opaquep<0) {
    graf_fill_rect(&g.graf,0,0,FBW,FBH,0x000000ff);
    opaquep=0;
  }
  
  // Render every modal from (opaquep) up.
  for (;opaquep<g.modalc;opaquep++) {
    struct modal *modal=g.modalv[opaquep];
    modal->type->render(modal);
  }

  graf_flush(&g.graf);
}
