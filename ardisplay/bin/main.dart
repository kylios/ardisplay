#!/usr/bin/env dart
// Copyright (c) 2015, <your name>. All rights reserved. Use of this source code
// is governed by a BSD-style license that can be found in the LICENSE file.

import 'package:args/args.dart';

import 'package:ardisplay/ardisplay.dart';


void parseArgs(argv) {

//    ArgParser parser = new ArgParser()
//        ..addFlag(name)
//    ArgResults results = parser.parse(arguments);

}

main(List<String> argv) {
    parseArgs(argv);

    String device = "/dev/tty.usbmodem1411";
    int baud = 9600;

    Ardisplay d = new Ardisplay(device, baud);

    d.connect();

    LEDCommand cmd1 = new LEDCommand(0, 0, Color.RED, LEDState.ON);
}
