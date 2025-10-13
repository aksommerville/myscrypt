#include "myscrypt.h"

/* Randomize selected bits of the config space.
 */
 
#define PICKSTR(pfx,ixlo,ixhi) { \
  int ix=ixlo+rand()%(ixhi-ixlo+1); \
  g.pfx##c=get_string(&g.pfx,1,ix); \
}
 
static void random_crypto_keys() {
  PICKSTR(vigenere_key,29,32)
  PICKSTR(playfair_key,33,36)
}

static void random_beast_names() {
  PICKSTR(vulture_name,37,42)
  PICKSTR(penguin_name,43,48)
  PICKSTR(eyeball_first_name,49,53)
  PICKSTR(eyeball_last_name,54,58)
}

#undef PICKSTR

static void random_sub_alphabet() {
  char deck[26]="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int deckc=26;
  int i=0; for (;i<26;i++) {
    int p=rand()%deckc;
    g.sub_alphabet[i]=deck[p];
    deckc--;
    memmove(deck+p,deck+p+1,deckc-p);
  }
}

static void random_maze_switches() {
  int flagidlo=NS_flag_maze_switch_1; // Switch flags must be sequential.
  int flagidhi=NS_flag_maze_switch_9;
  /* Initially, six of nine gates are closed.
   * Toggle five of them at random, possibly the same five each time.
   * That way we can't accidentally open the whole maze.
   * Also, five being odd, it's not possible to finish in the initial state either.
   */
  int i=5;
  while (i-->0) {
    int flagid=flagidlo+rand()%(flagidhi-flagidlo+1);
    flag_set(flagid,flag_get(flagid)?0:1);
  }
}

/* Schedule of gross config options.
 */
 
static void session_configure_constant(struct session *session) {
  g.vigenere_key="OCTOPUS"; g.vigenere_keyc=7;
  g.playfair_key="PINEAPPLE"; g.playfair_keyc=9;
  memcpy(g.sub_alphabet,"ABCDEFGHIJKLMNOPQRSTUVWXYZ",26);
  g.vulture_name="VINCENT"; g.vulture_namec=7;
  g.penguin_name="PENELOPE"; g.penguin_namec=8;
  g.eyeball_first_name="OSCAR"; g.eyeball_first_namec=5;
  g.eyeball_last_name="FINGER"; g.eyeball_last_namec=6;
  // Keep the maze-switch flags where they are. They're mixed, and this initial state is what I documented for cheating.
  g.save_dirty=1;
}
 
static void session_configure_variable(struct session *session) {
  random_crypto_keys();
  random_sub_alphabet();
  random_beast_names();
  random_maze_switches();
  g.save_dirty=1;
}
 
static void session_configure_stupid(struct session *session) {
  g.vigenere_keyc=0;
  g.playfair_keyc=0;
  g.sub_alphabet[0]=0;
  random_beast_names();
  // Open the maze:
  flag_set(NS_flag_maze_switch_1,1);
  flag_set(NS_flag_maze_switch_3,1);
  flag_set(NS_flag_maze_switch_4,1);
  flag_set(NS_flag_maze_switch_6,1);
  flag_set(NS_flag_maze_switch_8,1);
  flag_set(NS_flag_maze_switch_9,1);
  g.save_dirty=1;
}
 
static void session_configure_nocrypt(struct session *session) {
  g.vigenere_keyc=0;
  g.playfair_keyc=0;
  random_sub_alphabet();
  random_beast_names();
  random_maze_switches();
  g.save_dirty=1;
}
 
static void session_configure_restore(struct session *session) {
  // "Restore" just means "the currently loaded globals". The actual loading of the save file happens during init, see main.c.
  // The only state that really needs to change is the fact that Dot is alive again, and that's not recorded anywhere.
}

/* Reset.
 */
 
int session_reset(struct session *session,int start_mode) {

  egg_play_song(RID_song_in_tongues,0,1);

  // Clear things.
  while (g.spritec>0) sprite_del(g.spritev[--g.spritec]);
  if (start_mode!=SESSION_START_RESTORE) {
    int color=flag_get(NS_flag_color);
    memset(g.flagv,0,sizeof(g.flagv));
    g.flagv[0]=0x02; // NS_flag_one must always be set.
    if (color) g.flagv[NS_flag_color>>3]|=1<<(NS_flag_color&7); // Also preserve "color"; it really shouldn't be a flag
  }
  
  // Apply crypto and difficulty parameters.
  switch (start_mode) {
    case SESSION_START_CONSTANT: session_configure_constant(session); break;
    case SESSION_START_VARIABLE: session_configure_variable(session); break;
    case SESSION_START_RESTORE: session_configure_restore(session); break;
    case SESSION_START_STUPID: session_configure_stupid(session); break;
    case SESSION_START_NOCRYPT: session_configure_nocrypt(session); break;
    default: return -1;
  }
  
  // Set the "valid" flag so if Dot dies, Hello will offer a "Continue" option.
  flag_set(NS_flag_valid,1);
  
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
 
void game_over(int disposition) {
  modal_spawn_gameover(disposition);
}
