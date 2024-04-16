#include <Adafruit_NeoPixel.h>
#include <math.h>

#define NOTE_C4 523

const int max_distance = 335;
const float distance_level = 41.62;
const int PARKING_BUTTON_PIN = 3;
int buttonState = 0;
int lastButtonState = 0;
int parking_led_state = LOW;

const int PARKING_STATUS_LED = 5; // parking status led

const int PIN_RED = 13;
const int PIN_BLUE = 12;
const int PIN_GREEN = 11;

const int PIXELS_PIN = 2;

int red_limit = 1023 * 0.3;
int yellow_limit = 1023* 0.6;
int green_limit = 1023;

int potPin = A2;
int potVal = 0;

int grnVal = 0;
int redVal = 0;
int bluVal = 0;

const int stripe_red_limit = 8;
const int stripe_orange_limit = 6;
const int stripe_yellow_limit = 4;
const int stripe_green_limit = 2;

unsigned long previousMillis = 0UL;
unsigned long intervalYellow = 1000;
unsigned long intervalOrange = 600;
unsigned long intervalRed = 200;

double cm = 0;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(8, PIXELS_PIN, NEO_GRB + NEO_KHZ800);


void alarm(unsigned int interval)
{
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval * 2) {
   	// execute this code every interval ms
    tone(9, NOTE_C4, interval);
    previousMillis = currentMillis;
  }
  
}

long readUltrasonicDistance(int triggerPin, int echoPin) 
{
  pinMode(triggerPin, OUTPUT); // clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  
  return pulseIn(echoPin, HIGH);
  
}

// Write values to LED pins
void rgb_engine_color()
{
  analogWrite(PIN_RED, redVal);    
  analogWrite(PIN_BLUE, bluVal); 
  analogWrite(PIN_GREEN, grnVal);
}

void engine_status() 
{
  potVal = analogRead(potPin);   // read the potentiometer value at the input pin
  	if (potVal >= 0 && potVal < red_limit)  // Lowest third of the potentiometer's range (0-340)
  	{                  
      engine_off();
  	}
  	if (potVal < yellow_limit && potVal >= red_limit) // Middle third of potentiometer's range (341-681)
  	{
    	accessory_mode();
  	}
  	if(potVal >= yellow_limit)  // Upper third of potentiometer"s range (682-1023)
  	{
      engine_on();
    }
}

void engine_off() 
{
  grnVal = 0;
  bluVal = 0;
  redVal = 255;
  
  rgb_engine_color();
  digitalWrite(PARKING_STATUS_LED, LOW);
  parking_led_state = LOW;
  
}

void accessory_mode() 
{
  bluVal = 0;
  grnVal = 255;
  redVal = 255; 
  
  rgb_engine_color();
  analogWrite(PARKING_STATUS_LED, HIGH);
  parking_led_state = HIGH
}

void engine_on() 
{
  grnVal = 255;
  redVal = 0;
  bluVal = 0;
  
  rgb_engine_color();
  analogWrite(PARKING_STATUS_LED, HIGH);
  parking_led_state = HIGH;
}



void color_stripe_on(int level){
	for (int i=0; i < level; i++) {
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      if(i < stripe_green_limit){
      	pixels.setPixelColor(i, pixels.Color(0, 255, 0));
      }else if(i >= stripe_green_limit && i < stripe_yellow_limit){
      	pixels.setPixelColor(i, pixels.Color(255, 255, 0));
        alarm(intervalYellow);
      }
      else if(i >= stripe_yellow_limit && i < stripe_orange_limit){
      	pixels.setPixelColor(i, pixels.Color(255, 165, 100));
        alarm(intervalOrange);
      }
      else if(i >= stripe_orange_limit){
      	pixels.setPixelColor(i, pixels.Color(255, 0, 0));
        alarm(intervalRed);
      }
      
      
      // This sends the updated pixel color to the hardware.
      pixels.show();

      // Delay for a period of time (in milliseconds).
      delay(10);
       }
}
void color_stripe_off(int level){
	for (int i=7; i >= level; i--) {
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));

      // This sends the updated pixel color to the hardware.
      pixels.show();

      // Delay for a period of time (in milliseconds).
      delay(10);
       }
}

void parking_mode()
{
  buttonState = digitalRead(PARKING_BUTTON_PIN);
  
  	if(potVal > red_limit) // checks if engine is on
    {
      if(buttonState != lastButtonState) 
      {
        lastButtonState = buttonState;
        if(buttonState == LOW)
        {
          if(parking_led_state == HIGH)
          {
            parking_led_state = LOW; // Led off
            Serial.println("Parking Mode OFF");
          }
          else
          {
            parking_led_state = HIGH; // Led on
            Serial.println("Parking Mode ON");

          }
          digitalWrite(PARKING_STATUS_LED, parking_led_state);

        }

      }
    }
}

void setup()
{
  pinMode(PARKING_BUTTON_PIN, INPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PARKING_STATUS_LED, OUTPUT);
  pinMode(9, OUTPUT); // buzzer pin
  Serial.begin(9600);
  
  analogWrite(PARKING_STATUS_LED, HIGH);
  pixels.begin();
}

void loop()
{
  	delay(10);
	
   	engine_status();
  	parking_mode();
  	// when engine is on the ultrasonic sensor starts reading distance
  	if(potVal > red_limit) 
    {
      cm = 0.01723 * readUltrasonicDistance(7, 7);
    
    }
  
  if(parking_led_state == LOW && potVal > red_limit){
    color_stripe_on(round((max_distance-cm)/distance_level));
  	color_stripe_off(round((max_distance-cm)/distance_level));
  }
  if(parking_led_state == LOW || potVal <= red_limit)
     color_stripe_off(0);
  	
  	//Serial.println(cm);
  

  delay(10);
  
}