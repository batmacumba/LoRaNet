/*
  LoRaNetSwitch.cpp
  Created by Bruno Carneiro
  Released into the public domain
*/

#include "LoRaNetSwitch.h"
#include "LoRaNetRouter.h"
#include "LoRaNet.h"
#include "util/list.h"

#define             MAX_ACK_INTERVAL        600   
#define             ACK_LISTEN_INTERVAL     300
#define             MAX_RETRIES             3       
#define             DEFAULT_SF              7

// Global variables
static uint8_t          thisNodeAddr[4];
static uint8_t          broadcastAddr[4] = {10, 255, 255, 255};
static List             *frameQueue = list_new();        
static hashtable_t      *neighbors = ht_create(16);
static hashtable_t      *nextHopTable = ht_create(16);
static unsigned long    listenForAckTS = 0;
static uint8_t          listenForAckSequence;

// TODO: check for NULLs everywhere
// TODO: break big functions into smaller functions

void
populateRoutingTable()
{
    // // "10.105.154.79"
    // char *dst = "10.105.154.79";
    // nextHopEntry *fakeEntry = (nextHopEntry *) malloc(sizeof(nextHopEntry));
    // fakeEntry -> address[0] = 10; fakeEntry -> address[1] = 105; fakeEntry -> address[2] = 154; fakeEntry -> address[3] = 79;
    // fakeEntry -> nextHop[0] = 10; fakeEntry -> nextHop[1] = 105; fakeEntry -> nextHop[2] = 154; fakeEntry -> nextHop[3] = 79; 
    // ht_set(nextHopTable, (char *) dst, fakeEntry);

    // // "10.7.97.46"
    // dst = "10.7.97.46";
    // nextHopEntry *fakeEntry2 = (nextHopEntry *) malloc(sizeof(nextHopEntry));
    // fakeEntry2 -> address[0] = 10; fakeEntry2 -> address[1] = 7; fakeEntry2 -> address[2] = 97; fakeEntry2 -> address[3] = 46;
    // fakeEntry2 -> nextHop[0] = 10; fakeEntry2 -> nextHop[1] = 7; fakeEntry2 -> nextHop[2] = 97; fakeEntry2 -> nextHop[3] = 46; 
    // ht_set(nextHopTable, (char *) dst, fakeEntry2);

    // BROADCAST - DO NOT REMOVE
    char *dst = "10.255.255.255";
    nextHopEntry *broadcast = (nextHopEntry *) malloc(sizeof(nextHopEntry));
    broadcast -> address[0] = 10; broadcast -> address[1] = 255; broadcast -> address[2] = 255; broadcast -> address[3] = 255;
    broadcast -> nextHop[0] = 10; broadcast -> nextHop[1] = 255; broadcast -> nextHop[2] = 255; broadcast -> nextHop[3] = 255; 
    ht_set(nextHopTable, (char *) dst, broadcast);
}

