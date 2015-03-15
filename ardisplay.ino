/*
Name: LED_matrix
Author: Kyle Racette <kracette(at)gmail(dot)com>

Controls an 8x8 matrix of RGB LED lights, using Pulse Width Modulation
to achieve 256 values of each color respectively.

Some really useful links:
http://www.vetco.net/catalog/product_info.php?products_id=14087
https://plus.google.com/photos/106630138154329739101/albums/5287314028583882001/5286867776369729106?pid=5286867776369729106&oid=106630138154329739101
http://www.vetco.net/catalog/images/VUPN6866-ModuleSchematic.jpg
http://www.vetco.net/catalog/images/VUPN6866-pin_numbering.jpg
*/
 
// Simple control structure for the shift registers.
struct control {
  short data;
  short clock;
  short latch;
};

#define RED B00000100
#define GREEN B00000010
#define BLUE B00000001

#define RED_SHIFT 2
#define GREEN_SHIFT 1
#define BLUE_SHIFT 0

struct color {
  byte rgb;
};
 
struct control control;


/*
 * Allocate the entire matrix here in static memory
 */
byte matrix[8][8] = {

  {GREEN,         GREEN,        GREEN,        GREEN | RED,  GREEN | RED,  RED,          RED,          RED},
  {GREEN,         GREEN,        GREEN,        GREEN,        GREEN | RED,  GREEN | RED,  RED,          RED},
  {GREEN,         GREEN,        GREEN,        GREEN,        GREEN,        GREEN | RED,  GREEN | RED,  RED},
  {BLUE | GREEN,  GREEN,        GREEN,        GREEN,        GREEN,        GREEN,        GREEN | RED,  GREEN | RED},
  {BLUE | GREEN,  BLUE | GREEN, GREEN,        GREEN,        GREEN,        GREEN,        GREEN,        GREEN | RED},
  {BLUE,          BLUE | GREEN, BLUE | GREEN, GREEN,        GREEN,        GREEN,        GREEN,        GREEN },
  {BLUE,          BLUE,         BLUE | GREEN, BLUE | GREEN, GREEN,        GREEN,        GREEN,        GREEN},
  {BLUE,          BLUE,         BLUE,         BLUE | GREEN, BLUE | GREEN, GREEN,        GREEN,        GREEN}
};
                        

/*
 * setup() - this function runs once when you turn your Arduino on
 * We set the three control pins to outputs
 */
void setup()
{
  control.data = 2;
  control.clock = 3;
  control.latch = 4;
  
  pinMode(control.data, OUTPUT);
  pinMode(control.clock, OUTPUT);
  pinMode(control.latch, OUTPUT);
}

/*
 * loop() - this function will start after setup finishes and then repeat
 * we set which LEDs we want on then call a routine which sends the states to the 74HC595
 */
void loop()
{
  writeMatrix(matrix);
}


/**
 * Writes an 8x8 byte matrix out to the shift registers.
 */
void writeMatrix(byte matrix[8][8]) {
  byte anode, val;
  byte red_row, green_row, blue_row;
  byte buffer[4];
  for (int i = 0; i < 8; i++) {
    // multiplex one column at a time
    anode = 1 << i;

    // reset all the rows
    green_row = red_row = blue_row = B00000000;

    // set our row bytes for each color
    for (int j = 0; j < 8; j++) {
      byte val = matrix[i][j];

      // TODO: maybe there's a better way to do this?
      red_row |= ((val & RED) >> RED_SHIFT) << j;
      green_row |= ((val & GREEN) >> GREEN_SHIFT) << j;
      blue_row |= ((val & BLUE) >> BLUE_SHIFT) << j;
    }
    buffer[0] = anode;

    // XOR these all with ones because for the LED's a LOW current means on
    buffer[1] = red_row ^ B11111111;
    buffer[2] = green_row ^ B11111111;
    buffer[3] = blue_row ^ B11111111;

    digitalWrite(control.latch, LOW);
    shiftBytes(buffer);
    digitalWrite(control.latch, HIGH);
  }
}

void shiftBytes(byte* bs) {
  // The last bits must get shifted in first
  for (int i = 3; i >= 0; i--) {
    byte b = bs[i];
    shiftOut(control.data, control.clock, MSBFIRST, b);
  }
}
