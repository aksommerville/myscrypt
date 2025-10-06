#include "myscrypt.h"

static int plain_letter_by_index(int p) {
  p=p%26;
  if (p<0) p+=26;
  return 0x41+p;
}

// 0..25, or -1 if not a letter. Treats uppercase, lowercase, and substituted equally.
static int letter_index(unsigned char ch) {
  if ((ch>=0x41)&&(ch<=0x5a)) return ch-0x41;
  if ((ch>=0x61)&&(ch<=0x7a)) return ch-0x61;
  if ((ch>=0xc1)&&(ch<=0xda)) return ch-0xc1;
  if ((ch>=0xe1)&&(ch<=0xfa)) return ch-0xe1;
  return -1;
}

/* Apply the substitution cipher in place.
 * Our font tilesheet is just plaintext on the top and ciphertext on the bottom, just add 128.
 * If a shuffle alphabet is present, we apply it here.
 * No shuffle alphabet, we're noop.
 * If you want mystery glyphs without shuffling, give us the identity alphabet.
 */
 
static char apply_sub(char src) {
  if (g.sub_alphabet&&g.sub_alphabet[0]) {
    if ((src>=0x61)&&(src<=0x7a)) src-=0x20; // Force uppercase. Our fonts have identical upper and lower glyphs, so this is always safe.
    if ((src<0x41)||(src>0x5a)) return src; // Anything that isn't a letter, return it verbatim. Punctuation is identical between the plain and cipher fonts.
    src-=0x41; // 0..25
    src=g.sub_alphabet[src]; // Apply shuffle.
    return src|0x80; // Use mystery glyphs.
  } else {
    return src;
  }
}
 
static void encrypt_sub(char *v,int c) {
  for (;c-->0;v++) *v=apply_sub(*v);
}

static int subletter_by_index(int p) {
  p=p%26;
  if (p<0) p+=26;
  return apply_sub(0x41+p);
}

/* Apply vigenere cipher and also substitution.
 */
 
static int encrypt_vigenere(char *dst,int dsta,const char *src,int srcc,const char *key,int keyc) {
  if (dsta<srcc) return 0; // Length will not change. If it doesn't fit, we can abort.
  if (keyc<1) {
    if (srcc<=dsta) {
      memcpy(dst,src,srcc);
      encrypt_sub(dst,srcc);
    }
    return srcc;
  }
  int keyp=0;
  int i=srcc;
  for (;i-->0;dst++,src++) {
    int srcn=letter_index(*src);
    if (srcn<0) { // Preserve space and punctuation, to give the players a toehold.
      *dst=*src;
      continue;
    }
    int keyn=letter_index(key[keyp]);
    if (keyn<0) return 0; // Invalid key!
    if (++keyp>=keyc) keyp=0;
    *dst=subletter_by_index(srcn+keyn); // Add to encrypt.
  }
  return srcc;
}

/* Encrypt one playfair digraph.
 * (dst) is 2 bytes. (src) is 2 bytes, must be uppercase letters.
 * (table) is 25 bytes, must be 25 unique uppercase letters excluding J.
 */
 
static int playfair_find_char(int *x,int *y,char ch,const char *table) {
  for (*y=0;*y<5;(*y)++) {
    for (*x=0;*x<5;(*x)++,table++) {
      if (*table==ch) return 0;
    }
  }
  return -1;
}
 
static void playfair_encrypt_digraph(char *dst,const char *src,const char *table) {
  int ax,ay,bx,by;
  if (playfair_find_char(&ax,&ay,src[0],table)<0) { dst[0]=dst[1]='?'; return; }
  if (playfair_find_char(&bx,&by,src[1],table)<0) { dst[0]=dst[1]='?'; return; }
  
  // Same row, shift right.
  if (ay==by) {
    ax=(ax+1)%5;
    bx=(bx+1)%5;
    
  // Same column, shift down.
  } else if (ax==bx) {
    ay=(ay+1)%5;
    by=(by+1)%5;
    
  // Rectangular, swap the horizontals.
  } else {
    int tmp=ax;
    ax=bx;
    bx=tmp;
  }
  
  // Select the pre-substitution replacements.
  dst[0]=table[ay*5+ax];
  dst[1]=table[by*5+bx];
  
  // Apply substitution.
  dst[0]=apply_sub(dst[0]);
  dst[1]=apply_sub(dst[1]);
}

static int find_letter(const char *src,int srcc,char ch) {
  int srcp=0; for (;srcp<srcc;srcp++) {
    if (src[srcp]==ch) return srcp;
  }
  return -1;
}

/* Apply playfair cipher and also substitution.
 */
 
