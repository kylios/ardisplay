// Copyright (c) 2015, <your name>. All rights reserved. Use of this source code
// is governed by a BSD-style license that can be found in the LICENSE file.

/// The ardisplay library.
library ardisplay;

import 'dart:async';
import 'package:chrome/chrome_app.dart';

part 'src/ardisplay/commands.dart';

enum LEDState { ON, OFF }
enum Color { RED, GREEN, BLUE }

abstract class Command {

    ArrayBuffer toBytes();
}

class Ardisplay {

    /// our serial device.  defaults to chrome.serial's singleton.
    var _serial = serial;

    ConnectionInfo _con;
    Future<ConnectionInfo> _openFuture;
    bool _connectionOpened = false;

    String _device;
    int _bitrate;

    List<Command> _cmdQueue = new List<Command>();

    Ardisplay(this._device, this._bitrate);

    void connect() {

        var options = new ConnectionOptions(bitrate: this._bitrate);

        // Save the future.  It will be used to start the communications.
        this._openFuture = this._serial.connect(this._device, options)
                .then(this._beginSerial);
    }

    void sendCommand(Command cmd) {

        if (this._connectionOpened) {
            this._queue(cmd);
        } else {
            this._process(cmd);
        }
    }

    void _beginSerial(ConnectionInfo con) {
        this._con = con;
        this._connectionOpened = true;

        this._cmdQueue.forEach(this._process);
    }

    void _queue(cmd) => this._cmdQueue.add(cmd);

    void _process(cmd) =>
            this._serial.send(this._con.connectionId, cmd.toBytes());

}
