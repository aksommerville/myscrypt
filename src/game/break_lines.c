#include "myscrypt.h"

/* Break lines, main entry point.
 */
 
int break_lines(struct egg_render_tile *vtxv,int vtxa,const char *src,int srcc,int x0,int y0,int wlimit) {
  const int glyphw=8,glyphh=8;
  int xz=x0+wlimit;
  int x=x0,y=y0;
  int vtxc=0,srcp=0;
  while (srcp<srcc) {
  
    // LF: Break line explicitly.
    if (src[srcp]==0x0a) {
      srcp++;
      x=x0;
      y+=glyphh;
      continue;
    }
    
    // Any other whitespace: Advance but do not produce a glyph. OK to exceed (wlimit).
    if ((unsigned char)src[srcp]<=0x20) {
      srcp++;
      x+=glyphw;
      continue;
    }
    
    // Measure the next token. Tokens are anything but whitespace. Don't be clever about it.
    const char *token=src+srcp;
    int tokenc=1;
    while ((srcp+tokenc<srcc)&&((unsigned char)token[tokenc]>0x20)) tokenc++;
    int tokenw=glyphw*tokenc;
    
    // If it fits on this line, great, emit the vertices and consume it.
    // Also emit the token if we are at the left edge. Don't bother trying to break mid-word, we'll just ensure our text never needs to.
    if ((x+tokenw<=wlimit)||(x<=x0)) {
      srcp+=tokenc;
      for (;tokenc-->0;token++,x+=glyphw) {
        if (vtxc>=vtxa) return vtxc;
        struct egg_render_tile *vtx=vtxv+vtxc++;
        vtx->x=x;
        vtx->y=y;
        vtx->tileid=*token;
        vtx->xform=0;
      }
      continue;
    }
    
    // Skip to the next line and leave the readhead where it is.
    x=x0;
    y+=glyphh;
  }
  return vtxc;
}
