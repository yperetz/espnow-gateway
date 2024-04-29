#ifndef GATEWAY_GATEWAY_FSM_H
#define GATEWAY_GATEWAY_FSM_H

#include "etl/fsm.h"
#include "etl/message_packet.h"
#include "etl/queue.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace Gateway {

/**
 * @brief An instance counter for the gateway FSM.
 *
 * This is used to ensure keeping track of FSM indices
 *
 */
class GatewayBaseFsm
{
    public:
    GatewayBaseFsm() {
        s_countInstances++;
    }

    protected:
    static uint8_t s_countInstances;
};

/**
 * @brief A Wrapper for etl::fsm that includes a task
 * Derived class must implement static void TaskFunction(void *pvParameters),
 * Events, states and the virtual functions declared here
 *
 * @tparam Derived - Derived class
 * @tparam stackSize - Task stack size
 */
template <typename Derived, uint32_t stackSize>
class GatewayFsm : public GatewayBaseFsm, public etl::fsm
{
    public:
    GatewayFsm(const char *name, uint32_t priority, BaseType_t coreId = 0) :
        fsm(this->s_countInstances) {
        Derived *derived = static_cast<Derived *>(this);
        m_taskHandle = xTaskCreateStaticPinnedToCore(
            derived->TaskFunction, name, stackSize, nullptr, priority, m_taskBuffer, &m_taskTcb, coreId);
    }

    /* overriden from etl::fsm */

    /*
    Since the Events are defined in the derived class, influencing messagePacket and queue,
    this function should be overridden by the derived class.
    e.g:

    void GatewayFsm<Derived, stackSize>::receive(const etl::imessage &msg_) {
        if (accepts(msg_)) {
            // Place in queue.
            queue.emplace(msg_);
            ESP_LOGD(MODULE_TAG, "Queueing message %d"), int(msg_.get_message_id());
        } else {
            ESP_LOGW(MODULE_TAG, "Ignoring message %d"), int(msg_.get_message_id());
        }
    }
    */
    virtual void receive(const etl::imessage &msg_) override {
        Derived *derived = static_cast<Derived *>(this);
        derived->receive(msg_);
    }

    /*
    For the same reason as the receive function, the send function should be overridden
    e.g:

    void GatewayFsm<Derived, stackSize>::process_queue() {
        while (!queue.empty())
        {
          message_packet& packet = queue.front();
          etl::imessage& msg = packet.get();
          std::cout << "Processing message " << int(msg.get_message_id()) << std::endl;

          // Call the base class's receive function.
          // This will route it to the correct 'on_event' handler.
          etl::fsm::receive(msg);

          queue.pop();
        }
    */
    virtual void process_queue() {
        Derived *derived = static_cast<Derived *>(this);
        derived->process_queue();
    }

    private:
    static uint8_t s_nextId;

    StackType_t m_taskBuffer[stackSize];
    StaticTask_t m_taskTcb;
    TaskHandle_t m_taskHandle;
};

}  // namespace Gateway

#endif  // GATEWAY_GATEWAY_FSM_H