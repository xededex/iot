#include <ArduinoJson.h>

// пищалка - 11 цифра
// температура - 8 пин
// газ - 3 пин
// аналаговые
// co2 - A1
// A0 - влажность


// #define MQ2pin (0);
int COUNT_PIN = 12; // 0 - 12
#define SPTR_SIZE   20
char   *sPtr [SPTR_SIZE];
char *strData = NULL; // this is allocated in separate and needs to be free( ) eventually
size_t numberOfStr = 0;  // number of valid elements in sPtr[  ]







float get_smoke_level()
{
  
  int   MQ2pin  = A1;

  auto sensorValue = analogRead(MQ2pin); // прочитать аналоговый вход 0
  // auto value = digitalRead(2);
  
  // Serial.print("Sensor Value: ");
  // Serial.print(sensorValue);
  // Serial.print(value);
  


  return sensorValue;


  
  // if(sensorValue > 300)
  // {
  //   Serial.print(" | Smoke detected!");
  // }
  
  // Serial.println("");
  // delay(2000); // подождать 2 сек до следующего чтения
}






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
DynamicJsonDocument send_startup_status() {
  DynamicJsonDocument resp(1024);
  Serial.println("startup_status"); 
  delay(500);  
  // resp["type"]   = "info_init";
  // resp["status"] = "succes";
  // serializeJson(resp, Serial);
  // Serial.print("\n"); 
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


void setup() {
  Serial.begin(115200);
  Serial.flush();
  pinMode(10, OUTPUT);        // назначаем наш пин "выходом"
  

}

void parseRecieveData(String cmd) {
  // Serial.println("set_sign " + cmd + "\n");
  int N = separate (cmd, sPtr, SPTR_SIZE,  &strData);
  if (strcmp(sPtr[0], "statinit") == 0) {
    send_startup_status();
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

  
  else if (strcmp(sPtr[0], "setsignal") == 0) {
    auto pin = atoi(sPtr [1]);
    Serial.println (sPtr [2]);
    // digitalWrite(10, HIGH);
    // sPtr [2]
    Serial.println("set_sign\n");
    digitalWrite(pin, sPtr [2]); 

      // delay(1000);
      // digitalWrite(10, LOW); 

      // // digitalWrite(atoi(sPtr [1]), sPtr [2]); 
      // delay(1000);
  } else {
    Serial.println("err, parse_cmd_error,Команда не распознана");    
  }


  freeData(&strData);
  // for (int n = 0; n < N; n++) {
    
  //   Serial.println (sPtr [n]);
  // }


  
 


}


int PIN_SOUND = 2; 




void loop() {
  
    // Serial.println("start loop iter");
    // Serial.print("sensorsinfo, ");    
    // parseRecieveData("test1, test2, test3\n");
    // parseRecieveData("test14,test12,test3\n");
    // auto lvl_smoke = get_smoke_level();
    // if (lvl_smoke > 300.0) {
    //   Serial.println(String("warning, criticalsmokelvl") + String(",") + String(lvl_smoke) + String("\n"));
    //   // tone(PIN_SOUND, 100, 500);

    // }
    
    // delay(2000); 

    // Serial.print("sensorsinfo, ");  
    if (Serial.available() > 0) {
        // Serial.print(Serial.readString());  
        
        String str = Serial.readString();
        parseRecieveData(str);
        // Serial Pa(str);
    }

    

    // auto data = get_cond_sensors();
    // serializeJson(data, Serial);
    // Serial.print("\n");    

    


      // ждем еще 1 секунду
}