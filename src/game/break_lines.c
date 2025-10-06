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

/* Evaluate expression, for digest_plaintext.
 */
 
static int cpstr(char *dst,int dsta,const char *src,int srcc) {
  if (srcc<1) return 0;
  if (srcc<=dsta) memcpy(dst,src,srcc);
  return srcc;
}
 
static int digest_expression(char *dst,int dsta,const char *src,int srcc) {
  if (!src) srcc=0; else if (srcc<0) { srcc=0; while (src[srcc]) srcc++; }
  while (srcc&&((unsigned char)src[0]<=0x20)) { srcc--; src++; }
  while (srcc&&((unsigned char)src[srcc-1]<=0x20)) srcc--;
  
  if ((srcc==7)&&!memcmp(src,"VULTURE",7)) return cpstr(dst,dsta,g.vulture_name,g.vulture_namec);
  if ((srcc==7)&&!memcmp(src,"PENGUIN",7)) return cpstr(dst,dsta,g.penguin_name,g.penguin_namec);
  if ((srcc==13)&&!memcmp(src,"EYEBALL_FIRST",13)) return cpstr(dst,dsta,g.eyeball_first_name,g.eyeball_first_namec);
  if ((srcc==12)&&!memcmp(src,"EYEBALL_LAST",12)) return cpstr(dst,dsta,g.eyeball_last_name,g.eyeball_last_namec);
  if ((srcc==8)&&!memcmp(src,"VIGENERE",8)) return cpstr(dst,dsta,g.vigenere_key,g.vigenere_keyc);
  if ((srcc==8)&&!memcmp(src,"PLAYFAIR",8)) return cpstr(dst,dsta,g.playfair_key,g.playfair_keyc);
  
  return cpstr(dst,dsta,src,srcc);
}

/* Digest plaintext.
 * Nothing to do with breaking lines, but hey every function needs a home.
 */
 
int digest_plaintext(char *dst,int dsta,const char *src,int srcc) {
  if (!src) srcc=0; else if (srcc<0) { srcc=0; while (src[srcc]) srcc++; }
  int dstc=0,srcp=0;
  while (srcp<srcc) {
  
    // Curly introduces an insertion.
    if (src[srcp]=='{') {
      srcp++;
      const char *k=src+srcp;
      int kc=0;
      while ((srcp<srcc)&&(src[srcp++]!='}')) kc++;
      dstc+=digest_expression(dst+dstc,dsta-dstc,k,kc);
      
    // Lowercase letters become uppercase.
    } else if ((src[srcp]>=0x61)&&(src[srcp]<=0x7a)) {
      if (dstc<dsta) dst[dstc]=src[srcp]-0x20;
      dstc++;
      srcp++;
      
    // Everything else is verbatim.
    } else {
      if (dstc<dsta) dst[dstc]=src[srcp];
      dstc++;
      srcp++;
    }
  }
  if (dstc<dsta) dst[dstc]=0;
  return dstc;
}
