#ifndef IP_ADDRESS_H
#define IP_ADDRESS_H

#define PREFIX 		10

void
generateRandomIP(uint8_t *ip)
{
	ip[0] = PREFIX;
	ip[1] = (uint8_t) random(1, 254);
	ip[2] = (uint8_t) random(1, 254);
	ip[3] = (uint8_t) random(1, 254);
}

#endif	/* IP_ADDRESS_H */