#ifndef GATEWAY_GATEWAY_MANAGER_H
#define GATEWAY_GATEWAY_MANAGER_H

#include "single_use.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

namespace Gateway {

class GatewayManager : SingleUse<GatewayManager> {
  public:
    static constexpr uint16_t TASK_STACK_SIZE = 4096;
    static constexpr UBaseType_t TASK_PRIORITY = 3;

    enum eTaskEvents : EventBits_t {
        TASK_EVENT_START = BIT(0),
        TASK_EVENT_STOP = BIT(1),
        TASK_EVENT_ALL = BIT(2) - 1
    };

    /**
     * @brief Getting the single instance of GatewayManager
     *
     * @return The instance of GatewayManager
     */
    static GatewayManager &
    GetInstance();

    void Init();

  private:
    static void eventHandler(void *args);

    void eventHandlerLoop();

    EventGroupHandle_t m_eventHandle;
    StaticEventGroup_t m_eventGroupBuffer;

    TaskHandle_t m_eventTaskHandle;
    StaticTask_t m_eventTaskBuffer;
    StackType_t m_eventTaskStack[TASK_STACK_SIZE];
};

} // namespace Gateway

#endif // GATEWAY_GATEWAY_MANAGER_H