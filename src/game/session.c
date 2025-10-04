#include "myscrypt.h"

/* Reset.
 */
 
int session_reset(struct session *session) {
  if (!(session->map=map_by_id(RID_map_start))) return -1;
  return 0;
}
