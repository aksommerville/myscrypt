/* modal.h
 * Interactive layers. They aren't necessarily modal.
 */
 
#ifndef MODAL_H
#define MODAL_H

struct modal;
struct modal_type;

struct modal {
  const struct modal_type *type;
  int defunct;
};

struct modal_type {
  const char *name;
  int objlen;
  int opaque;
  int interactive;
  // All hooks are required:
  void (*del)(struct modal *modal);
  int (*init)(struct modal *modal);
  void (*focus)(struct modal *modal,int focus); // Only interactive modals will receive focus.
  void (*update)(struct modal *modal,double elapsed,int input,int pvinput);
  void (*render)(struct modal *modal);
};

/* Modals are owned by the global stack.
 * Nobody else should allocate or delete them.
 * Mark a modal defunct to have it deleted soon.
 */
void modal_del(struct modal *modal);
struct modal *modal_new(const struct modal_type *type);

/* The civilized way to instantiate a modal.
 * Returns WEAK.
 */
struct modal *modal_spawn(const struct modal_type *type);

extern const struct modal_type modal_type_play;
extern const struct modal_type modal_type_dialogue; // overlay of play
extern const struct modal_type modal_type_password; // ''
extern const struct modal_type modal_type_hello;
extern const struct modal_type modal_type_gameover;

struct modal *modal_spawn_dialogue(uint8_t cipher,uint8_t stringix);
struct modal *modal_spawn_password(uint8_t tileid,uint8_t stringix,uint8_t flagid); // tile for outerworld. two flags

#endif
