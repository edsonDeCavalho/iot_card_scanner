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

/**RFID**/
#define SS_PIN  5  // ESP32 pin GPIO5 
#define RST_PIN 0 // ESP32 pin GPIO27 
MFRC522 rfid(SS_PIN, RST_PIN);

/**Data**/
int roomId = 0;
String cardId = "03EF37FA";
/**Api URLS**/
String urlToSendPresence = "http://172.20.10.3:1234/rooms/present";
/**Wifi Settings**/
const char* ssid = "coralie";
const char* password = "Loacoco52001";
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;
/**Initialisation**/
void setup() {
   Serial.begin(115200); 
  /**RFID**/
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
  /**Wifi**/
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
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
/**Process RFID Lecture**/
void rfidProcess(){
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
      sendPresencePOST();
      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}
void sendPresencePOST(){
      WiFiClient client;
      HTTPClient http;
      // Your Domain name with URL path or IP address with path
      http.begin(urlToSendPresence.c_str());
      
      http.addHeader("Content-Type", "application/json");
      char json[50]; // Assurez-vous que cette taille est suffisante pour contenir votre JSON
      sprintf(json, "{\"room\":\"%d\",\"cardId\":\"%s\"}", roomId, cardId);
      int httpResponseCode = http.POST(json);

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
}
void loop() {
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
  
    if(WiFi.status()== WL_CONNECTED){
       /*Process à faire des que en possede internet*/
      Serial.println("Wifi connected  !!!!!  :) ");
      //sendPresencePOST();
      rfidProcess();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}