static int encrypt_playfair(char *dst,int dsta,const char *src,int srcc,const char *key,int keyc) {
  if (keyc<1) {
    if (srcc<=dsta) {
      memcpy(dst,src,srcc);
      encrypt_sub(dst,srcc);
    }
    return srcc;
  }
  
  // First, canonicalize the key by uppercasing, removing duplicates, and appending the entire alphabet (again dedupped).
  char fullkey[25];
  int fullkeyc=0,keyp=0,alphap=0;
  for (;keyp<keyc;keyp++) {
    int index=letter_index(key[keyp]);
    if (index<0) return 0; // bum key
    char canon=plain_letter_by_index(index);
    if (canon=='J') canon='I'; // J==I
    if (find_letter(fullkey,fullkeyc,canon)>=0) continue;
    fullkey[fullkeyc++]=canon;
  }
  if (fullkeyc<25) {
    for (;alphap<26;alphap++) {
      char canon=0x41+alphap;
      if (canon=='J') canon='I'; // J==I
      if (find_letter(fullkey,fullkeyc,canon)>=0) continue;
      fullkey[fullkeyc++]=canon;
      if (fullkeyc>=25) break;
    }
    if (fullkeyc<25) return 0; // i fucked up
  }
  
  // Then rearrange (fullkey) into (table), applying the spiral path.
  // I'm sure there's a graceful way to do this but I'd probably mess that up.
  char table[25];
  #define _(dstx,dsty,srcp) table[dsty*5+dstx]=fullkey[srcp];
  _(0,0, 0)
  _(1,0, 1)
  _(2,0, 2)
  _(3,0, 3)
  _(4,0, 4)
  _(4,1, 5)
  _(4,2, 6)
  _(4,3, 7)
  _(4,4, 8)
  _(3,4, 9)
  _(2,4,10)
  _(1,4,11)
  _(0,4,12)
  _(0,3,13)
  _(0,2,14)
  _(0,1,15)
  _(1,1,16)
  _(2,1,17)
  _(3,1,18)
  _(3,2,19)
  _(3,3,20)
  _(2,3,21)
  _(1,3,22)
  _(1,2,23)
  _(2,2,24)
  #undef _
  
  /* Walk (src), applying the cipher.
   * We're going to throw the player another bone here:
   * In addition to preserving space and punctuation, we also pad words, not just the entire message.
   * So there will be lots of words ending "X".
   * That's for the player's aid, but mostly for mine, since otherwise we'd have to manage the intervening plaintext characters in broken digraphs.
   */
  int dstc=0,srcp=0;
  while (srcp<srcc) {
    
    // Consume one input letter.
    // If not a letter, emit verbatim.
    char canon[2];
    int index=letter_index(src[srcp]);
    if (index<0) {
      if (dstc>=dsta) return 0;
      dst[dstc++]=src[srcp];
      srcp++;
      continue;
    }
    srcp++;
    canon[0]=plain_letter_by_index(index);
    if (canon[0]=='J') canon[0]='I';
    
    // Consume one more letter if there is one.
    // If not, keep the readhead where it is, and call the second letter X.
    canon[1]='X';
    if (srcp<srcc) {
      index=letter_index(src[srcp]);
      if (index>=0) {
        srcp++;
        canon[1]=plain_letter_by_index(index);
        if (canon[1]=='J') canon[1]='I';
        // If the second letter is the same as the first, unread it and replace with X.
        if (canon[0]==canon[1]) {
          srcp--;
          // Except if the first letter is X, make it Q instead. Try not to let this happen.
          if (canon[0]=='X') canon[1]='Q';
          else canon[1]='X';
        }
      }
    }
    
    // Processing individual digraphs is separate, see above.
    if (dstc>dsta-2) return 0;
    playfair_encrypt_digraph(dst+dstc,canon,table);
    dstc+=2;
  }
  return dstc;
}

/* Encrypt text with named cipher.
 */
 
int encrypt_text(char *dst,int dsta,int cipher,const char *src,int srcc) {
  if (!src) return 0;
  if (srcc<0) { srcc=0; while (src[srcc]) srcc++; }
  if (!srcc) return 0;
  switch (cipher) {
    case NS_cipher_plaintext: {
        if (srcc<=dsta) memcpy(dst,src,srcc);
        return srcc;
      }
    case NS_cipher_substitution: {
        if (srcc<=dsta) {
          memcpy(dst,src,srcc);
          encrypt_sub(dst,srcc);
        }
        return srcc;
      }
    case NS_cipher_vigenere: return encrypt_vigenere(dst,dsta,src,srcc,g.vigenere_key,g.vigenere_keyc);
    case NS_cipher_playfair: return encrypt_playfair(dst,dsta,src,srcc,g.playfair_key,g.playfair_keyc);
  }
  return 0;
}
