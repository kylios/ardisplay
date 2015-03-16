#!/usr/bin/env python

'''
File: ardisplay.py
Author: Kyle Racette <kracette@gmail.com>

Controls the arduino LED display over the serial port.
Can be imported as a library or
controlled via the command line.  Just type ./ardisplay.py --help
'''

from __future__ import print_function

import sys
import serial
import argparse
import time


class Ardisplay:

    def __init__(self, tty, baud=9600):
        '''
        Initialize the ardisplay.

        Arguments:
        tty -- The serial device to use for communication.

        Keyword Arguments:
        baud -- The baud rate to use (bps)
        '''
        self.ser = serial.Serial(tty, baud, timeout=1)

        time.sleep(1)

        print(tty)
        print(baud)

    def send_command(self, command):
        '''
        Send a single command to the ardisplay to turn an LED color on or off.

        A command should be a dictionary with the following members:
        - color: 'r', 'g', or 'b'
        - on: True or False
        - row: int
        - col: int
        '''

        # do this stuff with bits so we're super explicit
        color = b'\x01' if command['color'] == 'b' else \
                b'\x02' if command['color'] == 'g' else \
                b'\x04'  # if command['color'] == 'r'

        cmd = b'\xFF' if command['on'] else b'\x00'

        row = chr(command['row'])
        col = chr(command['col'])

        self.ser.write(row)
        self.ser.write(col)
        self.ser.write(color)
        self.ser.write(cmd)


def parse_args(args):
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter,
        description="Communicate with the ardisplay via serial port.",
        epilog="""
The script takes a
sequence of commands made up of a color, a row, a column, and a
command.  Colors take the form 'r', 'g', 'b', or their full name
equivalents.  The row and column are zero-indexed integers
referring to the LED to command.  The command must be either
'on' or 'off'.  The arguments themselves may be given in any order,
but the order must be consistent among each parameter of the
commands.  For example:

./argparse.py -o r -r 4 -c 3 -C on -o g -r 4 -c 2 -C off

is equivalent to

./argparse.py -o r -o g -r 4 -r 4 -c 3 -c 2 -C on -C off

""")
    parser.add_argument("--device", type=str, required=True,
                        help="The tty device to communicate with.")
    parser.add_argument("--baud", type=int, default=9600,
                        help="The baud rate to use.  Defaults to 9600.")
    parser.add_argument("-o", "--color", type=str, action='append',
                        choices=('r', 'g', 'b', 'red', 'green', 'blue'),
                        required=True,
                        dest="color",
                        help="The color to display")
    parser.add_argument("-C", "--command", type=str, action='append',
                        choices=('on', 'off'),
                        required=True,
                        dest="command",
                        help="The command.")
    parser.add_argument("-r", "--row", type=int, action='append',
                        required=True,
                        dest="row",
                        help="The row.")
    parser.add_argument("-c", "--col", type=int, action='append',
                        required=True,
                        dest="col",
                        help="The column.")

    options = parser.parse_args(args)

    # make sure they passed the same number of arguments for 'command', 'color'
    # 'row', and 'col'
    if len(options.command) != len(options.color) or \
       len(options.color) != len(options.row) or \
       len(options.row) != len(options.col):

        print("ERROR: command, color, row, and col must contain the same "
              "number of arguments")
        sys.exit(2)

    # Just combine 4 arguments into a dictionary
    def combine(color, row, col, command):
        return {
            'color': color,
            'row': int(row),
            'col': int(col),
            'on': True if command == 'on' else False
        }

    # convert the command args into a single list of commands
    options.commands = \
        map(combine, options.color, options.row, options.col, options.command)

    # remove the options we don't need anymore
    del options.command
    del options.color
    del options.row
    del options.col

    return options


def main(args):
    '''
    Execute the program.  args is a simple object with the command line
    options.
    '''

    print(args)

    display = Ardisplay(args.device, args.baud)
    for cmd in args.commands:
        display.send_command(cmd)


if __name__ == '__main__':
    args = parse_args(sys.argv[1:])
    main(args)
