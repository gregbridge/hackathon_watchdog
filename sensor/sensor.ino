// Modified from Single-mode of VL53L0X Example

/* This example shows how to get single-shot range
 measurements from the VL53L0X. The sensor can optionally be
 configured with different ranging profiles, as described in
 the VL53L0X API user manual, to get better performance for
 a certain application. This code is based on the four
 "SingleRanging" examples in the VL53L0X API.

 The range readings are in units of mm. */

#include <Wire.h>
#include <VL53L0X.h>

#define LED 2
#define PIR 18


volatile int ppl_flag = 0;
int distance = 0;
int COUNT = 0;

VL53L0X sensor;

// Unused remnants of feature creep
//int dist[ 2 ];


// Uncomment this line to use long range mode. This
// increases the sensitivity of the sensor and extends its
// potential range, but increases the likelihood of getting
// an inaccurate reading because of reflections from objects
// other than the intended target. It works best in dark
// conditions.

//#define LONG_RANGE


// Uncomment ONE of these two lines to get
// - higher speed at the cost of lower accuracy OR
// - higher accuracy at the cost of lower speed

#define HIGH_SPEED
//#define HIGH_ACCURACY


void setup()
{
  Serial.begin(115200);
  Wire.begin();

  pinMode(LED, OUTPUT);
  pinMode(PIR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIR), PIR_ISR, RISING);
  
  
  sensor.init();
  sensor.setTimeout(500);

#if defined LONG_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  sensor.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
  // reduce timing budget to 20 ms (default is about 33 ms)
  sensor.setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
  // increase timing budget to 200 ms
  sensor.setMeasurementTimingBudget(200000);
#endif


}

void PIR_ISR() {
  digitalWrite(LED, HIGH);
  ppl_flag = 1;
}


void loop()
{
if(ppl_flag) {
  distance = sensor.readRangeSingleMillimeters();
  if(distance == 8190 || distance < 60) {
    delay(150);
    distance = sensor.readRangeSingleMillimeters();
  }
  if(distance != 8109 && distance > 55) {
    COUNT++;
    // SEND COUNT
  }
  ppl_flag = 0;
}

  
  
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  delay(150);
  Serial.println();
}
