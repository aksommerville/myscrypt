#include "myscrypt.h"

/* Reset.
 */
 
int session_reset(struct session *session) {
  if (!(session->map=map_by_id(RID_map_start))) return -1;
  
  struct cmdlist_reader reader;
  if (cmdlist_reader_init(&reader,session->map->cmd,session->map->cmdc)>=0) {
    struct cmdlist_entry cmd;
    while (cmdlist_reader_next(&cmd,&reader)>0) {
      switch (cmd.opcode) {
        case CMD_map_sprite: {
            double x=cmd.arg[0]+0.5;
            double y=cmd.arg[1]+0.5;
            int rid=(cmd.arg[2]<<8)|cmd.arg[3];
            uint32_t arg=(cmd.arg[4]<<24)|(cmd.arg[5]<<16)|(cmd.arg[6]<<8)|cmd.arg[7];
            struct sprite *sprite=sprite_spawn_rid(rid,x,y,arg);
          } break;
      }
    }
  }
  
  return 0;
}
