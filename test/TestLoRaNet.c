#include <stdio.h>
#include "unity/src/unity.h"
#include "LoRaNet.h"

void 
setUp()
{
    
}

void 
tearDown()
{
    
}

void 
test_loranet_Initialization_Works(void)
{
    TEST_ASSERT_EQUAL(0, loranet_init(NULL));
}

void test_loranet_Initialization_Works(void);

int
main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_loranet_Initialization_Works);
    return UNITY_END();
}

