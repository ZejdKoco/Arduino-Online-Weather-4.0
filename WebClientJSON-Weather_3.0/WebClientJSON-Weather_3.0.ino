
#include "WiFiEsp.h"
// Emulate Serial1 on pins 6/7 if not present
#include "SoftwareSerial.h"
#include <Servo.h>

SoftwareSerial Serial1(2, 3); // RX, TX
Servo myservo;
Servo servoProg;
float stepNegativ = 1.5;    // variable to store the servo position
bool debug = false;
float stepPoz = 4.3;
float temp = 0;
int zero = 18;
const char* ssid = "Rezidencija Skenderija 2";            // your network SSID (name)
const char* pass = "ajhzeddk";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
String prognoza="def";
String deksripcija="def";
String temperatura="def";
String vlaznost="def";
String bitmapa = "";
const int pozicijeProg[]={16, 32, 49, 69, 89, 111, 129, 151, 171};
const int pozicijeTemp[]={20, 24, 28, 31, 35, 39, 43, 47, 51, 56, 61, 65, 70, 74, 78, 83, 88, 93, 97, 102, 106, 111, 115, 119, 124, 127, 132, 136, 140, 144, 147, 151, 155, 159, 164, 169};

// Initialize the Ethernet client object
WiFiEspClient client;

void startConnection(){
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    if(debug) Serial.println(("WiFi shield not present"));
    // don't continue
    while (true);
  }
  // attempt to connect to WiFi network
  while (WiFi.status() != WL_CONNECTED) {
    //Serial.print(("Attempting to connect to WPA SSID: "));
    //Serial.println(ssid);
    // Connect to WPA/WPA2 network
    //Serial.println("Connecting to wifi");
    status = WiFi.begin(ssid, pass);
  }
  // you're connected now, so print out the data
  //Serial.println("You're connected to the network");
  //printWifiStatus();
}