void
onReceive(int packetSize)
{
    if (packetSize < sizeof(linkHeader)) {
        LOG("LoRaNetSwitch::onReceive: Frame received is too short!\n");
        return;
    }

    // TODO: check if frame is LoRaNet
    genericFrame gf;

    for (int i = 0; i < sizeof(gf.lh); i++) 
        *(((uint8_t*) &gf.lh) + i) = (uint8_t) LoRa.read();
    LOG("LoRaNetSwitch::onReceive: Link layer header parsed successfully\n");
    LOG("LoRaNetSwitch::onReceive: Received frame type %d from source %s, sequence %d\n", gf.lh.type, 
        IPAddress(gf.lh.srcAddr).toString().c_str(), gf.lh.sequence);
    
    // EXPERIMENT: Block IP
    // if (IPAddress(gf.lh.srcAddr) == IPAddress(10, 7, 97, 46)) return;
    // if (IPAddress(gf.lh.srcAddr) == IPAddress(10, 151, 36, 241)) return;
    // if (IPAddress(gf.lh.srcAddr) == IPAddress(10, 248, 208, 24)) return;
    if (IPAddress(gf.lh.srcAddr) == IPAddress(10, 105, 154, 79)) return;

    // Save RSSI on dictionary
    char *key = (char *) IPAddress(gf.lh.srcAddr).toString().c_str();
    neighbor *n = (neighbor *) ht_get(neighbors, key);
    if (n == NULL) {
        n = (neighbor *) malloc(sizeof(neighbor));
        memcpy(n -> address, gf.lh.srcAddr, 4);
        n -> sequence = random(1, 255);
        n -> rssi = LoRa.packetRssi();
        ht_set(neighbors, (char *) IPAddress(gf.lh.srcAddr).toString().c_str(), n);
    }
    else n -> rssi = LoRa.packetRssi();

    // TODO: promiscuous mode?
    if (IPAddress(gf.lh.dstAddr) != IPAddress(thisNodeAddr) &&
        IPAddress(gf.lh.dstAddr) != IPAddress(broadcastAddr)) {
        LOG("LoRaNetSwitch::onReceive: Frame addressed to another node. Discarding...\n");
        return;
    }
    
    // Control Packets Handling
    if (gf.lh.type == CONTROL) {
        LOG("LoRaNetSwitch::onReceive: Control frame received\n");
        if (gf.lh.subtype == CONTROL_ACK) {
            LOG("LoRaNetSwitch::onReceive: ACK received for frame sequence %d\n", gf.lh.sequence);
            // TODO: write a new function for removing queued frames
            for (int i = 0 ; i < list_length(frameQueue); i++) {
                queueItem *qi = (queueItem *) list_get(frameQueue, i);
                if (IPAddress(qi -> gf.lh.dstAddr) == IPAddress(gf.lh.srcAddr) &&
                    qi -> gf.lh.sequence == gf.lh.sequence) {
                    list_del(frameQueue, i);
                    LOG("LoRaNetSwitch::onReceive: Removed acked frame from queue\n");
                }
            }

            if (gf.lh.sequence == listenForAckSequence) {
                LOG("LoRaNetSwitch::onReceive: stopped listening to ACK\n");
                listenForAckTS = 0;
                listenForAckSequence = 0;
            }
        }
        return;
    }
    
    // ACK
    if (IPAddress(gf.lh.dstAddr) != IPAddress(broadcastAddr))
        LoRaNetSwitch.acknowledge(gf.lh.srcAddr, gf.lh.sequence);

    // Upper Layers Handover
    if (gf.lh.type == DATA) 
        LoRaNet.handlePacket(packetSize - sizeof(gf.lh));
    else if (gf.lh.type == ROUTING) 
        LoRaNetRouter.handlePacket(packetSize - sizeof(gf.lh), gf.lh.srcAddr);
    else 
        LOG("LoRaNetSwitch::onReceive: Message type is unknown! Discarding...\n");
}

LoRaNetSwitchClass::LoRaNetSwitchClass()
{
    ;
}



int
LoRaNetSwitchClass::begin(uint8_t *addrp)
{
    LOG("LoRaNetSwitch::begin: LoRaNet Switch initializing...\n");
    memcpy(thisNodeAddr, addrp, sizeof(thisNodeAddr));
    LoRa.onReceive(onReceive);
    LoRa.receive();
    LoRa.setSpreadingFactor(DEFAULT_SF);
    LOG("LoRaNetSwitch::begin: LoRaNet Switch successfully initialized\n");
}

int 
LoRaNetSwitchClass::push(genericFrame gf, uint8_t type)
{
    // TODO: should we handle frames sent to this same node?
    queueItem qi;
    qi.type = type;
    qi.gf = gf;
    qi.lastSentAt = 0;
    qi.retries = -1;

    if (type == CONTROL) qi.headerFilled = true;
    else qi.headerFilled = false;

    LOG("LoRaNetSwitch::push: Adding new frame to frameQueue\n");
    list_append(frameQueue, &qi, sizeof(qi));
}

