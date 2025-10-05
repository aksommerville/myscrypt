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
#define FOR_EACH_SPRTYPE \
  _(dummy) \
  _(hero) \
  _(npc) \
  _(gate) \
  _(switch)
  
#define NS_flag_zero 0
#define NS_flag_one 1
#define NS_flag_leftswitch 2
#define NS_flag_rightswitch 3

#endif
