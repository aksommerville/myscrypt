/* shared_symbols.h
 * This file is consumed by eggdev and editor, in addition to compiling in with the game.
 */

#ifndef SHARED_SYMBOLS_H
#define SHARED_SYMBOLS_H

#define NS_sys_tilesize 16
// Define (mapw,maph) if you're using fixed-size maps.
#define NS_sys_mapw 20
#define NS_sys_maph 11

#define CMD_map_image     0x20 /* u16:imageid */
#define CMD_map_position  0x21 /* u8:horz, u8:vert */
#define CMD_map_sprite    0x61 /* u16:position, u16:spriteid, u32:arg */

#define CMD_sprite_solid 0x01
#define CMD_sprite_image 0x20 /* u16:imageid */
#define CMD_sprite_tile  0x21 /* u8:tileid, u8:xform */
#define CMD_sprite_type  0x22 /* u16:sprtype */
#define CMD_sprite_layer 0x23 /* u16:layer */

#define NS_tilesheet_physics 1
#define NS_tilesheet_family 0
#define NS_tilesheet_neighbors 0
#define NS_tilesheet_weight 0

#define NS_physics_vacant 0
#define NS_physics_solid 1

#define NS_cipher_plaintext 0
#define NS_cipher_substitution 1
#define NS_cipher_vigenere 2
#define NS_cipher_playfair 3

#define NS_sprtype_dummy 0
#define NS_sprtype_hero 1
#define NS_sprtype_npc 2
#define NS_sprtype_gate 3
#define NS_sprtype_switch 4
#define NS_sprtype_nameable 5
#define NS_sprtype_baby 6
#define NS_sprtype_firepot 7
#define NS_sprtype_fireball 8
#define FOR_EACH_SPRTYPE \
  _(dummy) \
  _(hero) \
  _(npc) \
  _(gate) \
  _(switch) \
  _(nameable) \
  _(baby) \
  _(firepot) \
  _(fireball)
  
#define NS_flag_zero 0
#define NS_flag_one 1
#define NS_flag_valid 2 /* Goes nonzero at session start. For detecting resumable sessions. */
#define NS_flag_rightswitch 3 /*available*/
#define NS_flag_maze_exit 4
#define NS_flag_name_vulture_1 5
#define NS_flag_name_vulture_2 6
#define NS_flag_vulture_gone 7
#define NS_flag_name_penguin_1 8
#define NS_flag_name_penguin_2 9
#define NS_flag_penguin_gone 10
#define NS_flag_name_eyeball_1 11
#define NS_flag_name_eyeball_2 12
#define NS_flag_eyeball_gone 13
#define NS_flag_maze_switch_1 14 /* maze_switch_* must be sequential */
#define NS_flag_maze_switch_2 15
#define NS_flag_maze_switch_3 16
#define NS_flag_maze_switch_4 17
#define NS_flag_maze_switch_5 18
#define NS_flag_maze_switch_6 19
#define NS_flag_maze_switch_7 20
#define NS_flag_maze_switch_8 21
#define NS_flag_maze_switch_9 22
#define NS_flag_switchroom1 23
#define NS_flag_switchroom2 24
#define NS_flag_switchroom3 25
#define NS_flag_switchroom4 26

#endif
