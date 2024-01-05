/*
  eeprom.h
  Created by Bruno Carneiro
  Released into the public domain
*/

#ifndef EEPROM_H
#define EEPROM_H

#include <EEPROM.h>
#include "debug.h"
#include "ipAddress.h" 

#define EEPROM_SIZE		4096
#define EEPROM_FLAG		"LoRaMe2h"
#define FLAG_LENGTH		9

typedef struct {
	char flag[FLAG_LENGTH] = EEPROM_FLAG;
	uint8_t ip[4];
} eepromData;

bool
eepromConfigured()
{
	// TODO: error checking?
    bool configured = false;
    eepromData ed;
    EEPROM.get(EEPROM_SIZE - sizeof(eepromData), ed);
    if (strncmp(ed.flag, EEPROM_FLAG, FLAG_LENGTH) == 0) {
    	LOG("eepromConfigured: EEPROM already configured!\n");
    	configured = true;
    }
    else
    	LOG("eepromConfigured: EEPROM not configured!\n");
    return configured;
}

void
eepromRead(uint8_t *ipp)
{
	// TODO: error checking?
	eepromData ed;
    EEPROM.get(EEPROM_SIZE - sizeof(eepromData), ed);
    LOG("eepromRead(): IP address read from EEPROM: %s\n", IPAddress(ed.ip).toString().c_str());
    memcpy(ipp, ed.ip, 4);
}

void
eepromSetup(uint8_t *ipp)
{
    // TODO: error checking?
	LOG("eepromSetup: Setting up EEPROM\n");
	eepromData ed;
	generateRandomIP(ed.ip);
	LOG("eepromSetup(): new IP address generated: %s\n", IPAddress(ed.ip).toString().c_str());
	EEPROM.put(EEPROM_SIZE - sizeof(eepromData), ed);
    EEPROM.commit();
    LOG("eepromSetup: Finished configuring EEPROM!\n");
    memcpy(ipp, ed.ip, 4);
}

#endif	/* EEPROM_H */