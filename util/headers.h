/*
  headers.h
  Created by Bruno Carneiro
  Released into the public domain
*/

#ifndef HEADERS_H
#define HEADERS_H

#define     INITIAL_TTL       64
// TODO: subtract LORA_MAX_LEN from headers to obtain new PAYLOAD_MAX_LEN
// #define     PAYLOAD_MAX_LEN   128
#define     PAYLOAD_MAX_LEN   229
#define     LORA_MAX_LEN   256
// headder maz length 27 so payload max length = 256-27 = 229

#define     CONTROL           1
#define     DATA              2
#define     ROUTING           3

#define     CONTROL_ACK       1  

/* AODV Message types */
#define AODV_HELLO    0     
#define AODV_RREQ     1
#define AODV_RREP     2
#define AODV_RERR     3
#define AODV_RREP_ACK 4

// TODO: make the headers as small as possible
typedef struct {
    uint8_t unknownSequence : 1;
    uint8_t hopCount;
    uint8_t rreqID;
    uint8_t dstAddr[4];
    uint8_t dstSequence;
    uint8_t originAddr[4];
    uint8_t originSequence;
} rreqHeader;

typedef struct {
    uint8_t hopCount;
    uint8_t dstAddr[4];
    uint8_t dstSequence;
    uint8_t originAddr[4];
} rrepHeader;

typedef struct {
    uint8_t unreachableDstAddr[4];
    uint8_t unreachableDstSequence;
} rerrHeader;

typedef struct {
    uint8_t dstAddr[4];
    uint8_t originAddr[4];
} rrepAckHeader;

typedef struct {
    uint8_t type;
    rreqHeader rreq;
    rrepHeader rrep;
    rerrHeader rerr;
    rrepAckHeader rrepAck;
} aodvHeader;

typedef struct {
	uint8_t version : 4;
    uint8_t len;
    uint8_t ttl;
    uint8_t srcAddr[4];
    uint8_t dstAddr[4];
} networkHeader;

typedef struct {
	uint8_t version : 2;
    uint8_t type : 2;
    uint8_t subtype : 4;
    uint8_t sequence;
    uint8_t srcAddr[4];
    uint8_t dstAddr[4];
} linkHeader;

typedef struct {
    linkHeader lh;
    networkHeader nh;
    aodvHeader ah;
    uint8_t payload[PAYLOAD_MAX_LEN];
} genericFrame;

#endif	/* HEADERS_H */


