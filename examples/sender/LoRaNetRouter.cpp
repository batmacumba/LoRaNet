/*
  LoRaNetRouter.cpp
  Created by Bruno Carneiro
  Released into the public domain
*/

#include "LoRaNetRouter.h"
#include "LoRaNetSwitch.h"
#include "util/list.h"

static uint8_t          broadcastAddr[4] = {10, 255, 255, 255};
static uint8_t          thisNodeAddr[4];
static uint8_t          thisNodeSequence = random(1, 255);
static uint8_t          rreqID = random(0, 255);
static hashtable_t      *routingTable = ht_create(16);
static List             *rreqBuffer = list_new();  

/*******************************************************************************
 * Auxiliary Functions
 * 
 ******************************************************************************/

int 
compareAddr(void *a, void *b)
{
    rreqBufferEntry *rbA = (rreqBufferEntry *) a;
    rreqBufferEntry *rbB = (rreqBufferEntry *) b;
    if (IPAddress(rbA -> dstAddr) == IPAddress(rbB -> dstAddr)) return 0;
    return 1;
}

int 
compareAddrAndID(void *a, void *b)
{
    rreqBufferEntry *rbA = (rreqBufferEntry *) a;
    rreqBufferEntry *rbB = (rreqBufferEntry *) b;
    if (IPAddress(rbA -> dstAddr) == IPAddress(rbB -> dstAddr) &&
        rbA -> rreqID == rbB -> rreqID) 
        return 0;
    return 1;
}

/*******************************************************************************
 * Main Class
 * 
 ******************************************************************************/

LoRaNetRouterClass::LoRaNetRouterClass()
{
    Serial.printf("Does this run??\n");
    // EXPERIMENT
    uint8_t dstAddr[4] = {10, 7, 97, 46};
    uint8_t previousHop[4] = {10, 7, 97, 46};
    routingEntry *re;
    re = (routingEntry *) malloc(sizeof(routingEntry));
    memcpy(re -> dstAddr, dstAddr, 4);
    memcpy(re -> nextHop, previousHop, 4);
    re -> dstSequence = 0;
    re -> hopCount = 0;
    re -> active = true;
    re -> unknownSequence = 1;
    ht_set(routingTable, (char *) IPAddress(dstAddr).toString().c_str(), re);

    uint8_t dstAddr2[4] = {10, 105, 154, 79};
    uint8_t previousHop2[4] = {10, 248, 208, 24};
    re = (routingEntry *) malloc(sizeof(routingEntry));
    memcpy(re -> dstAddr, dstAddr2, 4);
    memcpy(re -> nextHop, previousHop2, 4);
    re -> dstSequence = 0;
    re -> hopCount = 0;
    re -> active = true;
    re -> unknownSequence = 1;
    ht_set(routingTable, (char *) IPAddress(dstAddr2).toString().c_str(), re);
}

void
LoRaNetRouterClass::begin(uint8_t *addrp)
{
    memcpy(thisNodeAddr, addrp, sizeof(thisNodeAddr));
}


void
LoRaNetRouterClass::createRREQ(uint8_t *dstAddr)
{
    // TODO: implement RREQ_RATELIMIT, implement max RREQ_RETRIES, implement
    // expanding ring search technique

    // We must wait NET_TRAVERSAL_TIME miliseconds before originating another RREQ
    routingEntry tmp;
    memcpy(tmp.dstAddr, dstAddr, 4);
    if (list_contains(rreqBuffer, (void *) &tmp, compareAddr)) return;

    LOG("LoRaNetRouter::createRREQ: creating RREQ for destination %s\n", 
        IPAddress(dstAddr).toString().c_str());

    genericFrame gf;
    routingEntry *re = LoRaNetRouter.getRoute(dstAddr);
    // New entry 
    if (re == NULL) {
        re = (routingEntry *) malloc(sizeof(routingEntry));
        memcpy(re -> dstAddr, dstAddr, 4);
        re -> dstSequence = 0;
        re -> hopCount = 0;
        re -> active = false;
        re -> unknownSequence = 1;
        ht_set(routingTable, (char *) IPAddress(dstAddr).toString().c_str(), re);
    }

    // TODO: sequence number should go from 255 to 1
    thisNodeSequence++;
    rreqID++;
    
    gf.ah.type = AODV_RREQ;
    gf.ah.rreq.unknownSequence = re -> unknownSequence;
    gf.ah.rreq.hopCount = 0;
    gf.ah.rreq.rreqID = rreqID;
    memcpy(gf.ah.rreq.dstAddr, dstAddr, 4);
    gf.ah.rreq.dstSequence = re -> dstSequence;
    memcpy(gf.ah.rreq.originAddr, thisNodeAddr, 4);
    gf.ah.rreq.originSequence = thisNodeSequence;

    LoRaNetSwitch.push(gf, ROUTING); 

    bufferRREQ(dstAddr, rreqID);
}

