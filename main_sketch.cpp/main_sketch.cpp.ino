#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define BUZPIN 11//speaker
#define MQ2_PIN A1
#define MQ2_dPIN 3
#define WATER_PIN A0
#define DHTPIN 8
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);

#define SPTR_SIZE   20
// #define MQ2pin (0);
int COUNT_PIN = 12; // 0 - 12
int PIN_SOUND = 2; 
char *sPtr [SPTR_SIZE];
char *strData = NULL; // this is allocated in separate and needs to be free( ) eventually
size_t numberOfStr = 0;  // number of valid elements in sPtr[  ]
//
// Get data methods
//







float get_smoke_level()
{
  auto sensorValue = analogRead(MQ2_PIN);
  return sensorValue;
}

float get_gas_level()
{
  auto sensorValue = digitalRead(MQ2_dPIN);
  return sensorValue;
}

DynamicJsonDocument all_sensors_info()
{

  DynamicJsonDocument resp(1024);
  


  sensors_event_t event;
  dht.humidity().getEvent(&event);
  auto hum = event.relative_humidity;
  dht.temperature().getEvent(&event);
  auto temp = event.temperature;
  
  
  resp["type"]   = "all_sensors";
  resp["status"] = "succes";
  auto gas = get_gas_level();
  auto smoke = get_smoke_level();
  // auto temp  = get_temp_level();
  // auto hum = get_hum_level();
  auto water = get_water_level();

  resp["gas"] = (gas);
  resp["smoke"] = (smoke);
  resp["temp"] = temp;
  resp["hum"] = hum;
  resp["water"] = (water);
  return resp;
  
  
}

void notification_event_start()
{
  if (digitalRead(MQ2_dPIN) == 0)
  {
    Serial.println("warning, gas, 1");
    
    BUZGAS();
  }

    

  
  if (analogRead(MQ2_PIN) > 200)
  {
    Serial.println("warning, co2, " + String(analogRead(MQ2_PIN)));

    BUZCO2();
  }
  
  if (analogRead(WATER_PIN) > 500)
  {
    Serial.println("warning, water, " + String(analogRead(WATER_PIN)));

    BUZWATER();
  }


  sensors_event_t event;

  dht.temperature().getEvent(&event);
  auto temp = event.temperature;
    
  if (temp > 30) {
    Serial.println("warning, temp, " + String(temp));
    BUZTEMP();
  }


  

  // if (event.temperature>41)
  // {
  //   BUZTEMP();
  // }  
}


float get_temp_level()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  auto sensorValue = event.temperature;
  return sensorValue;
}

String get_hum_level()
{
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  auto hum = event.relative_humidity;
  auto sensorValue = String(hum);
  return sensorValue;
}

float get_water_level()
{
  auto sensorValue = analogRead(WATER_PIN);
  return sensorValue;
}
//
// Utility
//
// use this free the memory after use
void freeData(char **pdata) {
  free(*pdata);
  *pdata = NULL;
  numberOfStr = 0;
}

int separate (String& str,  // pass as a reference to avoid double memory usage
              char **p,  int size, char **pdata ) {
  int  n = 0;
  free(*pdata); // clean up last data as we are reusing sPtr[ ]
  // BE CAREFUL not to free it twice
  // calling free on NULL is OK
  *pdata = strdup(str.c_str()); // allocate new memory for the result.
  if (*pdata == NULL) {
    Serial.println("OUT OF MEMORY");
    return 0;
  }
  *p++ = strtok (*pdata, ",");
  for (n = 1; NULL != (*p++ = strtok (NULL, ",")); n++) {
    if (size == n) {
      break;
    }
  }
  return n;
}
//
// JSON 
//
DynamicJsonDocument send_startup_status() {
  DynamicJsonDocument resp(1024);
  Serial.println("startup_status"); 
  return resp;

}

DynamicJsonDocument send_sensors_info() {
  DynamicJsonDocument resp(1024);
  resp["type"]   = "info_sensors";
  resp["status"] = "succes";
  resp["time"]   =  1351824120;

  Serial.print("sensorsinfo, "); 
  for (int pin = 0; pin < COUNT_PIN; pin++) {  
    auto val = digitalRead(pin);
    resp["sensors"][String(pin)] = val;
     
  }
  serializeJson(resp, Serial);
  Serial.print("\n"); 
  return resp;
}

