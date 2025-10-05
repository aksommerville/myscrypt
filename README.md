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
- [ ] Graphics.
- [x] Dialogue.
- [x] Global store.
- [ ] Hazards.
- [x] Password entry.
- [x] Bump: Actuate only if the off-axis is so close to alignment, say +-0.5? I keep triggering dialogue with my toe.
- [ ] Transitions.
- [ ] Maps.
- [x] Music.
- [ ] Sound effects.
- [ ] Hello modal.
- [ ] Gameover modal.
- [ ] Itch page.
- [ ] Can we prevent hero from staying in a stepping frame while dialogue runs?
- [ ] Redraw the font simpler. Have pity on the poor player, they are going to write it all by hand.

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
