/*
    LoRaNet.c
    Created by Bruno Carneiro
    Released into the public domain
*/

#include <stdlib.h>
#include "LoRaNet.h"

struct loranet_s {
    loranet_options *options;
};

static loranet_t loranet;

uint8_t
loranet_init(loranet_options *options) 
{
    loranet.options = options; 
    return 0;
}