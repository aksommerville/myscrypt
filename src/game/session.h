/* session.h
 * Manages the logical game world.
 */
 
#ifndef SESSION_H
#define SESSION_H

#define SESSION_START_CONSTANT 1 /* "Jam mode". Use constant names, so we can give big hints. */
#define SESSION_START_VARIABLE 2 /* Full difficulty, and everything that can randomize does. */
#define SESSION_START_RESTORE  3 /* Load saved game. */
#define SESSION_START_STUPID   4 /* Easiest possible settings everywhere. No encryption. */
#define SESSION_START_NOCRYPT  5 /* All features except the real ciphers. So, substitution only, plus all the non-crypto things. */

struct session {
  struct map *map;
  int navdx,navdy;
  double deathclock; // Assailant may set this >0 to count down and later end game with disposition DEAD.
};

int session_reset(struct session *session,int start_mode);

void session_navigate_soon(struct session *session,int dx,int dy);

int session_run_map_commands(struct session *session);

#endif
