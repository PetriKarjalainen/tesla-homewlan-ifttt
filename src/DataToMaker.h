#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>  //ESP8266 Core WiFi Library
#else
#include <WiFi.h>      //ESP32 Core WiFi Library
#endif
#ifndef DataToMaker_h
class DataToMaker
{
  public:
    DataToMaker(const char*, String); // constructor
    bool connect();
    bool setValue(String);
    void sendToMaker();
    bool post();


  protected: // it is protected because the subclass needs access
    //to max distance!

  private:
    void compileData();
    WiFiClient client;
    const char* privateKey;
    String event;
    String value1, value2, value3 = "";
    bool dataAvailable;
    String postData;
};

DataToMaker::DataToMaker(const char* _privateKey, String _event)
{
  privateKey = _privateKey;
  event = _event;
}

bool DataToMaker::connect()
{
if (client.connect("maker.ifttt.com", 80))
  return true;
  else return false;
}

bool DataToMaker::post()
{
  compileData();
    // Redone DataToMaker by petrikarj to give response if success
    String data = "/trigger/" + event + "/with/key/" + privateKey;

    client.println("POST "+data+" HTTP/1.1");
    client.println("Host: maker.ifttt.com");
    client.println("Content-Type: application/json");
    client.println("Content-Length: 0");
    client.println("");

    // wait for data to be available
    unsigned long timeout = millis();
    while ((client.available() == 0)&&((millis() - timeout < 5000))) {
      if (millis() - timeout < 5000)
      {
        delay(100);
      }
      else
      {
        Serial.println(">>> posting timeout !");
      }
    }

    // Read all the lines of the reply from server and print them to Serial
    // not testing 'client.connected()' since we do not need to send data here
    data="";
    char ch;
    while (client.available()) {
      ch = static_cast<char>(client.read());
      data = data+ ch;
    }

    while (client.connected()) {
        client.stop();  // DISCONNECT FROM THE SERVER
        client.flush();
        delay(3000);
    }

    if (data.substring(9,9+6) == "200 OK")
      return true;
    else {
      Serial.println(data);
      return false;
    }
}

bool DataToMaker::setValue(String value)
{
  event = value;
  return true;
}

void DataToMaker::compileData()
{
}
#endif
