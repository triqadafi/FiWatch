// Board version 4.0+
#include <Arduino.h>
// ------------------------------------------------------------------
// Configuration
// ------------------------------------------------------------------
// Pin map
#define PIN A3
#define PIN_BUTTON A1
#define PIN_SCL A5
#define PIN_SDA A4
#define PIN_LED_POWER 5
#define PIN_LED_POWER_AUX 6

// WATCH LED color
#define PIXEL_COLOR_OFF pixels.Color(0, 0, 0)
#define PIXEL_COLOR_WHITE pixels.Color(255, 255, 255)

#define PIXEL_COLOR_HOUR pixels.Color(255, 0, 0)
#define PIXEL_COLOR_MINUTE pixels.Color(0, 0, 255)
#define PIXEL_COLOR_PLUSMINUTE pixels.Color(127, 0, 255)

// development flag
#define FI_DEBUG if(false) // just remove FI_DEBUG when you want to show the serial print


// ------------------------------------------------------------------
// RGB LED
// ------------------------------------------------------------------
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels(12, PIN, NEO_GRB + NEO_KHZ800);


// ------------------------------------------------------------------
// DEEP SLEEP
// ------------------------------------------------------------------
#ifdef USE_LIBRARY_LOW_POWER
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include "LowPower.h"
void wakeUp(){
    // Just a handler for the pin interrupt.
}
#else
#include <avr/power.h>     //disable/anable AVR MCU peripheries (Analog Comparator, ADC, USI, Timers/Counters)
#include <avr/sleep.h>     //AVR MCU power management
#include <avr/interrupt.h> //manipulation of the interrupt flags
ISR(PCINT1_vect) {
    // Just a handler for the pin interrupt.
}
#endif 

// ------------------------------------------------------------------
// RTC
// ------------------------------------------------------------------
#include <Wire.h>
#include <DS1307.h>
DS1307 clock;

// ------------------------------------------------------------------
// Helper Functions Declaration
// ------------------------------------------------------------------
void fi_led_on();
void fi_led_off();
void fi_sleep();

// ------------------------------------------------------------------
// SETUP
// ------------------------------------------------------------------
void setup()
{
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LED_POWER, OUTPUT);
  pinMode(PIN_LED_POWER_AUX, OUTPUT);
  digitalWrite(PIN_LED_POWER, LOW);
  digitalWrite(PIN_LED_POWER_AUX, LOW);

	Serial.begin(9600);

  // default reset time
	clock.begin();
	clock.fillByYMD(2022,6,17);
	clock.fillByHMS(10,12,00);
	clock.setTime();//write time to the RTC chip

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  fi_led_off();

  delay(100);  
  FI_DEBUG Serial.println("Redi!");
}

// ------------------------------------------------------------------
// LOOP
// ------------------------------------------------------------------
void loop()
{
  fi_sleep();
  
  fi_led_on();

  delay(500);
  
  clock.getTime();
  int hourvalue = clock.hour;
  int minutevalue = clock.minute;

  FI_DEBUG Serial.print("[DEBUG] DISPLAYING RAW: ");
  FI_DEBUG Serial.print(hourvalue);
  FI_DEBUG Serial.print(":");
  FI_DEBUG Serial.println(minutevalue);

  // adjust based on watch
  if(hourvalue > 12) hourvalue -= 12;
  int result_minute = minutevalue / 5;
  int result_hour = hourvalue;
  int result_plusminute = clock.minute % 5;

  // show hour hand
  pixels.setPixelColor(result_hour-1, PIXEL_COLOR_HOUR);
  pixels.show();
  delay(500);

  // show minute hand
  pixels.setPixelColor(result_minute-1, PIXEL_COLOR_MINUTE);
  pixels.show();
  delay(500);

  // show plus hand
  for (int i = 0; i < result_plusminute; i++)
  {
    int _plusminute = result_minute+i;
    _plusminute = _plusminute % 12;
    
    pixels.setPixelColor(_plusminute, PIXEL_COLOR_PLUSMINUTE);
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(250);

    if(i < result_plusminute-1){
      pixels.setPixelColor(_plusminute, PIXEL_COLOR_OFF);
      pixels.setPixelColor(result_hour-1, PIXEL_COLOR_HOUR);
      pixels.setPixelColor(result_minute-1, PIXEL_COLOR_MINUTE);
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(250);
    }
  }
  fi_led_off();
  delay(200);
}


// ------------------------------------------------------------------
// Helper Functions
// ------------------------------------------------------------------
// power off
void fi_led_on()         // here we put the arduino to sleep
{
  digitalWrite(PIN_LED_POWER, HIGH);
  digitalWrite(PIN_LED_POWER_AUX, HIGH);
  delay(20); // give time for RGB led to turn on
  pixels.setBrightness(5);
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.show();   // Send the updated pixel colors to the hardware.

}
// power off
void fi_led_off()         // here we put the arduino to sleep
{
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.show();   // Send the updated pixel colors to the hardware.
  digitalWrite(PIN_LED_POWER, LOW);
  digitalWrite(PIN_LED_POWER_AUX, LOW);

}

void fi_sleep()         // here we put the arduino to sleep
{
#ifdef USE_LIBRARY_LOW_POWER
  // Allow wake up pin to trigger interrupt on low.
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), wakeUp, CHANGE);
  
  // Enter power down state with ADC and BOD module disabled.
  // Wake up when wake up pin is low.
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
  
  // Disable external pin interrupt on wake up pin.
  detachInterrupt(digitalPinToInterrupt(PIN_BUTTON)); 
#else
  // OPTIMIZED DEEP SLEEP

  cli();
  PCICR |= (1 << (PCIE1));
  PCMSK1 |= (1 << (PCIE1));

  power_all_disable();                 //disable all peripheries (ADC, Timer0, Timer1, Universal Serial Interface)
  /*              
  power_adc_disable();                 //disable ADC
  power_timer0_disable();              //disable Timer0
  power_timer1_disable();              //disable Timer2
  power_usi_disable();                 //disable the Universal Serial Interface module
  */

  /* Now is the time to set the sleep mode. In the Atmega8 datasheet
    * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
    * there is a list of sleep modes which explains which clocks and 
    * wake up sources are available in which sleep mode.
    *
    * In the avr/sleep.h file, the call names of these sleep modes are to be found:
    *
    * The 5 different modes are:
    *     SLEEP_MODE_IDLE         -the least power savings 
    *     SLEEP_MODE_ADC
    *     SLEEP_MODE_PWR_SAVE
    *     SLEEP_MODE_STANDBY
    *     SLEEP_MODE_PWR_DOWN     -the most power savings
    *
    * For now, we want as much power savings as possible, so we 
    * choose the according 
    * sleep mode: SLEEP_MODE_PWR_DOWN
    * 
    */  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here

  #if defined(BODS) && defined(BODSE)  //if MCU has bulit-in BOD it will be disabled, ATmega328P, ATtiny85, AVR_ATtiny45, ATtiny25  
  sleep_bod_disable();                 //disable Brown Out Detector (BOD) before going to sleep, saves more power
  #endif

  sleep_enable();          // enables the sleep bit in the mcucr register

  sei();

  sleep_mode();
  /*** NOTE: sketch will continue from this point after sleep ***/
  PCICR  &= ~(1 << PCIE1);   // disable pin change interrupts for A0 to A5
  power_all_enable();                 //disable all peripheries (ADC, Timer0, Timer1, Universal Serial Interface)
#endif
}
