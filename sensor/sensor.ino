// Modified from Single-mode of VL53L0X Example but made by GREG and LUCAS

/* This example shows how to get single-shot range
 measurements from the VL53L0X. The sensor can optionally be
 configured with different ranging profiles, as described in
 the VL53L0X API user manual, to get better performance for
 a certain application. This code is based on the four
 "SingleRanging" examples in the VL53L0X API.

 The range readings are in units of mm. */

#include <Wire.h>
#include <VL53L0X.h>
#include <AWS_IOT.h>
#include <WiFi.h>

#define LED 2
#define PIR 15
#define BIGLED 18


volatile int ppl_flag = 0;
int distance = 0;
int COUNT = 0;

VL53L0X sensor;
void PIR_ISR(void);

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

// LUCAS 
AWS_IOT hornbill;

char WIFI_SSID[]="hackathon2019";
char WIFI_PASSWORD[]="fearlesscoder";
char HOST_ADDRESS[]="a25cwbvvrpbez9-ats.iot.us-west-2.amazonaws.com";
char CLIENT_ID[]= "client id";
char TOPIC_NAME[]= "$aws/things/watchdog/shadow/update";
int watchdogPin = 2;
int inPin = 18;
int val = 0;

 

int status = WL_IDLE_STATUS;

int tick=0,msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];
 

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)

{
    strncpy(rcvdPayload,payLoad,payloadLen);
    rcvdPayload[payloadLen] = 0;
    msgReceived = 1;
}


void setup()
{
  Serial.begin(115200);
  Wire.begin();

  pinMode(LED, OUTPUT);
  pinMode(BIGLED, OUTPUT);
  pinMode(PIR, INPUT);
  //attachInterrupt(digitalPinToInterrupt(PIR), PIR_ISR, RISING);
  
  
  sensor.init();
  sensor.setTimeout(500);

delay(2000);
    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(WIFI_SSID);

        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        // wait 5 seconds for connection:
        delay(5000);
    }
    Serial.println("Connected to wifi");
    if(hornbill.connect(HOST_ADDRESS,CLIENT_ID)== 0)
    {
        Serial.println("Connected to AWS");
        delay(1000);
 
       if(0==hornbill.subscribe(TOPIC_NAME,mySubCallBackHandler))
        {
            Serial.println("Subscribe Successfull");
        }
        else
        {
            Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
            while(1);
        }
    }
    else
    {
        Serial.println("AWS connection failed, Check the HOST Address");
        while(1);
    }
    delay(2000);


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

Serial.println("woot");
}

/*void PIR_ISR() {
  digitalWrite(LED, HIGH);
  ppl_flag = 1;
}*/


void loop()
{
digitalWrite(BIGLED, HIGH);
delay(500);
digitalWrite(BIGLED, LOW);
delay(250);






  
if(digitalRead(PIR) == HIGH) {
  distance = sensor.readRangeSingleMillimeters();
  if(distance == 8190 || distance < 60) {
    delay(150);
    distance = sensor.readRangeSingleMillimeters();
  }
  if(distance != 8109 && distance > 55) {
    COUNT++;
    
    Serial.println("PERSON!");
    Serial.println();
    ppl_flag = 0;
  }
  delay(500);
  digitalWrite(LED, LOW);
  ppl_flag = 0;

}

 if(msgReceived == 1)
    {
        msgReceived = 0;
        Serial.print("Received Message:");
        Serial.println(rcvdPayload);
 
        digitalWrite(BIGLED, HIGH);
        delay(1000);
        digitalWrite(BIGLED, LOW);
    }

     if(tick >= 10)   // publish to topic every 5seconds
    {
        tick=0;
        sprintf(payload,"{ \"state\": { \"desired\": {\"activate\":55}}}");
        if(hornbill.publish(TOPIC_NAME,payload) == 0)
        {       
            Serial.print("Publish Message:");
            Serial.println(payload);
        }
        else
        {
            Serial.println("Publish failed");
        }
    } 
    vTaskDelay(1000 / portTICK_RATE_MS);
    tick++;
 
  if(sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
}
