#include "myscrypt.h"

/* Get flag.
 */
 
int flag_get(int flagid) {
  if (flagid<0) return 0;
  int p=flagid>>3;
  if (p>=FLAGV_SIZE) return 0;
  return (g.flagv[p]&(1<<(flagid&7)))?1:0;
}

/* Set flag.
 */
 
int flag_set(int flagid,int v) {
  if (flagid<2) return 0; // sic 2: Flags 0 and 1 are special and read-only.
  int p=flagid>>3;
  if (p>=FLAGV_SIZE) return 0;
  uint8_t mask=1<<(flagid&7);
  if (v) {
    if (g.flagv[p]&mask) return 0;
    g.flagv[p]|=mask;
    v=1;
  } else {
    if (!(g.flagv[p]&mask)) return 0;
    g.flagv[p]&=~mask;
  }
  // We don't have a publish-and-subscribe model like you'd think.
  // A sprite can subscribe to all flag changes, or nothing.
  int i=g.spritec;
  struct sprite **sp=g.spritev;
  for (;i-->0;sp++) {
    struct sprite *sprite=*sp;
    if (sprite->defunct) continue;
    if (!sprite->type->flag) continue;
    sprite->type->flag(sprite,flagid,v);
  }
  g.save_dirty=1;
  return 1;
}

/* Load saved game.
 * This happens exactly once per execution, at init.
 * We will assume that globals are all zero initially.
 * If the saved game is valid, write it directly into the relevant globals.
 * Hello modal will notice that NS_flag_valid is set, and propose "Continue" as an option.
 * FLAGS ; ALPHABET ; VIG_KEY ; PF_KEY ; VULTURE ; PENGUIN ; EYEBALL1 ; EYEBALL2
 */
 
void load_saved_game() {
  g.extratextc=0;
  #define INVALID { \
    fprintf(stderr,"Saved game missing or invalid.\n"); \
    g.flagv[0]=0x02; \
    return; \
  }
  char src[256];
  int srcc=egg_store_get(src,sizeof(src),"save",4);
  if ((srcc<1)||(srcc>sizeof(src))) INVALID
  int srcp=0,tokenc;
  const char *token;
  #define NEXT_TOKEN { \
    while ((srcp<srcc)&&((unsigned char)src[srcp]<=0x20)) srcp++; \
    token=src+srcp; \
    tokenc=0; \
    while ((srcp<srcc)&&(src[srcp++]!=';')) tokenc++; \
    while (tokenc&&((unsigned char)token[tokenc-1]<=0x20)) tokenc--; \
  }
  
  NEXT_TOKEN // flags
  int shift=4,dstp=0;
  for (;tokenc-->0;token++) {
    if (dstp>=sizeof(g.flagv)) break;
    int digit=*token;
         if ((digit>='0')&&(digit<='9')) digit=digit-'0';
    else if ((digit>='a')&&(digit<='f')) digit=digit-'a'+10;
    else if ((digit>='A')&&(digit<='F')) digit=digit-'A'+10;
    else INVALID
    g.flagv[dstp]|=digit<<shift;
    if (shift) shift=0;
    else { shift=4; dstp++; }
  }
  
  NEXT_TOKEN // alphabet
  if (tokenc==0) {
    g.sub_alphabet[0]=0;
  } else if (tokenc==26) {
    memcpy(g.sub_alphabet,token,tokenc);
    int i=26; while (i-->0) {
      if ((g.sub_alphabet[i]<'A')||(g.sub_alphabet[i]>'Z')) {
        g.sub_alphabet[0]=0;
        INVALID
      }
    }
  } else INVALID
  
  #define STORE_STRING(name) { \
    NEXT_TOKEN \
    if (g.extratextc>sizeof(g.extratext)-tokenc) INVALID \
    memcpy(g.extratext+g.extratextc,token,tokenc); \
    g.name=g.extratext+g.extratextc; \
    g.name##c=tokenc; \
    g.extratextc+=tokenc; \
  }
  
  STORE_STRING(vigenere_key) // vigenere key
  STORE_STRING(playfair_key) // playfair key
  STORE_STRING(vulture_name) // vulture's name
  STORE_STRING(penguin_name) // penguin's name
  STORE_STRING(eyeball_first_name) // eyeball's first name
  STORE_STRING(eyeball_last_name) // eyeball's last name
  
  #undef INVALID
  #undef NEXT_TOKEN
  #undef STORE_STRING
}

/* Save game.
 */
 
void save_game() {
  char tmp[1024];
  int tmpc=0;
  
  // Start with a hexdump of flags. No need to record trailing zeroes.
  int flagc=sizeof(g.flagv);
  while (flagc&&!g.flagv[flagc-1]) flagc--;
  int i=0;
  for (;i<flagc;i++) {
    tmp[tmpc++]="0123456789abcdef"[g.flagv[i]>>4];
    tmp[tmpc++]="0123456789abcdef"[g.flagv[i]&15];
  }
  tmp[tmpc++]=';';
  
  #define APPEND(src,srcc) { \
    if ((srcc<0)||(tmpc>=sizeof(tmp)-srcc)) return; \
    memcpy(tmp+tmpc,src,srcc); \
    tmpc+=srcc; \
    tmp[tmpc++]=';'; \
  }
  
  // Next the alphabet if not empty.
  if (g.sub_alphabet[0]) {
    APPEND(g.sub_alphabet,26)
  } else {
    APPEND("",0)
  }
  
  // Then a bunch of simple strings.
  APPEND(g.vigenere_key,g.vigenere_keyc)
  APPEND(g.playfair_key,g.playfair_keyc)
  APPEND(g.vulture_name,g.vulture_namec)
  APPEND(g.penguin_name,g.penguin_namec)
  APPEND(g.eyeball_first_name,g.eyeball_first_namec)
  APPEND(g.eyeball_last_name,g.eyeball_last_namec)
  
  egg_store_set("save",4,tmp,tmpc);
}
