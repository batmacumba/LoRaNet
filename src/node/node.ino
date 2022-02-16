#include "LoRaNet.h"

#define BAND    433E6 

int i = 0;
unsigned long lastSentMsgTS = 0;
unsigned long lastMemoryCheckedTS = 0;

void 
setup()
{
    Serial.begin(115200);
    // Serial.println(ESP.getFreeHeap());
    LoRaNet.begin(BAND);
    LoRaNet.onReceive(handleMessage);
    // Serial.println(ESP.getFreeHeap());
    Serial.println("sizeof(linkHeader)");
    Serial.println(sizeof(linkHeader));
    Serial.println("sizeof(networkHeader)");
    Serial.println(sizeof(networkHeader));
    Serial.println("sizeof(rreqHeader)");
    Serial.println(sizeof(rreqHeader));
    Serial.println("sizeof(rrepHeader)");
    Serial.println(sizeof(rrepHeader));

}

void 
handleMessage(int payloadSize, IPAddress srcAddr)
{
    // received a packet
    // Serial.print("Received packet '");
    String message = "";
    // read packet
    for (int i = 0; i < payloadSize; i++) 
        message += (char) LoRa.read();

    // Serial.print(message);
    // print RSSI of packet
    // Serial.print("' with RSSI ");
    // Serial.print(LoRa.packetRssi());
    // Serial.print(" from address ");
    // Serial.println(srcAddr.toString());
    if (message == "ping") {
        message = "pong";
        Serial.println(message);
        LoRaNet.beginPacket(srcAddr.toString().c_str());
        LoRaNet.write((uint8_t *) message.c_str(), strlen(message.c_str()));
        LoRaNet.endPacket();
    }
    else if (message == "pong") {
        Serial.println(millis() - lastSentMsgTS);
    }
}

void 
loop()
{
    // if (millis() - lastSentMsgTS > 5000) {
    //     LoRaNet.beginPacket("10.105.154.79");
    //     String message = "ping";
    //     // Serial.println(message);
    //     LoRaNet.write((uint8_t *) message.c_str(), strlen(message.c_str()));
    //     LoRaNet.endPacket();
    //     lastSentMsgTS = millis();
    // }
    if (millis() - lastMemoryCheckedTS > 5000) {
        Serial.println(ESP.getFreeHeap());
        lastMemoryCheckedTS = millis();
    }
    LoRaNet.run();
}

