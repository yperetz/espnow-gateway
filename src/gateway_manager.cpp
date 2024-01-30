#include "gateway_manager.h"

#include "freertos/task.h"

namespace Gateway {
GatewayManager &GatewayManager::GetInstance() {
    static GatewayManager s_instance;

    return s_instance;
}

void GatewayManager::Init() {

    m_eventHandle = xEventGroupCreateStatic(&m_eventGroupBuffer);

    m_eventTaskHandle =
        xTaskCreateStaticPinnedToCore(GatewayManager::eventHandler,
                                      "GatewayManager",
                                      TASK_STACK_SIZE,
                                      (void *)this,
                                      TASK_PRIORITY,
                                      m_eventTaskStack,
                                      &m_eventTaskBuffer,
                                      (BaseType_t)0);
}

void GatewayManager::eventHandler(void *args) {
    GatewayManager *manager = (GatewayManager *)args;
    for (;;) {
        manager->eventHandlerLoop();
    }
}

void GatewayManager::eventHandlerLoop() {
    EventBits_t bits = xEventGroupWaitBits(m_eventHandle,
                                           eTaskEvents::TASK_EVENT_ALL,
                                           pdTRUE,
                                           pdTRUE,
                                           portMAX_DELAY);

}

} // namespace Gateway
