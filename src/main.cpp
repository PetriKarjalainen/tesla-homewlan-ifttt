#include <arduino.h>
#include "./esppl_functions.h"
#include <ESP8266WiFi.h>
#include "DataToMaker.h"
#include "config.h"
#define SERIAL_DEBUG // Uncomment this to dissable serial debugging

/*


This code monitors MAC addresses found with ESP8266 on wemos d1 board
and then send the status to IFTTT MAKER - if there is a change.

Needs IFTTT to work, create IFTTT webhook events:
<devicename>_<deviceState>
for each of your events and states to connect the MAC status events to IFTTT services

to use, copy config.org to config.h and update with your own data

Thanks for the idea to:
  https://github.com/RicardoOliveira/FriendDetector
  https://github.com/mylob/ESP-To-IFTTT


Change  history:
  19.1.2020 - v 1.0 PetriKarj

*/

#define COUNTDOWN 1200*7 // seconds * 7 = /150ms rounded up to reliably detect non active device
#define INITIALIZE 60*7 // seconds * 7 to initialize to reliably detect nearby active devices
int maccntdwn[NUMBER_OF_DEVICES];
bool mactomonitor[NUMBER_OF_DEVICES];
DataToMaker event(myKey, "ESP");
bool pvsValues[NUMBER_OF_DEVICES];
bool connectedToWiFI = false;
bool firstrun;

String boolstring( _Bool b ) { return b ? "true" : "false"; }

void debug(String message)
{
#ifdef SERIAL_DEBUG
  Serial.print(message);
#endif
}

void debugln(String message)
{
#ifdef SERIAL_DEBUG
  Serial.println(message);
#endif
}

void debugln()
{
#ifdef SERIAL_DEBUG
  Serial.println();
#endif
}


bool maccmp(uint8_t *mac1, uint8_t *mac2)
{
  for (int i=0; i < ESPPL_MAC_LEN; i++)
  {
    if (mac1[i] != mac2[i])
    {
      return false;
    }
  }
  return true;
}

// capture WLAN fames
void cb(esppl_frame_info *info)
{
  for (int i=0; i<NUMBER_OF_DEVICES; i++)
  {
    if (maccmp(info->sourceaddr, devicemac[i]) || maccmp(info->receiveraddr, devicemac[i]))
    {
      mactomonitor[i] = true;
      maccntdwn[i] = COUNTDOWN;
    }
  }
}

// process WLAN frames
void monitormac(){
  for (int i = ESPPL_CHANNEL_MIN; i <= ESPPL_CHANNEL_MAX; i++ )
    {
      esppl_set_channel(i);
      while (esppl_process_frames())
        {
        }
    }
  // countdown timer after which period a device is declared not to found in WLAN
  for (int i = 0 ; i < NUMBER_OF_DEVICES ; i++)
    {
        if (maccntdwn[i]>0)
          {
            mactomonitor[i]=true;
            maccntdwn[i]--;
          }
          else
            {
              mactomonitor[i] = false;
            }
      }

}

bool ConnectWifi()
{
  // Connect to WiFi network
  debug(ssid);
  unsigned long startTime = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && startTime + 30 * 1000 >= millis())
  {
    delay(500);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    debug(" connected ");
    return true;
  }
  else
  {
    WiFi.disconnect();
    debugln(" timed Out!!!");
    return false;
  }
}

bool wifiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

int decodedeviceStates(bool val)
{
  switch (val)
  {
    case false:
      return(1);
      break;
    case true:
      return(0);
      break;
  }
}


bool DetectChange()
{
  bool val;
  for (int i = 0 ; i < NUMBER_OF_DEVICES ; i++)
  {
    if ((val = mactomonitor[i]) != pvsValues[i])
    {
      pvsValues[i] = val;
      //PetriKarj simplify IFTTT recipe writing to allow devicename_event as event name
      event.setValue(String(devicename[i])+"_"+String(deviceStates[decodedeviceStates(val)]));
      debug("Changes found: (");
      debug(String(maccntdwn[i]));
      debug(") ");
      debug( (String(devicename[i]) + "_" + String(deviceStates[decodedeviceStates(val)])) );
      debug(" ---> ");
      return true;
    }
  }
  return false;
}

void send_to_ifttt() {
  while (DetectChange()){
    esppl_sniffing_stop();
    ConnectWifi();
    while (!wifiConnected())
    {
      for (int i = 0 ; i < (600) ; i++) // delay before trying to reconnect
        {
          delay(500); // 0.5s heartbeat interval for connection delay
          digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
      ConnectWifi();
    }
    if (wifiConnected())
    {
      int j=0;
      debug("IFTTT");
      while ((!event.connect()) && (j<10)) {j++;delay(5000);digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));}
      if (j<10)
      {
        j=0;
        debug(" connected ");
        while (!event.post() && j<10) {j++;delay(5000);digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));};
        if (j<10) debug(" --- Succesfully posted --- ");
      }
      else debug(" Failed To Connect!!!");
    }
      WiFi.disconnect();
      while (wifiConnected())
      {
        for (int i = 0 ; i < (600) ; i++) // delay before trying to reconnect
          {
            delay(500); // 0.5s heartbeat interval for connection delay
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
          }
          WiFi.disconnect();
      }
      debugln(" disconnected and restarting sniffing");
  }
  esppl_sniffing_start();
}

void setup() {
  #ifdef SERIAL_DEBUG
    Serial.begin(57600);
    delay(500);
    Serial.println();
  #endif

  pinMode(LED_BUILTIN, OUTPUT);

  esppl_init(cb);
  esppl_sniffing_start();


  debugln("Setup done");
}

void loop()
{

  // first detection to be written immediately
  for (int i = 0 ; i < NUMBER_OF_DEVICES ; i++) {maccntdwn[i] = INITIALIZE;}
  for (int i = 0 ; i < INITIALIZE ; i++) {monitormac(); digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));delay(150);}
  esppl_sniffing_stop();
  for (int i = 0 ; i < NUMBER_OF_DEVICES ; i++)
  {
      if (maccntdwn[i]==0) {pvsValues[i] = true;mactomonitor[i]=false;}
      else {pvsValues[i] = false;mactomonitor[i]=true;}
  }
  send_to_ifttt();
  esppl_sniffing_start();

  // This loop runs roughly at 150ms interval when not sending to ifttt
  while (true)
    {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      monitormac();
      send_to_ifttt();
    }
}
