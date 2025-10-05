# Mysteries of the Crypt

SPOILER WARNING: If you're going to play the game, [do that first](https://aksommerville.itch.io/mysteries-of-the-crypt) and **do not read the rest of this file!**
The fun of the game is in figuring out how to play it.
Reading my notes or source will give it all away.

Requires [Egg v2](https://github.com/aksommerville/egg2) to build.

For [Inferno Jam 2025](https://itch.io/jam/inferno-jam-2025), theme "One Color".
(as in, that's a literal requirement, not a theme to be interpretted).

- They allow shades of the color but let's be hard-core and go strict monochrome.
- Zelda-style adventure starring Dot Vine.
- Dialogue is encrypted with old-fashioned ciphers that the player must decrypt.
- - A trivial substitution cipher, and on top of that, say Vigenere and Playfair? The way Buried did it.
- - Provide lots of opportunities to crack and validate the sub. Don't force them to do frequency analysis.
- - eg hello modal, print encrypted. The actual content will be obvious ("Mysteries of the Crypt" and "Play").
- Use tilesheet text, not a font, so all glyph slots are interchangeable.
- - Start with a trivial substitution cipher. Our font tilesheet will be plaintext in 0..7 and ciphertext in 8..15.
- Include a maze that should pretty much require paper and pencil.
- Also some combat and hazards, so players who balk at the cryptopuzzle can at least have some fun.

Aim to complete by Monday evening, 6 October.

## TODO

- [x] Draw font and sub cipher.
- [x] Basic skeleton.
- - [x] Modal stack.
- - [x] Map loader. Single screen maps @ 20,11.
- - [x] Sprites.
- [x] Graphics.
- [x] Dialogue.
- [x] Global store.
- [ ] Hazards.
- [x] Password entry.
- [x] Bump: Actuate only if the off-axis is so close to alignment, say +-0.5? I keep triggering dialogue with my toe.
- [ ] Transitions.
- [x] Maps.
- [x] Music.
- [x] Sound effects.
- [x] Hello modal.
- [x] Gameover modal.
- [x] Password modal: Don't allow to submit the same name twice in a row. Also, strip all spaces when canonicalizing ("OSCARFINGER" should be fine)
- [ ] Itch page.
- [ ] Can we prevent hero from staying in a stepping frame while dialogue runs?
- [x] Redraw the font simpler. Have pity on the poor player, they are going to write it all by hand.
- [ ] Remember to turn off DISABLE_ENCRYPTION and the cheat room before releasing!

## Notes

Ensure that we implement Vigenere and Playfair exactly as Wikipedia describes them, so users have a chance.

https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher
 - Key index is zero-based.
 - Add to encrypt, subtract to decrypt.
 - Offer this hint, that Wikipedia page is the first Google hit: "le chiffrage indechiffrable"
 
https://en.wikipedia.org/wiki/Playfair_cipher
 - Replace J with I -- need to hint this.
 - Spiral from the upper left -- need to hint this.
 - Use X for parity and pair-breaking -- should hint this.
 - This Wikipedia page is only the third Google hit for "playfair". Maybe something more precise?
 
## Hints

Transcribe these for the Itch page, and hide under disclosure elements with lots of "hey you're ruining your own fun!".

- All text in the first two rooms (where you start, and the screen just north of it) can be deciphered with nothing else.
- If you're struggling with that: Look at the apostrophes, look for one and two letter words. It's very solvable.
- Include the sub-cipher cheat sheet.
- To complete the maze, you'll need to throw the appropriate switches on the north row.
- Touch each of these switches once: East room: middle, right. Middle room: Left, right. West room: Left, right.
- Once the switches are opened, follow the outside edge of the maze to reach the goal.
- There's a skeleton you ought to talk to in the maze; take the northern fork at the southeast corner.
- The three guard monsters will vanish if you say their name.
- The vulture's name can be found in the maze (west of start).
- It's "VINCENT".
- The penguin's name can be found east of start.
- It's "PENELOPE".
- The eyeball's name is in two pieces, guarded by vulture and penguin.
- It's "OSCAR FINGER".
- In the vulture's room, use the substitution cipher as before, and then a Vigenere cipher. (Google it for the algorithm, and you'll need a key too).
- The Vigenere key is provided by the wise man in the maze, by the switches on the north end.
- It's "OCTOPUS".
- In the penguin's room, use a Playfair cipher.
- Playfair ciphers depend on two conventions: order of filling the key sheet, and which letter to drop. Those are both given to you in the penguin's room.
- The Playfair key is provided in the east-of-start room where you learned the penguin's name.
- It's "PINEAPPLE".
- Only one baby is real. Touching any of the others will kill you. The real one is selected randomly each time you enter the room.
- The real baby will turn away when you look at it. (this is explained by a skeleton in the maze).
