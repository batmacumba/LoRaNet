/*
    LoRaNet.h
    Created by Bruno Carneiro
    Released into the public domain
*/

#ifndef LORANET_H
#define LORANET_H

#include "hdr/util.h"

typedef struct loranet_options_s {
    void *phy_options;
    void (*callback) (void *buffer, size_t bytes_read);
} loranet_options_t;

typedef struct loranet_s {
    loranet_options_t *options;
} loranet_t;

bool    loranet_init        (loranet_t *loranet, loranet_options_t *options);
size_t  loranet_write       (loranet_t *loranet, const char *dst_addr, const uint8_t *buffer, size_t buffer_size);
size_t  loranet_read        (loranet_t *loranet, void *buffer, size_t buffer_size);

#endif // LORANET_H