void makeHTTPrequest(){
    // if you get a connection, report back via serial
    if (client.connect("api.openweathermap.org", 80)) {
    // Make a HTTP request
    client.print(("GET /data/2.5/weather?q=Sarajevo&appid=31216b94e440cef83fbcc0d47e60d0e0&units=metric HTTP/1.1\r\n"));
    client.print(("Host: api.openweathermap.org\r\n"));
    client.print(("Connection: close\r\n\r\n"));
    if(debug) Serial.println("HTTP request sent");
  }
  else{
    if (debug) Serial.println("Couldnt send HTTP request");
    /*for(int i=0; i<10; i++){
      //Serial.println("error");
      digitalWrite(7, LOW);
      delay(1000);
      digitalWrite(7, HIGH);
      delay(1000);
    }*/
    return;
  }
}
void getStringHTTP(){
  char endOfHeaders[] = "\r\n\r\n";
  client.find(endOfHeaders);
  
  if(!client.available()){
    if(debug) Serial.println(("No HTTP Response"));
    /*for(int i=0; i<10; i++){
      digitalWrite(7, LOW);
      delay(250);
      digitalWrite(7, HIGH);
      delay(1000);
    }*/
    return;
  }
 int vrijeme = millis();
 String httpString = ""; 
  while (client.connected()) { 
   if(abs(vrijeme - millis()) > 5000){
    return;
   }
   httpString = client.readStringUntil('\n');
 } 
 int pozicija;
  String Sdeskripcija;
  String Stemperatura;
  pozicija = httpString.indexOf("\"main");
  while(httpString[pozicija] != ','){
    Sdeskripcija+=httpString[pozicija];
    pozicija++;
  }
  pozicija = httpString.indexOf("\"temp");
  while(httpString[pozicija] != ','){
    Stemperatura+=httpString[pozicija];
    pozicija++;
  }
  Stemperatura+="°C";
  String trenutnaVlaznost;
  pozicija = httpString.indexOf("\"humidity");
  while(httpString[pozicija] != ','){
    trenutnaVlaznost+=httpString[pozicija];
    pozicija++;
  }
  trenutnaVlaznost+="%";
  String moreInfo;
  pozicija = httpString.indexOf("\"description");
  while(httpString[pozicija] != ','){
    moreInfo+=httpString[pozicija];
    pozicija++;
  }
  String bitmapa2;
  pozicija = httpString.indexOf("\"icon");
  pozicija+=8;
  while(httpString[pozicija] != '"'){
    bitmapa2+=httpString[pozicija];
    pozicija++;
  }
  bitmapa = bitmapa2;
  prognoza = Sdeskripcija;
  deksripcija = moreInfo;
  temperatura = Stemperatura;
  vlaznost = trenutnaVlaznost;

  String privremeni = "";
  for(int i=0; i<temperatura.length(); i++){
    if(temperatura[i] == '.' || temperatura[i] == '0' ||  temperatura[i] == '1' || temperatura[i] == '2' || temperatura[i] == '3' || temperatura[i] == '4' || temperatura[i] == '5'|| temperatura[i] == '6'|| temperatura[i] == '7'|| temperatura[i] == '8'|| temperatura[i] == '9'){
      privremeni += temperatura[i];
    }
  }
  temp = privremeni.toFloat();
  if(debug) Serial.println(round(temp));
  if(debug) Serial.println(bitmapa);
  client.flush();
  //client.stop();
  
}
void sweepServo(){
  myservo.write(20);
  delay(1000);
  for(int i=0; i<=35; i++){
    myservo.write(pozicijeTemp[i]);
    delay(900);
  }
  delay(500);
  for(int i=35; i>=0; i--){
    myservo.write(pozicijeTemp[i]);
    delay(900);
  }
  delay(100);
}
void pomjeriPrognozu(){
  //servoProg.attach(11);
  if(bitmapa == "11d" || bitmapa == "11n") servoProg.write(pozicijeProg[0]);
  else if(bitmapa == "09d" || bitmapa == "09n") servoProg.write(pozicijeProg[1]);
  else if(bitmapa == "10d" || bitmapa == "10n") servoProg.write(pozicijeProg[2]);
  else if(bitmapa == "01d" || bitmapa == "01n") servoProg.write(pozicijeProg[3]);
  else if(bitmapa == "02d" || bitmapa == "02n") servoProg.write(pozicijeProg[4]);
  else if(bitmapa == "03d" || bitmapa == "03n") servoProg.write(pozicijeProg[5]);
  else if(bitmapa == "04d" || bitmapa == "04n") servoProg.write(pozicijeProg[6]);
  else if(bitmapa == "13d" || bitmapa == "13n") servoProg.write(pozicijeProg[7]);
  else if(bitmapa == "50d" || bitmapa == "50n") servoProg.write(pozicijeProg[8]);
  delay(1000);
  //servoProg.detach();
}
void pomjeriTemperaturu(){
  //myservo.attach(9);
  if(temp >= 0)
  myservo.write(pozicijeTemp[(int)round(temp)]);
  else{
    myservo.write(round(zero+temp*stepNegativ));
  }
  delay(1000);
  //myservo.detach();
}

void setup()
{
  // initialize serial for debugging
  Serial.begin(9600);
  startConnection();
  pinMode(13,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(0,OUTPUT);
  digitalWrite(13, HIGH);
  digitalWrite(7, LOW);
  digitalWrite(0, LOW);
  //startConnection();
  servoProg.attach(11);
  myservo.attach(9);
  //sweepServo();
  myservo.write(pozicijeTemp[0]);
  servoProg.write(pozicijeProg[0]);
  delay(1000);
  myservo.detach();
  servoProg.detach();
  
}
void printSerial(){
  Serial.println(prognoza);
  Serial.println(deksripcija);
  Serial.println(temperatura);
  Serial.println(vlaznost);
}
void loop()
{
  pocetak:
  myservo.detach();
  servoProg.detach();
  digitalWrite(7, LOW);
  makeHTTPrequest();
  getStringHTTP();
  while(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, pass);
    delay(5000);
  }
  servoProg.attach(11);
  myservo.attach(9);
  pomjeriPrognozu();
  pomjeriTemperaturu();
  //printSerial();
  digitalWrite(7, HIGH);
  delay(60UL * 1000UL * 5UL);
  //delay(20000);
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print(("SSID: "));
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print(("IP Address: "));
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print(("Signal strength (RSSI):"));
  Serial.print(rssi);
  Serial.println((" dBm"));
}
