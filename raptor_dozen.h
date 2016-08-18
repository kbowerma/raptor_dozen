#include "lib/HttpClient/firmware/HttpClient.h"  // for local build
//#include "lib/HttpClient/firmware/HttpClient.h"  // for local build

//Globals
#define FILENAME "raptor_dozen"
#define VERSION "2.3"
// DHT parameters
  #define DHTPIN 0
  #define DHTTYPE DHT22

//declarations
  int DIN1 = D1;
  int DIN2 = D2;
  int DIN3 = D3;
  int DIN4 = D4;
  int DIN5 = D5;
  int DIN6 = D6;
  int DIN7 = D7;
  int temperature;
  String temperatureString;
  int humidity;
  String humidityString;
  int relayOn(String command);
  int relayOff(String command);
  int pinState(String command);


//Prototypes
void publishTempandHumidity();

HttpClient http;
  // Headers currently need to be set at init, useful for API keys etc.
  http_header_t headers[] = {
      //  { "Content-Type", "application/json" },
      //  { "Accept" , "application/json" },
      { "Accept" , "*/*"},
      { NULL, NULL } // NOTE: Always terminate headers will NULL
  };

  http_request_t request;
  http_response_t response;
