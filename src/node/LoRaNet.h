/*
  LoRaNet.h
  Created by Bruno Carneiro
  Released into the public domain
*/

#ifndef LORAMESH_H
#define LORAMESH_H

// TODO: debug mode 

#include <Arduino.h>
#include <SPI.h>
#include "LoRa.h"
#include "LoRaNetSwitch.h"
#include "LoRaNetRouter.h"
#include "util/debug.h"
#include "util/error.h"

class LoRaNetClass {
    public:
        LoRaNetClass();
        int begin(long frequency);
        int beginPacket(const char *host);
        int endPacket();
        size_t write(const uint8_t *buffer, size_t size);
        void onReceive(void(*callback) (int, IPAddress));
        void handlePacket(int packetSize);
        void run();
    private:
        void (*_onReceive)(int, IPAddress);
};

extern LoRaNetClass LoRaNet;

#endif              /* LORAMESH_H */