void BUZWATER()
{
  {
    analogWrite(BUZPIN, 50);
    delay(100);
    analogWrite(BUZPIN, 0);
  }
}
void BUZTEMP()
{
  {
    analogWrite(BUZPIN, 50);
    delay(100);
    analogWrite(BUZPIN, 0);
    delay(100);
    analogWrite(BUZPIN, 50);
    delay(100);
    analogWrite(BUZPIN, 0);
  }
}
void BUZGAS()
{
  {
    analogWrite(BUZPIN, 50);
    delay(100);
    analogWrite(BUZPIN, 0);
    delay(100);
    analogWrite(BUZPIN, 50);
    delay(100);
    analogWrite(BUZPIN, 0);
    delay(100);
    analogWrite(BUZPIN, 50);
    delay(100);
    analogWrite(BUZPIN, 0);
  }
}
void BUZCO2()
{
  {
    analogWrite(BUZPIN, 50);
    delay(100);
    analogWrite(BUZPIN, 0);
    delay(100);
    analogWrite(BUZPIN, 50);
    delay(100);
    analogWrite(BUZPIN, 0);
    delay(100);
    analogWrite(BUZPIN, 50);
    delay(100);
    analogWrite(BUZPIN, 0);
    delay(100);
    analogWrite(BUZPIN, 50);
    delay(100);
    analogWrite(BUZPIN, 0);
  }
}
//
// Main 
//
void setup() {
  Serial.begin(9600);
  dht.begin();
  Serial.flush();
  pinMode(BUZPIN, OUTPUT);
  pinMode(MQ2_PIN, INPUT);
  pinMode(MQ2_dPIN, INPUT);
  pinMode(WATER_PIN, INPUT);       // назначаем наш пин "выходом"
}

void parseRecieveData(String cmd) {
	
  // Serial.println("set_sign " + cmd + "\n");

  int N = separate (cmd, sPtr, SPTR_SIZE,  &strData);
  if (strcmp(sPtr[0], "statinit") == 0) {
    send_startup_status();
  }

  if (strcmp(sPtr[0], "echo_test") == 0) {
    Serial.println("echo_test");
  }
  

  if (strcmp(sPtr[0], "all") == 0) {
    DynamicJsonDocument json = all_sensors_info();
    serializeJson(json, Serial);
    Serial.print("\n");     


  }


  
  else if (strcmp(sPtr[0], "getinfo") == 0) {
    send_sensors_info();
  }
  else if (strcmp(sPtr[0], "getlvlsmoke") == 0) {
    // DynamicJsonDocument resp(1024);
    auto val = get_smoke_level();
    // Serial.println(String(get_smoke_level()) + "\n");  
    Serial.println("smokeinfo," +  String(val));
  }

  else if (strcmp(sPtr[0], "getlvlgas") == 0) {
    // DynamicJsonDocument resp(1024);
    auto val = get_gas_level();
    // Serial.println(String(get_gas_level()) + "\n");  
    Serial.println("gasinfo," +  String(val));
  }

  else if (strcmp(sPtr[0], "getlvltemp") == 0) {
    // DynamicJsonDocument resp(1024);
    auto val = get_temp_level();
    // Serial.println(String(get_temp_level()) + "\n");  
    Serial.println("tempinfo," +  String(val));
  }

  else if (strcmp(sPtr[0], "getlvlhum") == 0) {
    // DynamicJsonDocument resp(1024);
    auto val = get_hum_level();
    // Serial.println(String(get_hum_level()) + "\n");  
    Serial.println("huminfo," +  String(val));
  }

  else if (strcmp(sPtr[0], "getlvlwater") == 0) {
    // DynamicJsonDocument resp(1024);
    auto val = get_water_level();
    // Serial.println(String(get_water_level()) + "\n");  
    Serial.println("waterinfo," +  String(val));
  }

  else if (strcmp(sPtr[0], "setsignal") == 0) {
    auto pin = atoi(sPtr [1]);
    // Serial.println (sPtr [2]);
    // Serial.println("set_sign\n");
    digitalWrite(pin, sPtr[2]); 
  } else {
    // Serial.println("echo_test\n");    
  }
  freeData(&strData);
}

void callbackNotificationWater() {
  auto lvlWater = analogRead(A0);
  if (lvlWater > 200) {
    Serial.println("warning, water, " + String(lvlWater));
  }
}







void callbackNotificationTemp() {
  sensors_event_t event;
  // dht.humidity().getEvent(&event);
  // auto hum = event.relative_humidity;
  dht.temperature().getEvent(&event);
  auto temp = event.temperature;

    
  if (temp > 44) {
  
    Serial.println("warning, temp, " + String(temp));
    
  
  }
    
}

void callbackNotificationCo2() {
  
  


    
}














void loop() {
  if (Serial.available() > 0) {
    // Serial.print(Serial.readString());   
    String str = Serial.readString();

    
    // Serial.println("get : "  + str);
    parseRecieveData(str);
  }

  notification_event_start();
  // sensors_event_t event;
  // // Serial.print("Temperature - ");
  // dht.temperature().getEvent(&event);
  delay(4000);


}