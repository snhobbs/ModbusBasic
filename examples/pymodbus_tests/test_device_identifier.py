#!/usr/bin/env python3
from pycrc.algorithms import Crc
from serial import Serial
from time import sleep
import sys

device = "/tmp/ptyp0"
if len(sys.argv) > 1:
    device = sys.argv[1]

slave_address = 0x03
if len(sys.argv) > 2:
    slave_address = int(sys.argv[2])

s = Serial(device, 9600)
# s = Serial("/dev/ttyUSB0", 9600)
crc = Crc(
    width=16,
    poly=0x8005,
    reflect_in=True,
    xor_in=0xFFFF,
    reflect_out=True,
    xor_out=0x0000,
)

function = 0x2B
frame = [slave_address, function, 0x0F, 0x01, 0x00]

crc_calc = crc.bit_by_bit(frame)
frame.append(crc_calc & 0xFF)
frame.append((crc_calc >> 8) & 0xFF)
s.write(bytes(frame))
print([hex(d) for d in frame])
sleep(2)
data = s.read_all()
print(data, len(data))
print([chr(d) for d in data])
