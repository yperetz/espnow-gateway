#include "gateway_manager.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MODULE_TAG "GatewayMain"

namespace Gateway {
extern "C" void app_main();
void app_main() {
    GatewayManager &gatewayManager = GatewayManager::GetInstance();
    (void)gatewayManager;  // Suppresses unused variable
    ESP_LOGI(MODULE_TAG, "GatewayManager created");

    vTaskSuspend(NULL);
}

}  // namespace Gateway
