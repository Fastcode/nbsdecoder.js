const fs = require('fs');
const os = require('os');
const path = require('path');
const { test } = require('uvu');
const assert = require('uvu/assert');

const { NbsDecoder, NbsEncoder } = require('..');

const pingType = Buffer.from('8ce1582fa0eadc84', 'hex'); // nuclear hash of 'message.Ping'
const pongType = Buffer.from('37c56336526573bb', 'hex'); // nuclear hash of 'message.Pong'

function usingTempDir(callback) {
  const tempDir = fs.mkdtempSync(`${os.tmpdir()}${path.sep}`);
  try {
    callback(tempDir);
  } finally {
    fs.rmSync(tempDir, { recursive: true });
  }
}

test('NbsEncoder constructor throws for invalid arguments', () => {
  assert.throws(
    () => {
      new NbsEncoder();
    },
    /missing argument `path`: provide a path to write to/,
    'NbsEncoder() constructor throws without path argument'
  );

  assert.throws(
    () => {
      new NbsEncoder(1);
    },
    /invalid argument `path`: expected string/,
    'NbsEncoder() constructor throws with incorrect type for path argument'
  );
});

test('NbsEncoder.write() throws for invalid arguments', () => {
  usingTempDir((dir) => {
    const file = path.join(dir, 'output.nbs');
    const encoder = new NbsEncoder(file);

    assert.throws(
      () => {
        encoder.write();
      },
      /missing argument `packet`: provide a packet to write to the file/,
      'NbsEncoder.write() throws without packet argument'
    );

    assert.throws(
      () => {
        encoder.write({});
      },
      /invalid type for argument `packet`: expected object with `timestamp` key/,
      'NbsEncoder.write() throws without timestamp in packet'
    );

    assert.throws(
      () => {
        encoder.write({
          timestamp: { seconds: 1897, nanos: 0 },
        });
      },
      /invalid type for argument `packet`: expected object with `type` key/,
      'NbsEncoder.write() throws without type in packet'
    );

    assert.throws(
      () => {
        encoder.write({
          timestamp: { seconds: 1897, nanos: 0 },
          type: pingType,
          subtype: 0,
        });
      },
      /invalid type for argument `packet`: expected object with `payload` key/,
      'NbsEncoder.write() throws without payload in packet'
    );

    assert.throws(
      () => {
        encoder.write({
          timestamp: 'string',
          type: pingType,
          subtype: 0,
          payload: Buffer.from('ping.699', 'utf8'),
        });
      },
      /invalid type for argument `packet`: error in `timestamp`: expected positive number or BigInt or timestamp object/,
      'NbsEncoder.write() throws with incorrect type for timestamp'
    );

    assert.throws(
      () => {
        encoder.write({
          timestamp: { wrongKey: 1 },
          type: pingType,
          subtype: 0,
          payload: Buffer.from('ping.699', 'utf8'),
        });
      },
      /invalid type for argument `packet`: error in `timestamp`: expected object with `seconds` and `nanos` keys/,
      'NbsEncoder.write() throws for incorrect keys in timestamp'
    );

    assert.throws(
      () => {
        encoder.write({
          timestamp: { seconds: 'string', nanos: 'string' },
          type: pingType,
          subtype: 0,
          payload: Buffer.from('ping.699', 'utf8'),
        });
      },
      /invalid type for argument `packet`: error in `timestamp`: `seconds` and `nanos` must be numbers/,
      'NbsEncoder.write() throws with incorrect types for timestamp properties'
    );

    assert.throws(
      () => {
        encoder.write({
          timestamp: { seconds: 1897, nanos: 0 },
          type: pingType,
          subtype: 'string',
          payload: Buffer.from('ping.699', 'utf8'),
        });
      },
      /invalid type for argument `packet`: expected `subtype` to be a number/,
      'NbsEncoder.write() throws with incorrect type for subtype'
    );

    encoder.close();
  });
});

test('Packets written by NbsEncoder can be read by NbsDecoder', () => {
  usingTempDir((dir) => {
    const file = path.join(dir, 'output.nbs');
    const encoder = new NbsEncoder(file);

    const pingPacket = {
      timestamp: { seconds: 1897, nanos: 0 },
      type: pingType,
      subtype: 0,
      payload: Buffer.from('ping.699', 'utf8'),
    };
    const pongPacket = {
      timestamp: { seconds: 1898, nanos: 0 },
      type: pongType,
      subtype: 0,
      payload: Buffer.from('pong.699', 'utf8'),
    };

    encoder.write(pingPacket);
    encoder.write(pongPacket);
    encoder.close();

    const decoder = new NbsDecoder([file]);
    const packets = decoder.getPackets({ seconds: 1899, nanos: 0 });

    assert.equal(packets.length, 2);
    assert.equal(packets[0], pingPacket);
    assert.equal(packets[1], pongPacket);
    decoder.close();
  });
});

test.run();
