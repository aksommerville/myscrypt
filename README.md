# Mysteries of the Crypt

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
- [ ] Dialogue.
- [ ] Transitions.
- [ ] Maps.
- [ ] Music.
- [ ] Sound effects.
- [ ] Hello modal.
- [ ] Gameover modal.
- [ ] Itch page.
