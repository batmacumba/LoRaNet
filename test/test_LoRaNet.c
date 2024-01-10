#include "unity.h"
#include "LoRaNet.h"

void 
setUp(void)
{
    ;
}

void 
tearDown(void)
{
    ;
}

void 
test_loranet_Initialization_Works(void)
{
    loranet_t loranet;
    loranet_options_t options;
    TEST_ASSERT_TRUE(loranet_init(loranet, options));
}
