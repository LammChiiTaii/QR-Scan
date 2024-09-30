#include <FontMaker.h>
#include <IOT47_UTF8.h>
#include <MyFontMaker.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c  //initialize with the I2C addr 0x3C Typically eBay OLED's
#define RX_PIN D5
#define TX_PIN D6

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     //   QT-PY / XIAO

#define SSID "American Study HD"
#define PASS "66668888"

Adafruit_SH1106G display1 = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SoftwareSerial mySerial(RX_PIN, TX_PIN);

void setpx(int16_t x, int16_t y, uint16_t color) {
  display1.drawPixel(x, y, color);
}
MakeFont display(&setpx);
String filterId = "";
boolean isIncreased = 0;

void setup() {
  pinMode(D7, INPUT);
  Serial.begin(9600);
  Serial.println("Initializing WiFi...");
  WiFi.mode(WIFI_STA);
  Serial.println("Setup done!");
  WiFi.begin(SSID, PASS);

  Serial.print("Connecting to WIFI ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("WIFI connected. LocalIP :");
  Serial.println(WiFi.localIP());

  mySerial.begin(9600);
  if (!display1.begin(i2c_Address, true)) {
    Serial.println("SH1106 allocation failed!");
    for (;;)
      ;
  }
  delay(2000);
  display.set_font(TCVN10Height2Width);
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop() {
  if (mySerial.available()) {
    isIncreased = 1;
    String data = mySerial.readString();
    String filter = getValue(data, ',', 0) + " ";
    filterId = getValue(filter, ' ', 1);
    Serial.println("FilterId is: " + filterId);
    getFilterInfo(filterId, 2);
    delay(1000);
  }

  if (digitalRead(D7) == 1 && isIncreased == 1){
    isIncreased = 0;
    display1.clearDisplay();
    display.print(4, 16, "INCREASED USED NUMBER", SH110X_WHITE, SH110X_BLACK);
    display.print(28, 32, "OF THIS FILTER", SH110X_WHITE, SH110X_BLACK);
    display1.display();
    delay(100);
    increaseFilterUsed(filterId, 0);
  }
}

void getFilterInfo(String filterId, int w){
  String api = "http://thongtinbenhnhanbvnb.com/filter/" + filterId;
  String output = "";

  WiFiClient client;
  HTTPClient http;

  if (http.begin(client, api)) {
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.print("http code");
      Serial.println(httpCode);

      if (httpCode == HTTP_CODE_OK) {
        output = http.getString();
        Serial.println(output);
        StringFilterSplit(output, w);
      }
    } else {
      Serial.println(httpCode);
      Serial.println("Failed");
    }
    http.end();
  } else {
    Serial.println("Unable to connect");
  }
}

void increaseFilterUsed(String filterId, int w){
  String api = "http://thongtinbenhnhanbvnb.com/increaefilterused?id=" + filterId;
  String output = "";

  WiFiClient client;
  HTTPClient http;

  if (http.begin(client, api)) {
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.print("http code");
      Serial.println(httpCode);

      if (httpCode == HTTP_CODE_OK) {
        output = http.getString();
        Serial.println(output);
        StringFilterSplit(output, w);
      }
    } else {
      Serial.println(httpCode);
      Serial.println("Failed");
    }
    http.end();
  } else {
    Serial.println("Unable to connect");
  }
}

void OledScreenFilterInfo(String a, String b, String c, String d, String e, int w) {
  if (a == "success"){
    for (int j = 0; j < w; j++)
      for (int i = 0; i > -256; i-=2) {
        display1.clearDisplay();
        display.print(0, 0, "ID: " + b + " ~ Used: " + c + " time", SH110X_WHITE, SH110X_BLACK);
        display.print(i, 16, "Description: " + d, SH110X_WHITE, SH110X_BLACK);
        if (e == "false") display.print(84, 0,  + " ~ In use", SH110X_WHITE, SH110X_BLACK);
        else if (e == "true") display.print(84, 0, " ~ Finished", SH110X_WHITE, SH110X_BLACK);
        display1.display();
        if (i == 0) delay(1000); 
      }
    display1.clearDisplay();
    display.print(0, 0, "ID: " + b + " ~ Used: " + c + " time", SH110X_WHITE, SH110X_BLACK);
    display.print(-55, 16, "Description: " + d, SH110X_WHITE, SH110X_BLACK);
    if (e == "false") display.print(84, 0,  + " ~ In use", SH110X_WHITE, SH110X_BLACK);
    else if (e == "true") display.print(84, 0, " ~ Finished", SH110X_WHITE, SH110X_BLACK);
    display1.display();
  }

  else if (a == "fail"){
    display1.clearDisplay();
    display.print(4, 16, "CANNOT FIND INFORMATION", SH110X_WHITE, SH110X_BLACK);
    display.print(28, 32, "OF THIS FILTER", SH110X_WHITE, SH110X_BLACK);
    display1.display();
  }
}

