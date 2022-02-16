/*
  LoRaNetRouter.h
  Created by Bruno Carneiro
  Released into the public domain
*/

#ifndef LORAMESHROUTER_H
#define LORAMESHROUTER_H

#include <Arduino.h>
#include <SPI.h>
#include "LoRa.h"
#include "util/headers.h"
#include "util/libcc/hash.h"
#include "util/debug.h"
#include "util/error.h"

// #define             ACTIVE_ROUTE_TIMEOUT     3000
// #define             ALLOWED_HELLO_LOSS       2
// #define             BLACKLIST_TIMEOUT        RREQ_RETRIES * NET_TRAVERSAL_TIME
// #define             DELETE_PERIOD            see note below
// #define             HELLO_INTERVAL           1000
// #define             LOCAL_ADD_TTL            2
// #define             MAX_REPAIR_TTL           0.3 * NET_DIAMETER
// #define             MIN_REPAIR_TTL           see note below
// #define             MY_ROUTE_TIMEOUT         2 * ACTIVE_ROUTE_TIMEOUT
#define             NET_DIAMETER             35
#define             NET_TRAVERSAL_TIME       2 * NODE_TRAVERSAL_TIME * NET_DIAMETER
// #define             NEXT_HOP_WAIT            NODE_TRAVERSAL_TIME + 10
#define             NODE_TRAVERSAL_TIME      100
#define             PATH_DISCOVERY_TIME      2 * NET_TRAVERSAL_TIME
// #define             RERR_RATELIMIT           10
// #define             RING_TRAVERSAL_TIME      2 * NODE_TRAVERSAL_TIME * (TTL_VALUE + TIMEOUT_BUFFER)
// #define             RREQ_RETRIES             2
// #define             RREQ_RATELIMIT           10
// #define             TIMEOUT_BUFFER           2
// #define             TTL_START                1
// #define             TTL_INCREMENT            2
// #define             TTL_THRESHOLD            7
// #define             TTL_VALUE                see note below

typedef struct {
    uint8_t         dstAddr[4];
    uint8_t         dstSequence;
    uint8_t         hopCount;
    uint8_t         nextHop[4];
    uint8_t         unknownSequence : 1;
    // List *          precursors;
    // unsigned long   lastUsedAt;
    bool            active;
} routingEntry;

typedef struct {
    uint8_t         dstAddr[4];
    uint8_t         rreqID;
    unsigned long   timestamp;
} rreqBufferEntry;

class LoRaNetRouterClass {
    public:
        LoRaNetRouterClass();
        // EXPERIMENT
        unsigned long    RREQ_TS = 0;

        int             begin(uint8_t *addrp);
        void            createRREQ(uint8_t *dstAddr);
        void            handleRREQ(int packetSize, uint8_t *previousHop);
        void            forwardRREQ(rreqHeader rreq);
        void            createRREP(rreqHeader rreq, bool thisNodeIsDestination);
        void            handleRREP(int packetSize, uint8_t *previousHop);
        void            forwardRREP(rrepHeader rrep);
        void            handlePacket(int packetSize, uint8_t *previousHop);
        void            getNextHop(uint8_t *dstAddr, uint8_t **nextHop);
        void            run();

    private:
        void            bufferRREQ(uint8_t *dstAddr, uint8_t rreqID);
        routingEntry    *getRoute(uint8_t *dstAddr);
};

extern LoRaNetRouterClass LoRaNetRouter;

#endif              /* LORAMESHROUTER_H */