/*
    LoRaNet.h
    Created by Bruno Carneiro
    Released into the public domain
*/

#ifndef LORANET_H
#define LORANET_H

#include <stdlib.h>

typedef struct loranet_s loranet_t;
typedef struct loranet_options_s {
    void *phy_options;
    void (*callback) (void *buffer, size_t bytes_read);
} loranet_options;

uint8_t loranet_init        (loranet_options *options);
uint8_t loranet_connect     (const char *dst_addr);
size_t  loranet_write       (const uint8_t *buffer, size_t bytes_to_write);
size_t  loranet_read        (void *buffer, size_t bytes_to_read);

#endif // LORANET_H
