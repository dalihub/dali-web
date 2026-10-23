// Executes only the access guards extracted from the pinned SDK, not WebGL.
import fs from 'node:fs';
import crypto from 'node:crypto';
import assert from 'node:assert/strict';
const sdk = process.argv[2];
assert(sdk, 'usage: node check-map-guards.mjs /path/to/emsdk');
const source = fs.readFileSync(`${sdk}/upstream/emscripten/src/lib/libwebgl.js`, 'utf8');
const start = source.indexOf('  glMapBufferRange: (target, offset, length, access) => {');
assert(start >= 0);
const bodyStart = source.indexOf('{', start) + 1;
const end = source.indexOf('    if (!emscriptenWebGLValidateMapBufferTarget(target))', bodyStart);
assert(end > bodyStart);
const guard = new Function('access', 'err', source.slice(bodyStart, end) + '\nreturn "guards-passed";');
const results = [];
for (const [access, expected] of [[2, 0], [6, 'guards-passed'], [10, 'guards-passed'], [3, 0], [34, 0]]) {
  const messages = [];
  const actual = guard(access, text => messages.push(text));
  assert.equal(actual, expected);
  results.push({access, actual, messages});
}
console.log(JSON.stringify({
  node: process.version,
  sdkSource: 'upstream/emscripten/src/lib/libwebgl.js',
  sha256: crypto.createHash('sha256').update(source).digest('hex'),
  results,
  limitation: 'SDK access guards only. No GL context, allocation, upload, DALi or browser execution.'
}, null, 2));
