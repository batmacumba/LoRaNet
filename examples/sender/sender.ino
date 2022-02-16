#include <LoRaNet.h>

#define BAND    433E6 

int i = 0;
unsigned long lastSentMsgTS = 0;
unsigned long lastMemoryCheckedTS = 0;

void 
setup()
{
    Serial.begin(115200);
    LoRaNet.begin(BAND);
    LoRaNet.onReceive(handleMessage);

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
    if (millis() - lastSentMsgTS > 5000) {
        LoRaNet.beginPacket("10.105.154.79");
        String message = "ping";
        Serial.println(message);
        LoRaNet.write((uint8_t *) message.c_str(), strlen(message.c_str()));
        LoRaNet.endPacket();
        lastSentMsgTS = millis();
    }
    LoRaNet.run();
}

