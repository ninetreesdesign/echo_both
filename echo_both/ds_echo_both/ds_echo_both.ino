/* UART Example, any character received on either
    the UART (HW serial) or USB serial
   is printed as a message to both ports.

   02/12/24 tweaked formatting
            printed file name and path
            added an OLED output
*/

#define HW1_SER   Serial1       // UART
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>    // minimal oled 128x64 driver

uint16_t CONSOLE_FLAG = 1;  // USB Monitor
uint16_t HWSER1_FLAG = 1;   // UART

/* get the file name and path of this program */
const char this_file[] PROGMEM = __FILE__;          // provided by system
const String file_and_path = String(this_file);     // convert to a String

int n = file_and_path.length();
int i = file_and_path.lastIndexOf("\\") + 1;        // escaped backslash for Windows paths
String filename = file_and_path.substring(i, n);    // from last '\' to end
String filepath = file_and_path.substring(0, i);    // from begin to last '\'
String msg = "";
byte led_pin = 13; // on-board LED
uint32_t t0 = 999;   // ON period (1/frequency)
uint32_t d1 =  60;   // ON duration

void setup() {
  pinMode(led_pin, OUTPUT);
  while (!Serial && (millis() < 6000)) {  // continues if the monitor window is never opened
    ; // wait for serial port to connect. Needed for Leonardo & Teensyduino 3
  }
  Serial.println("USB Console port. \n");

  Serial1.begin(9600);
  Serial1.println("Hardware Serial port. \n");

  initOled();

  echo("\n");
  echo("filepath: " + filepath); // Serial.println(filepath);
  echo("\n");
  echo("filename: " + filename); // Serial.println(filename);
  echo("\n");
  echo("Secs stream automatically; type in either window to echo characters to both ports.\n");
}

void loop() {
  char inB;
  char inC;

  // blink light and print a msg at a fixed interval in addition to other inputs
  if (millis() % t0 == d1) {
    digitalWrite(led_pin, HIGH);   // turn the LED on (HIGH is the voltage level)
    float x = (millis() + random(-200, 200)) * 10 / 10000.0;
    msg = (String)(x) + "    \n";
    echo(msg);
    oled.setTextXY(5, 4);           // Set cursor position, line 2 10th character
    oled.putString(msg);

    delay(1);       // skip 'til next tick
    digitalWrite(led_pin, LOW);    // turn the LED off
  }

  // capture and echo characters from either port
  if (Serial.available() > 0) {
    digitalWrite(led_pin, HIGH);   // turn the LED on (HIGH is the voltage level)
    inB = Serial.read();
    Serial.print("USB:    ");
    if (inB < 48) Serial.print(' ');
    else Serial.print(inB);
    Serial.print("   ");
    Serial.println(inB, DEC); // print numerical value of ascii char

    HW1_SER.print("USB: ");
    if (inB < 48) HW1_SER.print(' ');
    else HW1_SER.print(inB);
    HW1_SER.print("   ");
    HW1_SER.println(inB, DEC);
    digitalWrite(led_pin, LOW);   // turn the LED off
  }
  if (HW1_SER.available() > 0) {
    inC = HW1_SER.read();
    Serial.print(millis() * 100 / 10000.0);  // print a time-varying value to see consistent output
    Serial.print("   ");
    if (inC < 48) Serial.print(' ');  // print space for characters below printables
    else Serial.print(inC);
    Serial.print("   ");
    Serial.println(inC, DEC);

    HW1_SER.print("   ");
    if (inC < 48) HW1_SER.print(' ');
    else HW1_SER.print(inC);
    HW1_SER.print("   ");
    HW1_SER.println(inC, DEC);
  }

}


void initOled()
{
  Wire.begin();
  oled.init();                      // Initialze SSD1306 OLED display
  oled.clearDisplay();              // Clear screen
  oled.setTextXY(0, 0);             // Set cursor position, start of line 0
  oled.putString("Cambrian Works");
  oled.setTextXY(1, 3);             // Set cursor position, start of line 1
  oled.putString(" - * - ");
  oled.setTextXY(2, 3);            // Set cursor position, line 2 10th character
  oled.putString(" ----- ");
}


void echo(String msg) {
  // general fctn to print a string, routed to various ports depending on global flags
  if (HWSER1_FLAG) Serial1.print(msg);
  if (CONSOLE_FLAG) Serial.print(msg);
}
