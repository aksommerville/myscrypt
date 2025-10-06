const alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// ["decrypt",KEY,CIPHERTEXT] to decrypt Vigenere
if (process.argv[2] === "decrypt") {
  let dst = "";
  const key = process.argv[3].toUpperCase();
  const ctext = process.argv[4].toUpperCase();
  for (let keyp=0, ctextp=0; ctextp<ctext.length; ctextp++) {
    const ch = ctext.charCodeAt(ctextp);
    if ((ch < 0x41) || (ch > 0x5a)) { // Not a letter, emit verbatim and do not advance key position.
      dst += String.fromCharCode(ch);
      continue;
    }
    const kch = key[keyp];
    if (++keyp >= key.length) keyp = 0;
    const an = alphabet.indexOf(String.fromCharCode(ch));
    const bn = alphabet.indexOf(kch);
    if ((an < 0) || (bn < 0)) throw new Error(`Not a letter: ${ch} or ${kch}`);
    let cn = an - bn;
    if (cn < 0) cn += 26;
    dst += String.fromCharCode(0x41 + cn);
  }
  console.log(dst);
  process.exit(0);
}

// [A,B] or [A,"-"B] to add or subtract one pair
if (process.argv.length !== 4) throw new Error("Usage: addletter.js A B");
const a = process.argv[2].toUpperCase();
let b = process.argv[3].toUpperCase();
let op = "+";
if (b.startsWith("-")) {
  op = "-";
  b = b.substring(1);
}
const an = alphabet.indexOf(a);
const bn = alphabet.indexOf(b);
if ((an < 0) || (bn < 0)) throw new Error(`Not a letter: ${JSON.stringify(a)} or ${JSON.stringify(b)}`);
let cn = (op === "+") ? (an + bn) : (an - bn);
if (cn < 0) cn += 26;
else if (cn >= 26) cn -= 26;
console.log(alphabet[cn]);
