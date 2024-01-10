#include "src/LoRaNet.h"
#include "/Users/batmacumba/.gem/ruby/2.6.0/gems/ceedling-0.31.1/vendor/unity/src/unity.h"


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

    do {if ((loranet_init(loranet, options))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(21)));}} while(0);

}
