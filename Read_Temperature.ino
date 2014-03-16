/***************************************************
  Thermistor Calibration
 
  Find the Steinhart-Hart equation coefficients for a thermistor.
  
  See video of calibration here: http://www.youtube.com/watch?v=5qDVmvCPNdQ
  
  Copyright 2013 Tony DiCola (tony@tonydicola.com).
  Released under an MIT license: 
    http://opensource.org/licenses/MIT
  
 ****************************************************/

#define     THERMISTOR_PIN     0      //  A0 Pin between the thermistor and 
                                      // series resistor.

#define     SERIES_RESISTOR    9960  // Series resistor value in ohms.

#define     ADC_SAMPLES        5      // Number of ADC samples to average
                                      // when taking a reading.

// Temperature unit conversion functions and state.
typedef float (*TempConversion)(float);
TempConversion ToKelvin; 
TempConversion FromKelvin;
char* TempUnit;

// Steinhart-Hart coefficients for  Canadian Tire Probes
float A = 0.000881695175;
float B = 0.000180855655;
float C = 0.000000138945;
boolean ledState= HIGH;
int redBoardLed= 13; // Use the  LED on pin 13
int doneLed= 12;
void setup(void) {
  Serial.begin(9600);
  analogReference(DEFAULT);
  pinMode(redBoardLed, OUTPUT);// Setup and initialize Warning LEDs
  pinMode(doneLed,OUTPUT);
  digitalWrite(redBoardLed,LOW);
  digitalWrite(doneLed,LOW);
    ToKelvin = &celsiusToKelvin;
    FromKelvin = &kelvinToCelsius;
    TempUnit = "Celsius";
  
  
  Serial.println(F("Thermistor probe Readings"));
}

void loop(void) {
  float temp = FromKelvin(readTemp());
  Serial.print(F("Temperature: ")); Serial.print(temp); Serial.print(F(" ")); Serial.println(TempUnit);
  if (temp>66) {digitalWrite(redBoardLed, ledState);   // Toggle the LED 
  ledState =!ledState;
  }
  if(temp>=71)digitalWrite(doneLed,HIGH);
  delay(1000);
}

void waitForOk() {
  Serial.println(F("Type OK <enter> to continue.\n"));
  while (!(Serial.findUntil("OK", "\n")));
}

float waitForFloat() {
  while (!(Serial.available() > 0));
  return Serial.parseFloat();
}

void printTempMessage() {
  Serial.println(F("Wait about 30 seconds for the thermistor to stabilize then,"));
  Serial.print(F("type the water temperature (in ")); Serial.print(TempUnit); Serial.println(F(") and press <enter>.\n"));
}

double readResistance() {
  float reading = 0;
  for (int i = 0; i < ADC_SAMPLES; ++i) {
    reading += analogRead(THERMISTOR_PIN);
  }
  reading /= (float)ADC_SAMPLES;
  reading = (1023 / reading) - 1;
  return SERIES_RESISTOR / reading;
}

float kelvinToFahrenheit(float kelvin) {
  return kelvin*(9.0/5.0) - 459.67;
}

float fahrenheitToKelvin(float fahrenheit) {
  return (fahrenheit + 459.67)*(5.0/9.0);
}

float kelvinToCelsius(float kelvin) {
  return kelvin - 273.15;
}

float celsiusToKelvin(float celsius) {
  return celsius + 273.15; 
}

float readTemp() {
  float R = readResistance();
  float kelvin = 1.0/(A + B*log(R) + C*pow(log(R), 3.0));
  return kelvin;
}