void
LoRaNetRouterClass::handleRREQ(int packetSize, uint8_t *previousHop)
{
    if (packetSize < sizeof(rreqHeader)) {
        LOG("LoRaNetRouter::handleRREQ: Packet is too short!\n");
        return;
    }

    // TODO: check remaining packet size
    LOG("LoRaNetRouter::handleRREQ: RREQ message received\n");
    rreqHeader rreq;
    for (int i = 0; i < sizeof(rreq); i++) 
        *(((uint8_t*) &rreq) + i) = (uint8_t) LoRa.read();

    // Creates a route to previous hop
    routingEntry *re = LoRaNetRouter.getRoute(previousHop);

    if (re == NULL) {
        re = (routingEntry *) malloc(sizeof(routingEntry));
        memcpy(re -> dstAddr, previousHop, 4);
        re -> dstSequence = 0;
        re -> hopCount = 1;
        memcpy(re -> nextHop, previousHop, 4);
        re -> unknownSequence = 1;
        re -> active = true;
        ht_set(routingTable, (char *) IPAddress(previousHop).toString().c_str(), re);
    }
    
    // Discards known RREQs
    rreqBufferEntry tmp;
    memcpy(tmp.dstAddr, rreq.dstAddr, 4);
    tmp.rreqID = rreq.rreqID;
    if (list_contains(rreqBuffer, (void *) &tmp, compareAddrAndID)) {
        LOG("LoRaNetRouter::handleRREQ: RREQ message is already in buffer! Discarding...\n");
        return;
    }

    // RREQ Handling
    rreq.hopCount++;

    // BUG: check everything below this line

    // Creates or updates the reverse route to origin
    re = LoRaNetRouter.getRoute(rreq.originAddr);

    if (re == NULL) {
        re = (routingEntry *) malloc(sizeof(routingEntry));
        memcpy(re -> dstAddr, rreq.originAddr, 4);
        re -> dstSequence = 0;
        ht_set(routingTable, (char *) IPAddress(previousHop).toString().c_str(), re);
    }
    // TODO: this comparison below should be fixed
    if (rreq.originSequence > re -> dstSequence) 
        re -> dstSequence = rreq.originSequence;
    re -> unknownSequence = 0;
    memcpy(re -> nextHop, previousHop, 4);
    re -> hopCount = rreq.hopCount;
    re -> active = true;
    // TODO: implement lifetime as per rfc3561 instructions

    // RREP decision
    re = LoRaNetRouter.getRoute(rreq.dstAddr);

    if (IPAddress(rreq.dstAddr) == IPAddress(thisNodeAddr)) 
        LoRaNetRouter.createRREP(rreq, true);
    // EXPERIMENT
    // else if (re != NULL)
    //     LoRaNetRouter.createRREP(rreq, false);
    else 
        LoRaNetRouter.forwardRREQ(rreq);
}

void
LoRaNetRouterClass::forwardRREQ(rreqHeader rreq)
{
    if (rreq.hopCount > NET_DIAMETER) {
        LOG("LoRaNetRouter::forwardRREQ: hopCount is higher than NET_DIAMETER! Discarding...\n");
        return;
    }

    LOG("LoRaNetRouter::forwardRREQ: forwarding RREQ to destination %s\n", 
        IPAddress(rreq.dstAddr).toString().c_str());

    genericFrame gf;
    gf.ah.type = AODV_RREQ;
    gf.ah.rreq = rreq;

    LoRaNetSwitch.push(gf, ROUTING); 
    bufferRREQ(rreq.dstAddr, rreq.rreqID);
}

