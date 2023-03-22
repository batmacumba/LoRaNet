#include "LoRaNet.h"
#include "boards.h"

#define BAND    868E6 
// #define BAND    433E6 

int i = 0;
unsigned long lastSentMsgTS = 0;
unsigned long lastMemoryCheckedTS = 0;
// int serial_putchar(char c, FILE* f) {
//     if (c == '\n') serial_putchar('\r', f);
//     return Serial.write(c) == 1? 0 : 1;
// }
// FILE serial_stdout;

void 
setup()
{
    initBoard();
    // Serial.begin(115200);
    // fdev_setup_stream(&serial_stdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);
    // stdout = &serial_stdout;
    LoRaNet.begin(BAND);
    LoRaNet.onReceive(handleMessage);
    LoRa.setCodingRate4(5);
    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(250E3);
    Serial.printf("Setup Complete\n");
}

void 
handleMessage(int payloadSize, IPAddress srcAddr)
{
    String message = "";
    for (int i = 0; i < payloadSize; i++) 
        message += (char) LoRa.read();

    Serial.print(message);
    Serial.print("' with RSSI ");
    Serial.print(LoRa.packetRssi());
    Serial.print(" from address ");
    Serial.println(srcAddr.toString());
    if (message == "ping") {
        message = "pong";
        Serial.println(message);
        LoRaNet.beginPacket(srcAddr.toString().c_str());
        LoRaNet.write((uint8_t *) message.c_str(), strlen(message.c_str()));
        LoRaNet.endPacket();
    }
}

void 
loop()
{
    // if (millis() - lastSentMsgTS > 10000) {
    //     Serial.printf("Sending message to 10.39.54.148\n");
    //     String message = "ping";
    //     Serial.println(message);
    //     LoRaNet.beginPacket("10.39.54.148");
    //     LoRaNet.write((uint8_t *) message.c_str(), strlen(message.c_str()));
    //     Serial.println("check 1");
    //     LoRaNet.endPacket();
    //     lastSentMsgTS = millis();
    //     Serial.println("message sent");
    // }
    LoRaNet.run();
}

