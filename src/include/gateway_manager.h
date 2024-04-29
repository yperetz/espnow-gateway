#ifndef GATEWAY_GATEWAY_MANAGER_H
#define GATEWAY_GATEWAY_MANAGER_H

#include "gateway_fsm.h"
#include "single_use.h"

#include "etl/enum_type.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#define ETL_ENUM_TYPE_DEFAULT(ITEM) ETL_ENUM_TYPE(ITEM, #ITEM)

namespace Gateway {
static constexpr uint16_t MAIN_TASK_STACK_SIZE = 4096;

/* Device Events */
struct eEvent
{
    enum enum_type
    {
        Periodic,
        DataReceived
    };

    ETL_DECLARE_ENUM_TYPE(eEvent, uint8_t)
    ETL_ENUM_TYPE_DEFAULT(Periodic)
    ETL_ENUM_TYPE_DEFAULT(DataReceived)
    ETL_END_ENUM_TYPE
};

struct EventPeriodic : public etl::message<eEvent::Periodic>
{
    const char *name = "PeriodicEvent";
};

struct EventDataReceived : public etl::message<eEvent::DataReceived>
{
    EventDataReceived(int i_) :
        i(i_) {
    }

    const char *name = "DataReceivedEvent";
    int i;
};

/* State declarations */
struct eState
{
    enum enum_type
    {
        SETUP,
        MAIN,

        NUM
    };

    ETL_DECLARE_ENUM_TYPE(eState, uint8_t)
    ETL_ENUM_TYPE_DEFAULT(SETUP)
    ETL_ENUM_TYPE_DEFAULT(MAIN)
    ETL_ENUM_TYPE(NUM, "unknown")
    ETL_END_ENUM_TYPE
};

typedef etl::message_packet<EventPeriodic, EventDataReceived> MessagePacket;

/**
 * @brief Manager class for the gateway
 */
class GatewayManager : public SingleUse<GatewayManager>,
                       public GatewayFsm<GatewayManager, MAIN_TASK_STACK_SIZE>
{
    static constexpr UBaseType_t TASK_PRIORITY = 3;
    static constexpr time_t PERIODIC_MS = 3000;
    static constexpr uint16_t TIMER_BLOCK_TIMEOUT_MS = 1000;

    public:
    /**
     * @brief Getting the single instance of GatewayManager
     *
     * @return The instance of GatewayManager
     */
    static GatewayManager &GetInstance();

    /**
     * @brief The main function that will be called by the FreeRTOS task
     *
     * @param pvParameters - Unused, follows the FreeRTOS task definition
     */
    static void TaskFunction(void *pvParameters);

    /**
     * @brief Operations to perform when an event is received
     *
     * @param state - Current state
     * @param event - Event received
     */
    static void EventEntry(eState state, eEvent event);

    /**
     * @brief Operations to perform when entering a state
     *
     * @param state - State that is being entered
     */
    static void StateEntry(eState state);

    /**
     * @brief Operations to perform when exiting a state
     *
     * @param state - State that is being exited
     */
    static void StateExit(eState state);

    /* overriden fsm functions */
    void receive(const etl::imessage &msg_) override;
    void process_queue() override;

    private:
    // ctor
    GatewayManager();

    static void periodicCb(TimerHandle_t timer);

    void init();
    void eventHandlerLoop();

    TimerHandle_t m_periodicTimer;
    StaticTimer_t m_periodicTimerBuffer{};
    etl::ifsm_state *m_stateList[eState::NUM];
    etl::queue<MessagePacket, 10> m_queue;
};

using GwFsm = GatewayFsm<GatewayManager, MAIN_TASK_STACK_SIZE>;

/* Device State */
class SetupState : public etl::fsm_state<GwFsm, SetupState, eState::SETUP, EventPeriodic, EventDataReceived>
{
    public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;
    etl::fsm_state_id_t on_event(const EventPeriodic &event);
    etl::fsm_state_id_t on_event(const EventDataReceived &data);
    etl::fsm_state_id_t on_event_unknown(const etl::imessage &msg);
    static SetupState &GetInstance();
};

class MainState : public etl::fsm_state<GwFsm, MainState, eState::MAIN, EventPeriodic, EventDataReceived>
{
    public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;
    etl::fsm_state_id_t on_event(const EventPeriodic &event);
    etl::fsm_state_id_t on_event(const EventDataReceived &data);
    etl::fsm_state_id_t on_event_unknown(const etl::imessage &msg);
    static MainState &GetInstance();
};

}  // namespace Gateway

#endif  // GATEWAY_GATEWAY_MANAGER_H