void OledScreenPatientInfo(String a, String b, String c) {
      display.print(0, 32, "Patient: " + a, SH110X_WHITE, SH110X_BLACK);
      display.print(0, 48, "Age: " + b + " ~ Phone: " + c, SH110X_WHITE, SH110X_BLACK);
      display1.display();
}

void StringFilterSplit(String input, int w){
  int msgPosStart = input.indexOf("\"msg\":") + 7;
  int msgPosEnd   = input.indexOf("\"", msgPosStart);
  String msg = input.substring(msgPosStart, msgPosEnd);

  int idPosStart = input.indexOf("\"id\":") + 6;
  int idPosEnd   = input.indexOf("\"", idPosStart);
  String id = input.substring(idPosStart, idPosEnd);

  int usedPosStart = input.indexOf("\"used\":") + 7;
  int usedPosEnd   = input.indexOf(",", usedPosStart);
  String used = input.substring(usedPosStart, usedPosEnd);

  int desPosStart = input.indexOf("\"description\":") + 15;
  int desPosEnd   = input.indexOf("\"", desPosStart);
  String des = input.substring(desPosStart, desPosEnd);
  des.replace('-', '~');

  int isFinishedPosStart = input.indexOf("\"isFinished\":") + 13;
  int isFinishedPosEnd   = input.indexOf(",", isFinishedPosStart);
  String isFinished = input.substring(isFinishedPosStart, isFinishedPosEnd);

  int forPatientPosStart = input.indexOf("[") + 1;
  int forPatientPosEnd   = input.lastIndexOf("]");
  if (forPatientPosStart == forPatientPosEnd) OledScreenFilterInfo(msg, id, used, des, isFinished, w);
  else {
    String forPatient = input.substring(forPatientPosStart, forPatientPosEnd);

    int patientNo[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int number = 0;

    for (int j = 0; j < (forPatient.length() - 5); j++){
      if (forPatient.startsWith("\"id\":", j)){
        patientNo[number] = j;
        number++;
      }
    }

    OledScreenFilterInfo(msg, id, used, des, isFinished, w);

    for (int i = 0; i < number; i++){

      int patientPosStart = forPatient.indexOf("\"id\":", patientNo[i]);
      int patientPosEnd   = forPatient.indexOf("]", patientPosStart);
      String patient = forPatient.substring(patientPosStart, patientPosEnd);

      int patientIdPosStart = patient.indexOf("\"id\":") + 6;
      int patientIdPosEnd   = patient.indexOf("\"", patientIdPosStart);
      String patientId = patient.substring(patientIdPosStart, patientIdPosEnd);

      int namePatientPosStart = patient.indexOf("\"name\":") + 8;
      int namePatientPosEnd   = patient.indexOf("\"", namePatientPosStart);
      String namePatient = patient.substring(namePatientPosStart, namePatientPosEnd);

      int agePatientPosStart = patient.indexOf("\"age\":") + 6;
      int agePatientPosEnd   = patient.indexOf(",", agePatientPosStart);
      String agePatient = patient.substring(agePatientPosStart, agePatientPosEnd);

      int phonePatientPosStart = patient.indexOf("\"phone\":") + 9;
      int phonePatientPosEnd   = patient.indexOf("\"", phonePatientPosStart);
      String phonePatient = patient.substring(phonePatientPosStart, phonePatientPosEnd);

      OledScreenPatientInfo(namePatient, agePatient, phonePatient);

      int schedulePosStart = patient.indexOf("[") + 1;
      int schedulePosEnd   = patient.indexOf("]");
      if (schedulePosStart != schedulePosEnd) {
        String schedule = patient.substring(schedulePosStart, schedulePosEnd);

        int scheduleNo[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        int numberSchedule = 0;

        for (int j = 0; j < (schedule.length() - 7); j++){
          if (schedule.startsWith("\"time\":", j)){
            scheduleNo[numberSchedule] = j;
            numberSchedule++;
          }
        }

        for (int j = 0; j < numberSchedule; j++){

          int lichPosStart = schedule.indexOf("\"time\":", scheduleNo[j]);
          int lichPosEnd   = schedule.indexOf("}", lichPosStart);
          String lich = schedule.substring(lichPosStart, lichPosEnd);

          int scheduleTimePosStart = lich.indexOf("\"time\":") + 8;
          int scheduleTimePosEnd   = lich.indexOf("\"", scheduleTimePosStart);
          String scheduleTime = lich.substring(scheduleTimePosStart, scheduleTimePosEnd);

          int scheduleDayOfWeekPosStart = lich.indexOf("\"dayOfWeek\":") + 13;
          int scheduleDayOfWeekPosEnd   = lich.indexOf("\"", scheduleDayOfWeekPosStart);
          String scheduleDayOfWeek = lich.substring(scheduleDayOfWeekPosStart, scheduleDayOfWeekPosEnd);

        }
      }
    }
  }
}