void 
LoRaNetSwitchClass::acknowledge(uint8_t *dstAddr, uint8_t sequence)
{
    LOG("LoRaNetSwitch::acknowledge: Sending back ACK\n");
    genericFrame ack;
    ack.lh.version = 0;
    ack.lh.type = CONTROL;
    ack.lh.subtype = CONTROL_ACK;
    ack.lh.sequence = sequence;

    memcpy(ack.lh.srcAddr, thisNodeAddr, 4);
    memcpy(ack.lh.dstAddr, dstAddr, 4);

    LoRaNetSwitch.push(ack, CONTROL); 
}

uint8_t 
LoRaNetSwitchClass::getNextSequence(uint8_t *address)
{
    uint8_t dst[4];
    memcpy(dst, address, 4);
    char *key = (char *) IPAddress(dst).toString().c_str();
    neighbor *n = (neighbor *) ht_get(neighbors, key);
    // New entry
    if (n == NULL) {
        uint8_t tmp = random(1, 255);
        n = (neighbor *) malloc(sizeof(neighbor));
        memcpy(n -> address, dst, 4);
        n -> sequence = tmp;
        n -> rssi = 0;
        ht_set(neighbors, (char *) IPAddress(dst).toString().c_str(), n);
    }
    // Increment existing sequence number
    else n -> sequence += 1;
    LOG("LoRaNetSwitch::getNextSequence: Sequence number for neighbor %s is %d\n",
        IPAddress(address).toString().c_str(), n -> sequence);
    return n -> sequence;
}

