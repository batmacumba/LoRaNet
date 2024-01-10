/*
    LoRaNet.c
    Created by Bruno Carneiro
    Released into the public domain
*/
#include "LoRaNet.h"

struct loranet_s {
    loranet_options_t *options;
};

bool
loranet_init(loranet_t *loranet, loranet_options_t *options)
{
    if (loranet == NULL || options == NULL) {
        return false;
    }
    loranet -> options = options; 
    // TODO: set up router and switch
    // TODO: set up phy
    return true;
}

size_t
loranet_write(loranet_t *loranet, const char *dst_addr, const uint8_t *buffer, size_t buffer_size)
{
    if (loranet == NULL || dst_addr == NULL || buffer == NULL) {
        return -1;
    }
    // TODO: check if dst_addr is valid
    // TODO: check if bytes_to_write is valid (not too big || equal to 0)
    // TODO: copy and enqueue message to be sent by router/switch
    // TODO: break message into smaller chunks if necessary
}

size_t
loranet_read(loranet_t *loranet, void *buffer, size_t buffer_size)
{
    if (loranet == NULL || buffer == NULL) {
        return -1;
    }
    // TODO: check if buffer_size is valid (not too big || equal to 0)
    // TODO: copy and dequeue message received by router/switch
}