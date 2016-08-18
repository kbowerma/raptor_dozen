/* 12.31.2015 Kyle Bowerman
    + added header file
   2.29.2016
    + added event call and version
   3.1.2016
    + cloned from scraper-relay-test
    + added http client
  8.1.2016
      + ported to local command line build
      + Add ip, version publish at startup
      + upgraded local firmware to v5.2 (git checkout  release/v0.5.2)
      + renamed from raptor-garden
      + Add DHT temp and humidty reading
  8.16.2016 2.1d switch to dht22


*/

#include "raptor_dozen.h"
   //#include "HttpClient/HttpClient.h"  //for web ide
  #include "lib/HttpClient/firmware/HttpClient.h"  // for local build
  #include "lib/Adafruit_DHT_Library/firmware/Adafruit_DHT.h"

    // DHT parameters
  #define DHTPIN 0
  #define DHTTYPE DHT22

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
  // DHT sensor
  DHT dht(DHTPIN, DHTTYPE);
  //Timer timer(10000, publishTempandHumidity);



void setup() {

  // Start DHT sensor
  dht.begin();
  //timer.start();

  pinMode(DIN1, OUTPUT);
  pinMode(DIN2, OUTPUT);
  pinMode(DIN3, OUTPUT);
  pinMode(DIN4, OUTPUT);
  pinMode(DIN5, OUTPUT);
  pinMode(DIN6, OUTPUT);
  pinMode(DIN7, OUTPUT);

  Serial.begin(9600);
  IPAddress myIP = WiFi.localIP();
  String ipStr = String(myIP[0])+"."+String(myIP[1])+"."+String(myIP[2])+"."+String(myIP[3]);


  Particle.publish("LocalIP", ipStr, 60,PRIVATE);

  String myVersion = System.version().c_str();
  Particle.publish("Version", myVersion, 60,PRIVATE);


 Particle.function("relayon", relayOn) ;
 Particle.function("relayoff", relayOff);
 Particle.function("pinState", pinState);
 Particle.variable("file",FILENAME, STRING);
 Particle.variable("version",VERSION, STRING);
 Particle.variable("temperature", temperatureString );
 Particle.variable("humidity", humidityString );
 Particle.variable("ip", ipStr );
 Particle.variable("firmware", myVersion);


 // http://sailpipe-dev.herokuapp.com/device/create?name=photon&data=12.3
  request.hostname = "sailpipe-dev.herokuapp.com";
  request.port = 80;
 // request.path = "/device/create"
 // request.path = "/device/create?name=photon&data=12.3&desc=fromDeviceInloop&type=data";
  // http.get(request, response, headers);

}

void loop() {
    request.path = "/device/create";  // I have to put this in the loop becuase the path conatins the data


// handmad 5 minute interval timer
if ( millis() % 300000 == 0 ) {
  publishTempandHumidity();
}

}

void publishTempandHumidity() {
  // Temperature measurement
  temperature = dht.getTempFarenheit();
  temperatureString = String(temperature);
  request.path = String("/device/create?type=sensor&desc=temperature&name=raptor&data=" + temperatureString );
  http.get(request, response, headers);

   delay(4000);
  // Humidity measurement
  humidity = dht.getHumidity();
  humidityString = String(humidity);
  request.path = String("/device/create?type=sensor&desc=humidity&name=raptor&data=" + humidityString );
  http.get(request, response, headers);
  Particle.publish("temperature", String(temperature) + " °F");
  Particle.publish("humidity", String(humidity) + "%");


}

int pinState(String command){
    return digitalRead(command.toInt());

}

int relayOn(String command) {
    Particle.publish("relay ON pin", command);   //publish even to particle cloud
    request.path = String("/device/create?type=event&desc=relay%20ON&name=raptor&data=" + command );
     http.get(request, response, headers);
      // Particle.publish("DEBUG",  request.path);
      // Particle.publish("mresponse",  response.body); //DEBUG


    if(command != "all") {
        digitalWrite(command.toInt(), HIGH);
        return 1;
    }
    if(command == "all") {
        for (int i = 0; i < 8; i++ ) {
            digitalWrite(i, HIGH);
        }
        return 1;
    }
    else return -1;
}

int relayOff(String command) {
    Particle.publish("relay off pin", command);
    request.path = String("/device/create?type=event&desc=relay%20Off&name=raptor&data=" + command );
     http.get(request, response, headers);


    if(command != "all") {
        digitalWrite(command.toInt(), LOW);
        return 1;
    }
    if(command == "all") {
        for (int i = 0; i < 8; i++ ) {
            digitalWrite(i, LOW);
        }
        return 1;

    }
    else return -1;
}
