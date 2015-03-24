part of ardisplay;

class LEDCommand extends Command {

    int _row, _col;
    Color _color;
    LEDState _command;

    LEDCommand(this._row, this._col, this._color, this._command);

    ArrayBuffer toBytes() {
        return new ArrayBuffer.fromBytes(10);
    }
}