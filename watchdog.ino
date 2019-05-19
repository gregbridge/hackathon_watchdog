#include <AWS_IOT.h>
#include <WiFi.h>

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



void setup() {
    pinMode(inPin, INPUT);
    pinMode(watchdogPin, OUTPUT);
    Serial.begin(115200);
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

}

void loop() {

    if(msgReceived == 1)
    {
        msgReceived = 0;
        Serial.print("Received Message:");
        Serial.println(rcvdPayload);
    
        digitalWrite(inPin, HIGH);
        delay(1000);
        digitalWrite(inPin, LOW);

        digitalWrite(watchdogPin, HIGH);
        delay(1000);
        digitalWrite(watchdogPin, LOW);
    }

    val = digitalRead(inPin);
   //if (val != 0)
    //Serial.println("Yes");
    //else
    //Serial.println("No");
    
    if(tick >= 10)   // publish to topic every 5seconds
    {
        tick=0;
        sprintf(payload,"{ \"state\": { \"desired\": {\"activate\": UPDATE SHADOW}}}");
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
}
