/*
Name: LED_matrix
Author: Kyle Racette <kracette(at)gmail(dot)com>

Controls an 8x8 matrix of RGB LED lights, using Pulse Width Modulation
to achieve 256 values of each color respectively.
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
 
struct control plexer;


/*
 * Allocate the entire matrix here in static memory
 */
byte matrix[8][8] = {

  {GREEN,         GREEN | RED,  RED,          RED,          RED,          RED,          RED,          RED},
  {GREEN,         GREEN,        GREEN,        GREEN | RED,  RED,          RED,          RED,          RED},
  {GREEN,         GREEN | RED,  RED,          RED,          RED,          RED,          RED,          RED},
  {BLUE | GREEN,  GREEN,        GREEN | RED,  RED,          RED,          RED,          RED,          GREEN | RED},
  {BLUE | GREEN,  BLUE | GREEN, GREEN,        GREEN | RED,  RED,          RED,          GREEN | RED,  GREEN | RED},
  {BLUE,          BLUE | GREEN, BLUE | GREEN, GREEN,        RED,          GREEN | RED,  GREEN | RED,  GREEN},
  {BLUE,          BLUE,         BLUE | GREEN, BLUE | GREEN, GREEN,        GREEN,        GREEN,        GREEN},
  {BLUE,          BLUE,         BLUE,         BLUE | GREEN, BLUE | GREEN, GREEN,        GREEN,        GREEN}
};
                        

/*
 * setup() - this function runs once when you turn your Arduino on
 * We set the three control pins to outputs
 */
void setup()
{
  plexer.data = 2;
  plexer.clock = 3;
  plexer.latch = 4;
  
  pinMode(plexer.data, OUTPUT);
  pinMode(plexer.clock, OUTPUT);
  pinMode(plexer.latch, OUTPUT);
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
      //red_row &= (row{j} & RED) << j << ;
      //val = val << 8;
      red_row |= ((val & RED) >> RED_SHIFT) << j;
      green_row |= ((val & GREEN) >> GREEN_SHIFT) << j;
      blue_row |= ((val & BLUE) >> BLUE_SHIFT) << j;
    }
    buffer[0] = anode;

    // XOR these all with ones because for the LED's a LOW current means on
    buffer[1] = red_row ^ B11111111;
    buffer[2] = green_row ^ B11111111;
    buffer[3] = blue_row ^ B11111111;

    digitalWrite(plexer.latch, LOW);
    shiftBytes(buffer);
    digitalWrite(plexer.latch, HIGH);
  }
}

void shiftBytes(byte* bs) {
  for (int i = 3; i >= 0; i--) {
    byte b = bs[i];
    shiftOut(plexer.data, plexer.clock, MSBFIRST, b);
  }
}
