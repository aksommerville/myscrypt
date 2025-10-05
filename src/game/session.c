#include "myscrypt.h"

/* Reset.
 */
 
int session_reset(struct session *session) {

  egg_play_song(RID_song_in_tongues,0,1);
  //egg_play_song(RID_song_lets_brew_potions,0,1); // <-- hello
  //egg_play_song(RID_song_frolic_in_the_dew,0,1); // <-- gameover

  // Clear things.
  while (g.spritec>0) sprite_del(g.spritev[--g.spritec]);
  memset(g.flagv,0,sizeof(g.flagv));
  g.flagv[0]=0x02; // NS_flag_one must always be set.
  
  // Load the first map.
  if (!(session->map=map_by_id(RID_map_start))) return -1;
  if (session_run_map_commands(session)<0) return -1;
  return 0;
}

/* Map commands.
 */
 
int session_run_map_commands(struct session *session) {
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
            if ((rid==RID_sprite_hero)&&any_sprite_of_type(&sprite_type_hero)) {
              // Don't spawn the hero if we already have one; one is plenty.
            } else {
              struct sprite *sprite=sprite_spawn_rid(rid,x,y,arg);
            }
          } break;
      }
    }
  }
  return 0;
}

/* Navigate.
 */
 
void session_navigate_soon(struct session *session,int dx,int dy) {
  session->navdx=dx;
  session->navdy=dy;
}

/* End game.
 */
 
void game_over(int win) {
  fprintf(stderr,"%s:%s %s\n",__FILE__,__func__,win?"WIN":"LOSE");//TODO
  egg_terminate(0);
}
