const bindings = require('bindings')('png');

function encode({ pixels, width, height, palette, alpha = true }) {
  if (!(pixels instanceof Buffer)) {
    throw new Error('Pixels must be a valid buffer');
  }

  if (!Number.isInteger(width) || !Number.isInteger(height) || width <= 0 || height <= 0) {
    throw new Error('Dimensions must be positive integers');
  }

  return bindings.encode(pixels, width, height, palette, alpha);
}

function decode(data) {
  if (!(data instanceof Buffer)) {
    throw new Error('Input data must be a buffer');
  }

  return bindings.decode(data);
}

module.exports = {
  encode,
  decode,
};
