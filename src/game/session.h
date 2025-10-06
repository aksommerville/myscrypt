/* session.h
 * Manages the logical game world.
 */
 
#ifndef SESSION_H
#define SESSION_H

struct session {
  struct map *map;
  int navdx,navdy;
  double deathclock; // Assailant may set this >0 to count down and later end game with disposition DEAD.
};

int session_reset(struct session *session);

void session_navigate_soon(struct session *session,int dx,int dy);

int session_run_map_commands(struct session *session);

#endif