void
LoRaNetRouterClass::createRREP(rreqHeader rreq, bool thisNodeIsDestination)
{
    LOG("LoRaNetRouter::createRREP: creating RREP for originator %s\n", 
        IPAddress(rreq.originAddr).toString().c_str());

    genericFrame gf;
    gf.ah.type = AODV_RREP;
    memcpy(gf.ah.rrep.originAddr, rreq.originAddr, 4);

    if (thisNodeIsDestination) {
        if (thisNodeSequence + 1 == rreq.dstSequence) thisNodeSequence++;
        gf.ah.rrep.dstSequence = thisNodeSequence;
        memcpy(gf.ah.rrep.dstAddr, thisNodeAddr, 4);
        gf.ah.rrep.hopCount = 0;
    }
    else {
        routingEntry *re = LoRaNetRouter.getRoute(rreq.dstAddr);
        if (re != NULL) {
            gf.ah.rrep.dstSequence = re -> dstSequence;
            memcpy(gf.ah.rrep.dstAddr, re -> dstAddr, 4);
            // TODO: is this correct? vvv
            gf.ah.rrep.hopCount = rreq.hopCount + re -> hopCount;
        }
    }

    LoRaNetSwitch.push(gf, ROUTING); 
}

void
LoRaNetRouterClass::handleRREP(int packetSize, uint8_t *previousHop)
{
    if (packetSize < sizeof(rrepHeader)) {
        LOG("LoRaNetRouter::handleRREP: Packet is too short!\n");
        return;
    }

    // TODO: check remaining packet size
    LOG("LoRaNetRouter::handleRREP: RREP message received\n");
    rrepHeader rrep;
    for (int i = 0; i < sizeof(rrep); i++) 
        *(((uint8_t*) &rrep) + i) = (uint8_t) LoRa.read();
    LOG("LoRaNetRouter::handleRREP: handling RREP from destination %s\n", 
        IPAddress(rrep.dstAddr).toString().c_str());

    // Creates a route to previous hop
    routingEntry *re = LoRaNetRouter.getRoute(previousHop);

    if (re == NULL) {
        re = (routingEntry *) malloc(sizeof(routingEntry));
        memcpy(re -> dstAddr, previousHop, 4);
        re -> dstSequence = 0;
        re -> hopCount = 1;
        memcpy(re -> nextHop, previousHop, 4);
        re -> unknownSequence = 1;
        re -> active = true;
        ht_set(routingTable, (char *) IPAddress(previousHop).toString().c_str(), re);
    }

    // RREP Handling
    rrep.hopCount++;

    // BUG: check everything below this line

    // Creates or updates the forward route to destination
    re = LoRaNetRouter.getRoute(rrep.dstAddr);

    if (re == NULL) {
        re = (routingEntry *) malloc(sizeof(routingEntry));
        memcpy(re -> dstAddr, rrep.dstAddr, 4);
        re -> dstSequence = 0;
        re -> unknownSequence = 1;
        ht_set(routingTable, (char *) IPAddress(previousHop).toString().c_str(), re);
    }
    // TODO: this sequence number comparison below should be fixed
    if (re -> unknownSequence ||
        (rrep.dstSequence > re -> dstSequence && re -> unknownSequence == 0) ||
        (rrep.dstSequence == re -> dstSequence && re -> active == false) ||
        (rrep.dstSequence == re -> dstSequence && rrep.hopCount < re -> hopCount)) {
        re -> dstSequence = rrep.dstSequence;
        re -> unknownSequence = 0;
        re -> hopCount = rrep.hopCount;
        memcpy(re -> nextHop, previousHop, 4);
        re -> active = true;
    }
    // TODO: implement lifetime as per rfc3561 instructions

    // RREP forwarding decision
    re = LoRaNetRouter.getRoute(rrep.originAddr);

    if (IPAddress(rrep.originAddr) != IPAddress(thisNodeAddr)) 
        LoRaNetRouter.forwardRREP(rrep);
}

void
LoRaNetRouterClass::forwardRREP(rrepHeader rrep)
{
    if (rrep.hopCount > NET_DIAMETER) {
        LOG("LoRaNetRouter::forwardRREP: hopCount is higher than NET_DIAMETER! Discarding...\n");
        return;
    }

    LOG("LoRaNetRouter::forwardRREP: forwarding RREP to origin %s\n", 
        IPAddress(rrep.originAddr).toString().c_str());

    genericFrame gf;
    gf.ah.type = AODV_RREP;
    gf.ah.rrep = rrep;
    LoRaNetSwitch.push(gf, ROUTING); 
}

