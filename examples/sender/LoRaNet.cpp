#include "HardwareSerial.h"
/*
  LoRaNet.cpp
  Created by Bruno Carneiro
  Released into the public domain
*/

#include "LoRaNet.h"
#include "util/eeprom.h"
#include "util/headers.h"

static uint8_t          thisNodeAddr[4];
// TODO: this is not thread-safe
static genericFrame     snd;
static genericFrame     rcv;

static void (*_onReceive)(int, IPAddress);

LoRaNetClass::LoRaNetClass() :
  _onReceive(NULL)
{
    ;
}

void 
LoRaNetClass::onReceive(void(*callback)(int, IPAddress))
{
  _onReceive = callback;
}

void
LoRaNetClass::handlePacket(int packetSize)
{
    if (packetSize < sizeof(networkHeader)) {
        LOG("LoRaNet::handlePacket: Packet is too short!\n");
        return;
    }

    // TODO: check remaining packet size
    LOG("LoRaNet::handlePacket: Data message received\n");
    for (int i = 0; i < sizeof(rcv.nh); i++) 
        *(((uint8_t*) &rcv.nh) + i) = (uint8_t) LoRa.read();
    LOG("LoRaNet::handlePacket: Network layer header parsed successfully\n");

    if (IPAddress(rcv.nh.dstAddr) == IPAddress(thisNodeAddr)) {
        if (_onReceive) 
            _onReceive(packetSize - sizeof(rcv.nh), IPAddress(rcv.nh.srcAddr));
    }
    else {
        LOG("LoRaNet::handlePacket: Message addressed to another node, forwarding...\n");
        for (int i = 0; i < rcv.nh.len; i++)
            *(((uint8_t*) &rcv.payload) + i) = (uint8_t) LoRa.read();
        // TODO: check if remaining packetsize is good 
        
        /*********************************
         * PAREI AQUI
         *  FAZER O DECRESCIMO DO TTL
         *  SE NAO HOUVER => RERR
        *********************************/
        LoRaNetSwitch.push(rcv, DATA); 
    }
}

int
LoRaNetClass::begin(long frequency)
{
    Serial.printf("This is a checkpoint2\n");
    // TODO: return SUCCESS or FAIL
    // LoRa radio setup
    SPI.begin(SCK,MISO,MOSI,SS);
    // LoRa.setPins(SS,RST_LoRa,DIO0);
    LoRa.setPins(SS,23,26);

    // TODO: PADBOOST?
    if (!LoRa.begin(frequency, true)) {
        LOG("LoRaNet::begin: LoRa startup failed!\n");
        goto lmFail;
    }
    else {
        LOG("LoRaNet::begin: LoRa startup successful.\n");
    }

    // Address recovery
    if (!EEPROM.begin(EEPROM_SIZE)) {
        LOG("LoRaNet::begin: EEPROM initialization failed!\n");
        goto lmFail;
    }
    else
        LOG("LoRaNet::begin: EEPROM initialized!\n");

    if (eepromConfigured()) eepromRead(thisNodeAddr);
    else eepromSetup(thisNodeAddr);
    Serial.printf("This is a checkpoint3\n");
    for (int i=0; i<4; i++){
      Serial.printf("%u.", thisNodeAddr[i]);
    }
    Serial.printf("\n%u\n", thisNodeAddr);
    LoRaNetSwitch.begin(thisNodeAddr);
    Serial.printf("This is a checkpoint8\n");
    LoRaNetRouter.begin(thisNodeAddr);
    Serial.printf("This is a checkpoint9\n");

    lmFail:
        return FAIL;
}

int
LoRaNetClass::beginPacket(const char *host)
{
    // TODO: error checking
    // TODO: input sanitization
    snd.nh.version = 0;
    snd.nh.len = 0;
    snd.nh.ttl = INITIAL_TTL;

    IPAddress dst;
    dst.fromString(host);
    uint8_t dstAddr[4] = {dst[0], dst[1], dst[2], dst[3]};
    memcpy(snd.nh.srcAddr, thisNodeAddr, 4);
    memcpy(snd.nh.dstAddr, dstAddr, 4);
}

size_t 
LoRaNetClass::write(const uint8_t *buffer, size_t size)
{
    // TODO: error checking
    // TODO: return an informative error
    if (snd.nh.len + size > PAYLOAD_MAX_LEN) return FAIL;

    memcpy(snd.payload + snd.nh.len, buffer, size);
    snd.nh.len += size;
    return size;
}

int
LoRaNetClass::endPacket()
{
    // TODO: error checking
    // TODO: use mutex below
    LoRaNetSwitch.push(snd, DATA); 
}

void
LoRaNetClass::run()
{
    LoRaNetSwitch.run(); 
    LoRaNetRouter.run();
}


LoRaNetClass LoRaNet;