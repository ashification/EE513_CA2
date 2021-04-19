// Based on the Paho C code example from www.eclipse.org/paho/
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "ADXL345.h"
#include "MQTTClient.h"
#include "ledClass.h"
  
#define CPU_TEMP     "/sys/class/thermal/thermal_zone0/temp"
#define UPTIME       "/proc/uptime"
#define GPIO         "/sys/class/gpio/"
#define FLASH_DELAY  500000 // 500 milliseconds


using namespace std;
using namespace exploringRPi;

//Please replace the following address with the address of your server
#define ADDRESS    "tcp://192.168.1.32:1883"
#define CLIENTID   "Publisher_rpi"
#define AUTHMETHOD "alee"
#define AUTHTOKEN  "rpi"
#define TOPIC1      "ee513/CPUTemp"
#define TOPIC2      "ee513/RPIPitchAndRoll"
#define QOS        2
#define TIMEOUT    10000L

float getCPUTemperature() {        // get the CPU temperature
   int cpuTemp;                    // store as an int
   fstream fs;
   fs.open(CPU_TEMP, fstream::in); // read from the file
   fs >> cpuTemp;
   fs.close();
   return (((float)cpuTemp)/1000);
}

float getUptime() {        // get uptime
   float uptime;                    // store as an string
   fstream fs;
   fs.open(UPTIME, fstream::in); // read from the file
   fs >> uptime;
   fs.close();
   return uptime;
}

float getRpiPitch() {
   ADXL345 sensor(1,0x53);
   sensor.setResolution(ADXL345::NORMAL);
   sensor.setRange(ADXL345::PLUSMINUS_4_G);
   //sensor.displayPitchAndRoll();
   sensor.displayPitch();
   return sensor.displayPitch();
}

float getRpiRoll() {
   ADXL345 sensor(1,0x53);
   sensor.setResolution(ADXL345::NORMAL);
   sensor.setRange(ADXL345::PLUSMINUS_4_G);
   sensor.displayRoll();
   return sensor.displayRoll();

}



int main(int argc, char* argv[]) {
   char str_payload1[100];          // Set your max message size here
   char str_payload2[100];          // Set your max message size here
   //printf("Uptime in Seconds: ", getUptime());

   LED blueled(26);          // create four LED objects

   
   MQTTClient client;
   MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
   MQTTClient_message pubmsg = MQTTClient_message_initializer;
   MQTTClient_deliveryToken token;
   MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
   
   opts.keepAliveInterval = 20;
   opts.cleansession = 1;
   opts.username = AUTHMETHOD;
   opts.password = AUTHTOKEN;
   
   int rc;
   
   if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
      cout << "Failed to connect, return code " << rc << endl;
      return -1;
   }


   for(int i=0; i<20; i++){          // LEDs will alternate
     blueled.turnOn();             // turn GPIO off
     
     // Publish the CPU Temp Msg
     sprintf(str_payload1, "{\"d\":{\"CPUTemp\": %f }}", getCPUTemperature());   
     pubmsg.payload = str_payload1;
     pubmsg.payloadlen = strlen(str_payload1);
     pubmsg.qos = QOS;
     pubmsg.retained = 0;
     MQTTClient_publishMessage(client, TOPIC1, &pubmsg, &token);
     cout << "Waiting for up to " << (int)(TIMEOUT/1000) <<
          " seconds for publication of " << str_payload1 <<
          " \non topic " << TOPIC1 << " for ClientID: " << CLIENTID << endl;
     rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
     cout << "Message with token " << (int)token << " delivered." << endl;

     // Publish the pitch and roll msg

     sprintf(str_payload2, "{\"d\":{\"RPI Pitch\": %f :\"and Roll\": %f}}", getRpiPitch(),getRpiRoll());   
     pubmsg.payload = str_payload2;
     pubmsg.payloadlen = strlen(str_payload2);
     pubmsg.qos = QOS;
     pubmsg.retained = 0;
   
     MQTTClient_publishMessage(client, TOPIC2, &pubmsg, &token);
     cout << "Waiting for up to " << (int)(TIMEOUT/1000) <<
          " seconds for publication of " << str_payload2 <<
          " \non topic " << TOPIC2 << " for ClientID: " << CLIENTID << endl;
     rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
     cout << "Message with token " << (int)token << " delivered." << endl;

     blueled.turnOff();             // turn GPIO off
     usleep(FLASH_DELAY);           // sleep for 50ms

    }

   MQTTClient_disconnect(client, 10000);
   MQTTClient_destroy(&client);
   return rc;
}