void
LoRaNetRouterClass::handlePacket(int packetSize, uint8_t *previousHop)
{
    if (packetSize < 1) {
        LOG("LoRaNetRouter::handlePacket: Packet is too short!\n");
        return;
    }

    // TODO: check remaining packet size
    LOG("LoRaNetRouter::handlePacket: Routing message received\n");
    uint8_t type = (uint8_t) LoRa.read();
    if (type == AODV_RREQ) LoRaNetRouter.handleRREQ(packetSize - 1, previousHop); 
    else if (type == AODV_RREP) LoRaNetRouter.handleRREP(packetSize - 1, previousHop); 
    else if (type == AODV_RERR) ;
    else if (type == AODV_RREP_ACK) ;
}

void   
LoRaNetRouterClass::getNextHop(uint8_t *dstAddr, uint8_t **nextHop)
{
    if (IPAddress(dstAddr) == IPAddress(broadcastAddr)) {
        *nextHop = (uint8_t *) &broadcastAddr;
        return;
    }

    uint8_t tmp[4];
    memcpy(tmp, dstAddr, 4);
    char *key = (char *) IPAddress(tmp).toString().c_str();
    routingEntry *re = (routingEntry *) ht_get(routingTable, key);
    // EXPERIMENT
    if (re == NULL || re -> active == false) *nextHop = NULL;
    // if (re == NULL) *nextHop = NULL;
    else *nextHop = (uint8_t *) &re -> nextHop;
}

void
LoRaNetRouterClass::run()
{
    // TODO: discard stale routes?
    for (int i = 0 ; i < list_length(rreqBuffer); i++) {
        // LOG("LoRaNetRouter::run: run %d\n", i);
        unsigned long now = millis();
        rreqBufferEntry *rbEntry = (rreqBufferEntry *) list_get(rreqBuffer, i);
        if (rbEntry == NULL) continue;
        // TODO: handle RREQ retries before deletion
        if (now - rbEntry -> timestamp > PATH_DISCOVERY_TIME) 
            list_del(rreqBuffer, i);
    }
}

void
LoRaNetRouterClass::bufferRREQ(uint8_t *dstAddr, uint8_t rreqID)
{
    rreqBufferEntry newEntry;
    memcpy(newEntry.dstAddr, dstAddr, 4);
    newEntry.rreqID = rreqID;
    newEntry.timestamp = millis();
    list_append(rreqBuffer, &newEntry, sizeof(rreqBufferEntry));
}

routingEntry *
LoRaNetRouterClass::getRoute(uint8_t *dstAddr)
{
    char *key = (char *) IPAddress(dstAddr).toString().c_str();
    routingEntry *re = (routingEntry *) ht_get(routingTable, key);
    return re;
}



LoRaNetRouterClass LoRaNetRouter;

// typedef struct {
//     uint8_t unknownSequence : 1;
//     uint8_t hopCount;
//     uint8_t rreqID;
//     uint8_t dstAddr[4];
//     uint8_t dstSequence;
//     uint8_t originAddr[4];
//     uint8_t originSequence;
// } rreqHeader;

// typedef struct {
//     uint8_t hopCount;
//     uint8_t dstAddr[4];
//     uint8_t dstSequence;
//     uint8_t originAddr[4];
// } rrepHeader;

// typedef struct {
//     uint8_t unreachableDstAddr[4];
//     uint8_t unreachableDstSequence;
// } rerrHeader;

// typedef struct {
//     uint8_t dstAddr[4];
//     uint8_t originAddr[4];
// } rrepAckHeader;

// typedef struct {
//     uint8_t type;
//     rreqHeader rreq;
//     rrepHeader rrep;
//     rerrHeader rerr;
//     rrepAckHeader rrepAck;
// } aodvHeader;

// typedef struct {
//     uint8_t version : 4;
//     uint8_t len;
//     uint8_t ttl;
//     uint8_t srcAddr[4];
//     uint8_t dstAddr[4];
// } networkHeader;

// typedef struct {
//     uint8_t version : 2;
//     uint8_t type : 2;
//     uint8_t subtype : 4;
//     uint8_t sequence;
//     uint8_t srcAddr[4];
//     uint8_t dstAddr[4];
// } linkHeader;

// typedef struct {
//     linkHeader lh;
//     networkHeader nh;
//     aodvHeader ah;
//     uint8_t payload[PAYLOAD_MAX_LEN];
// } genericFrame;