#include "../myscrypt.h"

/* Delete.
 */
 
void modal_del(struct modal *modal) {
  if (!modal) return;
  modal->type->del(modal);
  free(modal);
}

/* New.
 */
 
struct modal *modal_new(const struct modal_type *type) {
  if (!type) return 0;
  struct modal *modal=calloc(1,type->objlen);
  if (!modal) return 0;
  modal->type=type;
  if (type->init(modal)<0) {
    modal_del(modal);
    return 0;
  }
  return modal;
}

/* Spawn.
 */

struct modal *modal_spawn(const struct modal_type *type) {
  if (g.modalc>=MODAL_LIMIT) return 0;
  struct modal *modal=modal_new(type);
  if (!modal) return 0;
  if (g.modalc>=MODAL_LIMIT) {
    modal_del(modal);
    return 0;
  }
  g.modalv[g.modalc++]=modal; // HANDOFF
  return modal;
}

/* Defunct all modals.
 */

void modal_defunct_all() {
  int i=g.modalc;
  while (i-->0) g.modalv[i]->defunct=1;
}

/* Test residence.
 */

int modal_is_resident(const struct modal *modal) {
  if (!modal) return 0;
  int i=g.modalc;
  while (i-->0) {
    if (g.modalv[i]==modal) return 1;
  }
  return 0;
}
