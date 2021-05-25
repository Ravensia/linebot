#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <MicroGear.h>
#include <ArduinoJson.h>
const char* ssid     = "Nozomi";
const char* password = "88888888";

const char* host = "http://uvcpostbox.herokuapp.com/bot.php"; // ตัวอย่างเช่น http://ชื่อappของคุณ.herokuapp.com/bot.php
#define APPID   "uvcpostbox" // ชื่อ APP ID ใน NETPIE
#define KEY     "j9E47rQUiayqHzL" // key ของ Device Key
#define SECRET  "0tun9PGHZYfR7Tanfu6acaRnt" // Secret ของ Device Key

#define ALIAS   "NodeMCU"  // ตั้งให้ตรงกับชื่อ อุปกรณ์ของ Device Key ใน NETPIE
#define TargetWeb "switch"

#include <Servo.h>
Servo myservo;

#define Motor_Direction1 0
#define Motor_Direction2 2
#define Motor_PWM 4
#define UpperSwitch 12
#define LowwerSwitch 13

#define UVC 5
#define UVC_LED 16

#define IR 3
#define LoadCell A0
#define ServoPin 14

bool Open = false;
bool OpenTrapDoor = false;
bool justopendoor = true;
bool justopentrapdoor = true;

int pos;
float Load1;
float Load2;

unsigned long uvctime = 5000;                          //เวลาที่เปิดหลอดไฟ UVC
unsigned long trapdoortime = 5000;
unsigned long doortime = 5000;
unsigned long time_start = 0;
unsigned long time_start_trapdoor = 0;
unsigned long time_start_door = 0;

WiFiClient client;
String uid = "";
int timer = 0;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) { //
  Serial.print("Incoming message -->");
  msg[msglen] = '\0';
  Serial.println((char *)msg);
  String msgLINE = (char *)msg;
  if ((msgLINE == "เปิดกล่อง") && (millis() - time_start == uvctime)) {
    send_json("เปิดกล่องพัสดุให้เรียบร้อยแล้ว ^^");
    Open = true;
  }
  else if ((msgLINE == "เปิดกล่อง") && (!(millis() - time_start == uvctime))) {
  send_json("กำลังฆ่าเชื้ออยู่ รออีกหน่อยนะ");
  }
  else
  {
    send_json("อ่ะ อ่าาา พิมพ์อะไรผิดอ้ะป่าววว ~");
  }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connecting to NETPIE...");
  microgear.setName(ALIAS);
}


void setup() {
  pinMode(Motor_Direction1, OUTPUT);
  pinMode(Motor_Direction2, OUTPUT);
  pinMode(Motor_PWM, OUTPUT);
  pinMode(UpperSwitch, INPUT_PULLUP);
  pinMode(LowwerSwitch, INPUT_PULLUP);
  pinMode(UVC, OUTPUT);
  pinMode(UVC_LED, OUTPUT);
  pinMode(IR, INPUT);
  myservo.attach(ServoPin);

  digitalWrite(Motor_Direction1, 0);
  digitalWrite(Motor_Direction2, 0);
  analogWrite(Motor_PWM, 0);
  myservo.write(180);

  microgear.setEEPROMOffset(3000);
  microgear.on(MESSAGE, onMsghandler);
  microgear.on(CONNECTED, onConnected);

  Serial.begin(115200);
  Serial.println("Starting...");


  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  microgear.init(KEY, SECRET, ALIAS);
  microgear.connect(APPID);
}

void send_json(String data) {
  StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["ESP"] = data;

  JsonArray& values = JSONencoder.createNestedArray("values"); //JSON array
  values.add(20); //Add value to array
  values.add(21); //Add value to array
  values.add(23); //Add value to array


  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);

  HTTPClient http;    //Declare object of class HTTPClient

  http.begin(host);      //Specify request destination
  http.addHeader("Content-Type", "application/json");  //Specify content-type header

  int httpCode = http.POST(JSONmessageBuffer);   //Send the request
  String payload = http.getString();                                        //Get the response payload

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload

  http.end();  //Close connection
}
void loop() {
  if (microgear.connected()) {
    Serial.println("Connected to NETPIE !");
    microgear.loop();
    timer = 0;
  }
  else {
    Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    }
    else timer += 100;
  }

  if ((digitalRead(IR) == 1) || (OpenTrapDoor))
  {
    if (justopentrapdoor)
    {
      digitalWrite(UVC, 0);
      digitalWrite(UVC_LED, 0);
      for (pos = 180; pos >= 30; pos -= 1)
      {
        myservo.write(pos);
        delay(15);
      }
      OpenTrapDoor = true;
      time_start_trapdoor = millis();
      Load1 = map(analogRead(LoadCell) , 0, 992, 0, 10);
      justopentrapdoor = false;
    }
    if (millis() - time_start_trapdoor >= trapdoortime)
    {
      for (pos = 30; pos <= 180; pos += 1)
      {
        myservo.write(pos);
        delay(15);
      }
      OpenTrapDoor = false;
      Load2 = map(analogRead(LoadCell) , 0, 992, 0, 10);
      if (Load2 >= Load1)
      {
        time_start = millis();
        send_json("มีพัสดุมาส่ง กำลังทำการฆ่าเชื้อ !");
      }
      justopentrapdoor = true;
    }
  }

  if ((digitalRead(UVC) == 1) && (millis() - time_start == uvctime))
  {
    digitalWrite(UVC, 0);
    digitalWrite(UVC_LED, 0);
    send_json("ทำการฆ่าเชื้อเสร็จแล้ว มารับของได้เลย +3+");
  }

  if (Open)
  {
    if (justopendoor)
    {
      digitalWrite(UVC, 0);
      digitalWrite(UVC_LED, 0);
      while (digitalRead(UpperSwitch) == 1)
      {
        digitalWrite(Motor_Direction1, 1);
        digitalWrite(Motor_Direction2, 0);
        analogWrite(Motor_PWM, 750);
        Serial.println("End round");
      }
      digitalWrite(Motor_Direction1, 0);
      digitalWrite(Motor_Direction2, 0);
      analogWrite(Motor_PWM, 0);
      justopendoor = false;
    }

    time_start_door = millis();

    if (millis() - time_start_door >= doortime)
    {
      while (digitalRead(LowwerSwitch) == 1)
      {
        digitalWrite(Motor_Direction1, 0);
        digitalWrite(Motor_Direction2, 1);
        analogWrite(Motor_PWM, 750);
        Serial.println("End round");
      }
      digitalWrite(Motor_Direction1, 0);
      digitalWrite(Motor_Direction2, 0);
      analogWrite(Motor_PWM, 0);
    }
    Open = false;
    justopendoor = true;
  }
}
