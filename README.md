# iot_card_scanner

![STM32 Microcontroller](https://cdn.shopify.com/s/files/1/0609/6011/2892/files/doc-esp32-pinout-reference-wroom-devkit.png)

# README

This code is designed for an ESP32 microcontroller to interact with an RFID-RC522 reader and send RFID tag information to a specified API endpoint over HTTP. It enables the ESP32 to read RFID tags and communicate their presence to a remote server.

## Getting Started

To use this code, ensure you have the necessary hardware components set up, including an ESP32 microcontroller, an RFID-RC522 reader, and a stable internet connection. Additionally, make sure you have the required libraries installed in your Arduino IDE:

- MFRC522
- HTTPClient
- Arduino_JSON

## Installation

1. Clone or download the code repository.
2. Open the code in your Arduino IDE.
3. Make sure you have selected the correct board (ESP32) and port.
4. Upload the code to your ESP32 microcontroller.

## Configuration

### RFID Setup

- **SS_PIN (Slave Select Pin):** Connect the RFID-RC522 module's SS pin to GPIO5 (Pin 5) of the ESP32.
- **RST_PIN (Reset Pin):** Connect the RFID-RC522 module's RST pin to GPIO0 (Pin 0) of the ESP32.

### WiFi Settings

- **SSID (WiFi Name):** Set the SSID of your WiFi network.
- **Password:** Set the password of your WiFi network.

### API Configuration

- **urlToSendPresence:** Set the URL of the API endpoint where RFID tag information will be sent.

## Usage

1. Power on the ESP32 microcontroller.
2. Tap an RFID/NFC tag on the RFID-RC522 reader.
3. The ESP32 will read the tag information and attempt to send it to the specified API endpoint.
4. Check the serial monitor for debug messages and HTTP response codes.

## Important Notes

- Ensure that the ESP32 is connected to a stable WiFi network with internet access.
- Verify that the API endpoint specified in `urlToSendPresence` is accessible and capable of receiving POST requests.
- Make sure that the RFID-RC522 reader is functioning correctly and properly connected to the ESP32.

## Troubleshooting

- If the ESP32 fails to connect to WiFi, check your network credentials and connection stability.
- If HTTP requests fail, verify the correctness of the API endpoint URL and ensure that the server is operational.

For more detailed instructions and troubleshooting tips, visit [ESP32 RFID/NFC Tutorial](https://esp32io.com/tutorials/esp32-rfid-nfc).

## License

This code is released into the public domain. Feel free to modify and distribute it for any purpose without restrictions.

For further assistance or inquiries, please visit [esp32io.com](https://esp32io.com).

