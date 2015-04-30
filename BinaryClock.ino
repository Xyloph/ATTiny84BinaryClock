/* *
 * Binary Clock using RTC, 74HC595N and 6 2N2222A NPN
 */

#include <Time.h>
#include <I2cMaster.h>

#define SDA_PIN 3 // Used for RTC
#define SCL_PIN 4 // Used for RTC

#define LATCH   1 // Used for Shift Register
#define CLOCK   2 // Used for Shift Register
#define DATA    0 // Used for Shift Register

#define LED1    7 // Led out on ATTiny84
#define LED2    8 // Led out on ATTiny84
#define LED4    9 // Led out on ATTiny84
#define LED8   10 // Led out on ATTiny84

#define LED1V  1  // Constant used for LED bit  
#define LED2V  2  // Constant used for LED bit
#define LED4V  4  // Constant used for LED bit
#define LED8V  8  // Constant used for LED bit

// i2c 8-bit address for DS1307. low bit is read/write
#define DS1307ADDR 0XD0

// select software or hardware i2c
#define USE_SOFT_I2C 1

// Grounds byte used for bit shifting
byte grounds;

// SoftI2C used for reading
SoftI2cMaster rtc(SDA_PIN, SCL_PIN);

// Buffer for storing data from DS1307
uint8_t buf[8];

void setup()  
{
  setSyncProvider(getTime);   // the function to get the time from the RTC
  
  // Set output pins to, well, output mode.
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED8, OUTPUT);
  
  // Remove all the LEDS lit
  clearLeds();
}

void loop()
{
  // Set the initial ground byte, open to tens of hour.
  grounds = 0b10000000;
  
  // Display magic.
  time_t time = now();
  
  processUnits(hour(time), false);
  processUnits(minute(time), true);
  processUnits(second(time), true);
}

// Open the grounds requested through the shift register.
void updateGrounds()
{
  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, grounds);
  digitalWrite(LATCH, HIGH);
}

// Set all LED pins low to avoid weird multiplexing results.
void clearLeds()
{
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED4, LOW);
  digitalWrite(LED8, LOW);
}

// Shows a number by showing the tens then the units.
// Takes care of changing the grounds.
// If shift is false, do not shift grounds initially. Used for Hours.
void processUnits(uint8_t nbr, boolean shift)
{
  if (shift)
  {
    // Shift grounds
    grounds = grounds >> 1;
  }
  
  // Update the grounds
  updateGrounds();
  
  // Extract the tens and units.  
  uint8_t nbr1 = nbr % 10;
  uint8_t nbr10 = nbr / 10;
  
  digitalWrite(LED4, (nbr10 & LED4V) == LED4V ? HIGH : LOW);
  digitalWrite(LED2, (nbr10 & LED2V) == LED2V ? HIGH : LOW);
  digitalWrite(LED1, (nbr10 & LED1V) == LED1V ? HIGH : LOW);
  delay(1);
  
  // Remove all the LEDS lit
  clearLeds();
  
  // Shift grounds
  grounds = grounds >> 1;
  updateGrounds();
  
  digitalWrite(LED8, (nbr1 & LED8V) == LED8V ? HIGH : LOW);
  digitalWrite(LED4, (nbr1 & LED4V) == LED4V ? HIGH : LOW);
  digitalWrite(LED2, (nbr1 & LED2V) == LED2V ? HIGH : LOW);
  digitalWrite(LED1, (nbr1 & LED1V) == LED1V ? HIGH : LOW);
  delay(1);
  
  // Clear LEDs again
  clearLeds();
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}

//Read 'count' bytes from the DS1307 starting at 'address'
uint8_t readDS1307(uint8_t address, uint8_t *buf, uint8_t count) {
  // issue a start condition, send device address and write direction bit
  if (!rtc.start(DS1307ADDR | I2C_WRITE)) return false;

  // send the DS1307 address
  if (!rtc.write(address)) return false;

  // issue a repeated start condition, send device address and read direction bit
  if (!rtc.restart(DS1307ADDR | I2C_READ))return false;

  // read data from the DS1307
  for (uint8_t i = 0; i < count; i++) {

    // send Ack until last byte then send Ack
    buf[i] = rtc.read(i == (count-1));
  }

  // issue a stop condition
  rtc.stop();
  return true;
}

// Create a time object for the Time lib, allowing calls to now(), hour(), etc.
time_t getTime()
{
  tmElements_t tm;
  
  if (readDS1307(0, buf, 8))
  {
    // read successful
    tm.Second = bcd2dec(buf[0]);
    tm.Minute = bcd2dec(buf[1]);
    tm.Hour = bcd2dec(buf[2]);
    tm.Wday = bcd2dec(buf[3]);
    tm.Day = bcd2dec(buf[4]);
    tm.Month = bcd2dec(buf[5]);
    tm.Year = bcd2dec(buf[6]);
  }
  
  return(makeTime(tm));
}

