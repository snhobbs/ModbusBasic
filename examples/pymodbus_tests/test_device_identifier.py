#!/usr/bin/env python3
from pycrc.algorithms import Crc
from serial import Serial
from time import sleep
import sys
import click

@click.command()
@click.option("--device", "-d", default="/tmp/ptyp0")
@click.option("--address", "-a", type=int, default=3)
@click.option("--repeat", "-r", type=int, default=0)
@click.option("--period", "-p", type=float, default=1)
@click.option("--baud", "-d", type=int, default=9600)
def run(device, address, repeat, period, baud):
    s = Serial(device, baud)
    crc = Crc(width = 16, poly = 0x8005,reflect_in = True, xor_in = 0xffff, reflect_out = True, xor_out = 0x0000)

    count = 0
    while True:
        if count > repeat + 1 and repeat > 0:
            break
        else:
            count -= 1
        function = 0x2b
        frame = [address, function, 0x0f, 0x01, 0x00]
        crc_calc = crc.bit_by_bit(frame)
        frame.append(crc_calc & 0xff)
        frame.append((crc_calc >> 8) & 0xff)
        s.write(bytes(frame))
        print([hex(d) for d in frame])
        data = (s.read_all())
        print(data, len(data))
        print([chr(d) for d in data])
        sleep(period)

if __name__ == "__main__":
    run()
