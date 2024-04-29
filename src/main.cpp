#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace Gateway {
extern "C" void app_main();
void app_main() {

  static const char *TAG = "MAIN";
  for (;;) {
    ESP_LOGI("MAIN", "[APP] IDF version: %s", esp_get_idf_version());
    ESP_LOGD(TAG, "HELLO WORLD D");
    ESP_LOGI(TAG, "HELLO WORLD I");
    ESP_LOGW(TAG, "HELLO WORLD W");
    ESP_LOGE(TAG, "HELLO WORLD E");
    vTaskDelay(100);
  }
}

}  // namespace Gateway
