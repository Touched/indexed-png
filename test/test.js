const assert = require('assert');
const fs = require('fs');
const path = require('path');
const gary = require('./fixtures/gary.js');
const { encode, decode } = require('../');

function readFile(filePath) {
  return fs.readFileSync(path.join(__dirname, filePath));
}

describe('PNG', () => {
  it('can decode and encode', () => {
    const encoded = encode({
      pixels: new Buffer(gary.data),
      width: 16,
      height: 32,
      palette: gary.palette,
    })

    assert.deepEqual(decode(encoded), {
      width: 16,
      height: 32,
      alpha: true,
      palette: gary.palette,
      pixels: new Buffer(gary.data),
    });
  });

  describe('Decode', () => {
    it('throws an exception for unsupported PNG formats', () => {
      assert.throws(() => {
        decode(readFile('fixtures/unindexed.png'));
      }, /conversion to indexed colors is unsupported/);
    });

    it('rejects invalid input', () => {
      assert.throws(() => {
        decode();
      }, /Input data must be a buffer/);
    });
  });

  describe('Encode', () => {
    it('must throw an exception if the buffer does not contain enough data', () => {
      assert.throws(() => {
        encode({
          pixels: new Buffer([]),
          width: 100,
          height: 100,
          palette: [],
        });
      }, /Not enough data/);
    });

    it('must throw an exception if the PNG encoding fails', () => {
      assert.throws(() => {
        encode({
          pixels: new Buffer([0]),
          width: 1,
          height: 1,
          palette: [],
        });
      }, /Valid palette required for paletted images/);
    });

    it('rejects invalid data', () => {
      assert.throws(() => {
        encode({});
      }, /Pixels must be a valid buffer/);
    });
  });
});
