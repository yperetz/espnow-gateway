#include "gateway_fsm.h"

#include "esp_log.h"
#define MODULE_TAG "GatewayFsm"

namespace Gateway {

uint8_t GatewayBaseFsm::s_countInstances = 0;  // Initialize static counter

}  // namespace Gateway