int
LoRaNetSwitchClass::handleQueueItem(queueItem *qi, uint8_t type, int qIndex)
{
    // Header configuration
    if (qi -> headerFilled == false) {
        uint8_t dstAddr[4];
        uint8_t *nextHop;
        if (type == DATA) 
            memcpy(dstAddr, qi -> gf.nh.dstAddr, 4);
        else if (type == ROUTING && qi -> gf.ah.type == AODV_RREQ)
            memcpy(dstAddr, broadcastAddr, 4);
        else if (type == ROUTING && qi -> gf.ah.type == AODV_RREP)
            memcpy(dstAddr, qi -> gf.ah.rrep.originAddr, 4);
        // else if (type == ROUTING && qi -> gf.ah.type == AODV_RERR)
        //     memcpy(dstAddr, broadcastAddr, 4);
        // else if (type == ROUTING && qi -> gf.ah.type == AODV_RREP_ACK)
        //     memcpy(dstAddr, qi -> gf.ah.rrepAck.dstAddr, 4);

        LoRaNetRouter.getNextHop((uint8_t *) &dstAddr, &nextHop);
        
        // We don't know the next hop for the given destination
        if (nextHop == NULL) {
            LoRaNetRouter.createRREQ(dstAddr);
            return SUCCESS;
        }
        // We know the next hop for the given destination
        else {
            qi -> gf.lh.version = 0;
            qi -> gf.lh.type = type;
            qi -> gf.lh.subtype = 0;
            qi -> gf.lh.sequence = getNextSequence(nextHop);
            memcpy(qi -> gf.lh.dstAddr, nextHop, 4);
            memcpy(qi -> gf.lh.srcAddr, thisNodeAddr, 4);
        }
        qi -> headerFilled = true;
    }

    LOG("LoRaNetSwitch::handleQueueItem: Sending frame type %d to destination %s, sequence %d\n", type, 
        IPAddress(qi -> gf.lh.dstAddr).toString().c_str(), qi -> gf.lh.sequence);
    // Frame preparation
    uint8_t buffer[LORA_MAX_LEN];
    size_t bytesWritten = 0;
    memcpy(buffer + bytesWritten, &qi -> gf.lh, sizeof(linkHeader));
    bytesWritten += sizeof(linkHeader);

    if (type == DATA) {
        memcpy(buffer + bytesWritten, &qi -> gf.nh, sizeof(networkHeader));
        bytesWritten += sizeof(networkHeader);
        memcpy(buffer + bytesWritten, qi -> gf.payload, qi -> gf.nh.len);
        bytesWritten += qi -> gf.nh.len;
    }
    else if (type == ROUTING) {
        memcpy(buffer + bytesWritten, &qi -> gf.ah.type, sizeof(1));
        bytesWritten += sizeof(qi -> gf.ah.type);
        if (qi -> gf.ah.type == AODV_RREQ) {
            memcpy(buffer + bytesWritten, &qi -> gf.ah.rreq, sizeof(rreqHeader));
            bytesWritten += sizeof(qi -> gf.ah.rreq);
        }
        else if (qi -> gf.ah.type == AODV_RREP) {
            memcpy(buffer + bytesWritten, &qi -> gf.ah.rrep, sizeof(rrepHeader));
            bytesWritten += sizeof(qi -> gf.ah.rrep);
        }
        else if (qi -> gf.ah.type == AODV_RERR) {
            memcpy(buffer + bytesWritten, &qi -> gf.ah.rerr, sizeof(rerrHeader));
            bytesWritten += sizeof(qi -> gf.ah.rerr);
        }
        else if (qi -> gf.ah.type == AODV_RREP_ACK) {
            memcpy(buffer + bytesWritten, &qi -> gf.ah.rrepAck, sizeof(rrepAckHeader));
            bytesWritten += sizeof(qi -> gf.ah.rrepAck);
        }
    }

    // TODO: mutex aqui? ou desligar interrupts
    LOG("LoRaNetSwitch::handleQueueItem: writing to LoRa interface...\n");
    if (!LoRa.beginPacket()) return FAIL;
    LoRa.write(buffer, bytesWritten);
    if (!LoRa.endPacket()) return FAIL;
    LOG("LoRaNetSwitch::handleQueueItem: finished writing\n");

    // Cleanup 
    // TODO: don't listen for ACKs for broadcasts
    if (type == DATA || type == ROUTING) {
        qi -> lastSentAt = millis();
        qi -> retries++;
        listenForAckTS = millis();
        listenForAckSequence = qi -> gf.lh.sequence;
        LOG("LoRaNetSwitch::handleQueueItem: listening for ACK...\n");

    }
    else if (type == CONTROL) {
        if (qi -> gf.lh.subtype == CONTROL_ACK)
            list_del(frameQueue, qIndex);
    }
    // Broadcast frames should not be retransmitted
    if (IPAddress(qi -> gf.lh.dstAddr) == IPAddress(broadcastAddr)) 
        list_del(frameQueue, qIndex);

    LOG("LoRaNetSwitch::handleQueueItem: putting LoRa interface in receive mode...\n");
    LoRa.receive();
}

void
LoRaNetSwitchClass::run()
{
    for (int i = 0 ; i < list_length(frameQueue); i++) {
        // LOG("LoRaNetSwitch::run: run %d\n", i);
        unsigned long now = millis();
        // If we just sent a DATA or ROUTING message, we must listen for ACK
        if (listenForAckTS > 0) {
            if (now - listenForAckTS < ACK_LISTEN_INTERVAL) return;
            else {
                LOG("LoRaNetSwitch::run: Frame sequence number %d, ACK not received\n", 
                    listenForAckSequence);
                listenForAckTS = 0;
                listenForAckSequence = 0;
            }
        }

        queueItem *qi = (queueItem *) list_get(frameQueue, i);
        if (qi == NULL) continue;

        if (qi -> retries >= MAX_RETRIES) {
            LOG("LoRaNetSwitch::run: Frame to destination %s, sequence %d, exceeded max retries\n", 
                IPAddress(qi -> gf.lh.dstAddr).toString().c_str(), qi -> gf.lh.sequence);
            list_del(frameQueue, i); 
        }
        else if (now - qi -> lastSentAt > MAX_ACK_INTERVAL) 
            LoRaNetSwitch.handleQueueItem(qi, qi -> type, i);
    }
}

LoRaNetSwitchClass LoRaNetSwitch;
