# Indexed PNG
An indexed PNG encoder and decoder C++ native module for Node.js

## Why?
As of the time of writing, there is no working PNG module that can
read and write PNG data with palette information without errors. Most
modules only support reading indexed images, and even then they can
only return RGBA pixel data, not the raw indexed pixel data.

# Usage
```javascript
// ES6
import png from '@touched/indexed-png';

// Common.js
const png = require('@touched/indexed-png');

// Decoding
const decoded = png.decode(fs.readFileSync('example.png'));
// { width: ..., height: ..., palette: [[0, 0, 0, 0], ...], pixels: <Buffer> }

// Encoding
const pixels = Buffer(10 * 10).fill(0);
const data = { width: 10, height: 10, palette: [[0, 0, 0, 255]], pixels };
fs.writeFileSync('decoded.png', png.encode(data));
```

# License

GPLv3
