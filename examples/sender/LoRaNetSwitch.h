/*
  LoRaNetSwitch.h
  Created by Bruno Carneiro
  Released into the public domain
*/

#ifndef LORAMESHSWITCH_H
#define LORAMESHSWITCH_H

#include <Arduino.h>
#include <SPI.h>
#include "LoRa.h"
#include "util/headers.h"
#include "util/debug.h"
#include "util/error.h"

#define MAX_DATA_QUEUE_LEN  16

typedef struct {
    uint8_t         type;
    genericFrame    gf;
    bool            headerFilled;
    unsigned long   lastSentAt;
    int             retries;
} queueItem;

typedef struct {
    uint8_t         address[4];
    uint8_t         sequence;
    int             rssi;
} neighbor;

typedef struct {
    uint8_t         address[4];
    uint8_t         nextHop[4];
} nextHopEntry;

class LoRaNetSwitchClass {
    public:
        LoRaNetSwitchClass();

        void    begin(uint8_t *addrp);
        void    push(genericFrame dp, uint8_t type);
        void    acknowledge(uint8_t *dstAddr, uint8_t sequence);
        void    run();

    private:
        void    handleQueueItem(queueItem *qi, uint8_t type, int qIndex);
        uint8_t getNextSequence(uint8_t *address);
};

extern LoRaNetSwitchClass LoRaNetSwitch;

#endif              /* LORAMESHSWITCH_H */