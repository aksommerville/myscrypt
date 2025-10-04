/* session.h
 * Manages the logical game world.
 */
 
#ifndef SESSION_H
#define SESSION_H

struct session {
  struct map *map;
};

int session_reset(struct session *session);

#endif
