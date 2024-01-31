/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-rfid-nfc
 */

#include <SPI.h>
#include <MFRC522.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <string.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include "musicNotes.h"
/**Buzzer**/
#define BUZZZER_PIN  16 // ESP32 pin GPIO18 connected to piezo buzzer
#define LED_ROUGE 14  // pin led rouge
#define LED_VERTE 26  // pin led verte
int melodyPresenceValid[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
int melodyPresenceInvalid[] = {
  NOTE_DS8
};

int noteDurationsValid[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

int noteDurationsInvalid[] = {
  1
};
/**RFID**/
#define SS_PIN  5  // ESP32 pin GPIO5 
#define RST_PIN 27 // ESP32 pin GPIO27 
MFRC522 rfid(SS_PIN, RST_PIN);
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDzTCCArWgAwIBAgIQCjeHZF5ftIwiTv0b7RQMPDANBgkqhkiG9w0BAQsFADBa\n" \
"MQswCQYDVQQGEwJJRTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJl\n" \
"clRydXN0MSIwIAYDVQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTIw\n" \
"MDEyNzEyNDgwOFoXDTI0MTIzMTIzNTk1OVowSjELMAkGA1UEBhMCVVMxGTAXBgNV\n" \
"BAoTEENsb3VkZmxhcmUsIEluYy4xIDAeBgNVBAMTF0Nsb3VkZmxhcmUgSW5jIEVD\n" \
"QyBDQS0zMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEua1NZpkUC0bsH4HRKlAe\n" \
"nQMVLzQSfS2WuIg4m4Vfj7+7Te9hRsTJc9QkT+DuHM5ss1FxL2ruTAUJd9NyYqSb\n" \
"16OCAWgwggFkMB0GA1UdDgQWBBSlzjfq67B1DpRniLRF+tkkEIeWHzAfBgNVHSME\n" \
"GDAWgBTlnVkwgkdYzKz6CFQ2hns6tQRN8DAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0l\n" \
"BBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwNAYI\n" \
"KwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5j\n" \
"b20wOgYDVR0fBDMwMTAvoC2gK4YpaHR0cDovL2NybDMuZGlnaWNlcnQuY29tL09t\n" \
"bmlyb290MjAyNS5jcmwwbQYDVR0gBGYwZDA3BglghkgBhv1sAQEwKjAoBggrBgEF\n" \
"BQcCARYcaHR0cHM6Ly93d3cuZGlnaWNlcnQuY29tL0NQUzALBglghkgBhv1sAQIw\n" \
"CAYGZ4EMAQIBMAgGBmeBDAECAjAIBgZngQwBAgMwDQYJKoZIhvcNAQELBQADggEB\n" \
"AAUkHd0bsCrrmNaF4zlNXmtXnYJX/OvoMaJXkGUFvhZEOFp3ArnPEELG4ZKk40Un\n" \
"+ABHLGioVplTVI+tnkDB0A+21w0LOEhsUCxJkAZbZB2LzEgwLt4I4ptJIsCSDBFe\n" \
"lpKU1fwg3FZs5ZKTv3ocwDfjhUkV+ivhdDkYD7fa86JXWGBPzI6UAPxGezQxPk1H\n" \
"goE6y/SJXQ7vTQ1unBuCJN0yJV0ReFEQPaA1IwQvZW+cwdFD19Ae8zFnWSfda9J1\n" \
"CZMRJCQUzym+5iPDuI9yP+kHyCREU3qzuWFloUwOxkgAyXVjBYdwRVKD05WdRerw\n" \
"6DEdfgkfCv4+3ao8XnTSrLE=\n" \
"-----END CERTIFICATE-----\n";
/**Data**/
int roomId = 1;
String cardId = "1234567891";
/**Wifi Settings**/
const char* ssid = "edsonwifi";
const char* password = "00000000";
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;
WiFiMulti WiFiMulti;
WiFiClientSecure *client = new WiFiClientSecure;

void setClock() {
  configTime(0, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

/**Initialisation**/
void setup() {
  /**Buzzer**/
  pinMode(BUZZZER_PIN, OUTPUT);
  /**LED ROUGE**/
  pinMode(LED_ROUGE, OUTPUT);
  /**LED VERTE**/
  pinMode(LED_VERTE, OUTPUT);
  Serial.begin(115200); 
  /**RFID**/
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
  /**Wifi**/
  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
 Serial.println(" connected");

setClock();  
}
/**Conversion de Char a String **/

void array_to_string(byte array[], unsigned int len, char buffer[])
{
   for (unsigned int i = 0; i < len; i++)
   {
      byte nib1 = (array[i] >> 4) & 0x0F;
      byte nib2 = (array[i] >> 0) & 0x0F;
      buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
      buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
   }
   buffer[len*2] = '\0';
}

void printBuzzer(int melody[],int duration[]){
    Serial.println(sizeof(melody)/sizeof(melody));
    int taille = sizeof(melody)/sizeof(melody);
   for (int thisNote = 0; thisNote < taille; thisNote++) {
    int noteDuration = 1000 / duration[thisNote];
    tone(BUZZZER_PIN, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZZER_PIN);
  }
}
void printBuzzer2(int melody[],int duration[],int tailleDuration){
    Serial.println(sizeof(melody)/sizeof(melody));
   for (int thisNote = 0; thisNote < tailleDuration; thisNote++) {
    int noteDuration = 1000 / duration[thisNote];
    tone(BUZZZER_PIN, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZZER_PIN);
  }
}


void allumerLedRouge(){
  digitalWrite(LED_ROUGE, HIGH); // turn the LED on
  delay(5);             // wait for 500 milliseconds
}
void etandreLedRouge(){
  digitalWrite(LED_ROUGE, LOW);  // turn the LED off
  delay(5);    
}
void allumerLedVerte(){
  digitalWrite(LED_VERTE, HIGH); // turn the LED on
  delay(5);             // wait for 500 milliseconds   
}
void etandreLedVerte(){
  digitalWrite(LED_VERTE, LOW);  // turn the LED off
  delay(5); 
}

/**Process RFID Lecture**/
bool rfidProcess(){
   if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // print UID in Serial Monitor in the hex format
      Serial.print("UID:");
      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();

      char str[(int)rfid.uid.size] = "";
      array_to_string(rfid.uid.uidByte,rfid.uid.size, str);
      Serial.println(str);
      /**Send to API**/
      cardId=str;
      //sendPresencePOST();
      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
      return true;
    } else{
      return false;
    }
  } else {
    return false;
  }
}

void sendPost3(WiFiClientSecure *client){
      {
      HTTPClient https;
  
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, "https://iot-m2-card.onrender.com/rooms/present")) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        https.addHeader("Content-Type", "application/json");
        char json[50]; // Assurez-vous que cette taille est suffisante pour contenir votre JSON
         sprintf(json, "{\"roomId\":%d,\"cardId\":\"%s\"}", roomId, cardId);
        int httpCode = https.POST(json);
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
            allumerLedVerte();
            printBuzzer2(melodyPresenceValid,noteDurationsValid,8);
            etandreLedVerte();
            
          }
          if(httpCode ==  HTTP_CODE_BAD_REQUEST){
            Serial.println("Print Burzzer 400");
            allumerLedRouge();
            printBuzzer2(melodyPresenceInvalid,noteDurationsInvalid,1);
            etandreLedRouge();
            
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
          allumerLedRouge();
          printBuzzer2(melodyPresenceInvalid,noteDurationsInvalid,1);
          
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    } 
}
void loop() {
  if(client) {
    client -> setCACert(rootCACertificate);

      if(rfidProcess()){
        sendPost3(client);
      }
  } else {
    Serial.println("Unable to create client");
  }
}