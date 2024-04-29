#include "gateway_manager.h"
#include "gateway_fsm.h"

#include "freertos/task.h"

#define MODULE_TAG "GatewayManager"

namespace Gateway {

GatewayManager::GatewayManager() :
    GatewayFsm<GatewayManager, MAIN_TASK_STACK_SIZE>("GatewayManager", TASK_PRIORITY) {}

GatewayManager &GatewayManager::GetInstance() {
    static GatewayManager instance;

    return instance;
}

void GatewayManager::TaskFunction(void *pvParameters) {
    GatewayManager &manager = GetInstance();

    manager.init();
    ESP_LOGI(MODULE_TAG, "Gateway Manager init completed");

    for (;;) {
        manager.eventHandlerLoop();
    }
}

void GatewayManager::EventEntry(eState state, eEvent event) {
    ESP_LOGD(MODULE_TAG, "Entered Event: state %s, event %s", state.c_str(), event.c_str());
}

void GatewayManager::StateEntry(eState state) {
    ESP_LOGD(MODULE_TAG, "Entered State: %s", state.c_str());
}

void GatewayManager::StateExit(eState state) {
    ESP_LOGD(MODULE_TAG, "Exited State: %s", state.c_str());
}

void GatewayManager::receive(const etl::imessage &msg_) {
    if (accepts(msg_)) {
        // Place in queue.
        ESP_LOGV(MODULE_TAG, "Queueing message %d", int(msg_.get_message_id()));
        m_queue.emplace(msg_);
    } else {
        ESP_LOGW(MODULE_TAG, "Ignoring message %d", int(msg_.get_message_id()));
    }
}

void GatewayManager::process_queue() {
    while (!m_queue.empty()) {
        MessagePacket &packet = m_queue.front();
        etl::imessage &msg = packet.get();
        ESP_LOGV(MODULE_TAG, "Processing message %d", int(msg.get_message_id()));

        // Call the base class's receive function.
        // This will route it to the correct 'on_event' handler.
        etl::fsm::receive(msg);

        m_queue.pop();
    }
}

void GatewayManager::periodicCb(TimerHandle_t timer) {
    static EventPeriodic event{};

    etl::send_message(GatewayManager::GetInstance(), event);
}

void GatewayManager::init() {
    constexpr bool isIntervalValid = (PERIODIC_MS > 0 &&
                                      (PERIODIC_MS * configTICK_RATE_HZ) % 1000 == 0);

    static_assert(isIntervalValid, "Periodic must represent of natural number of ticks");

    m_periodicTimer = xTimerCreateStatic("PeriodicTimer",
                                         pdMS_TO_TICKS(PERIODIC_MS),
                                         pdTRUE,
                                         this,
                                         periodicCb,
                                         &m_periodicTimerBuffer);

    // FSM config
    etl::ifsm_state *stateList[] = {&SetupState::GetInstance(), &MainState::GetInstance()};
    memcpy(m_stateList, stateList, sizeof(stateList));
    set_states(m_stateList, eState::NUM);
    start();  // Start FSM

    configASSERT(xTimerStart(m_periodicTimer, pdMS_TO_TICKS(TIMER_BLOCK_TIMEOUT_MS)) == pdTRUE);
}

void GatewayManager::eventHandlerLoop() {
    process_queue();
    vTaskDelay(1);
}

etl::fsm_state_id_t SetupState::on_enter_state() {
    GatewayManager::GetInstance().StateEntry(eState::SETUP);
    return eState::SETUP;
}

void SetupState::on_exit_state() {
    GatewayManager::GetInstance().StateExit(eState::SETUP);
}

etl::fsm_state_id_t SetupState::on_event(const EventPeriodic &event) {
    GatewayManager::GetInstance().EventEntry(eState::SETUP, eEvent::Periodic);

    static int8_t count{};
    if (count < 4) {
        count++;

        return eState::SETUP;
    }

    return eState::MAIN;
}

etl::fsm_state_id_t SetupState::on_event(const EventDataReceived &data) {
    GatewayManager::GetInstance().EventEntry(eState::SETUP, eEvent::DataReceived);
    return eState::SETUP;
}

etl::fsm_state_id_t SetupState::on_event_unknown(const etl::imessage &msg) {
    ESP_LOGW(MODULE_TAG, "State Setup, Event Unknown");
    return eState::SETUP;
}

SetupState &SetupState::GetInstance() {
    static SetupState instance;

    return instance;
}

etl::fsm_state_id_t MainState::on_enter_state() {
    GatewayManager::GetInstance().StateEntry(eState::MAIN);

    return eState::MAIN;
}

void MainState::on_exit_state() {
    GatewayManager::GetInstance().StateExit(eState::MAIN);
}

etl::fsm_state_id_t MainState::on_event(const EventPeriodic &event) {
    GatewayManager::GetInstance().EventEntry(eState::MAIN, eEvent::Periodic);

    return eState::MAIN;
}

etl::fsm_state_id_t MainState::on_event(const EventDataReceived &data) {
    GatewayManager::GetInstance().EventEntry(eState::MAIN, eEvent::DataReceived);

    return eState::MAIN;
}

etl::fsm_state_id_t MainState::on_event_unknown(const etl::imessage &msg) {
    ESP_LOGW(MODULE_TAG, "State Main, Event Unknown");

    return eState::MAIN;
}

MainState &MainState::GetInstance() {
    static MainState instance;

    return instance;
}

}  // namespace Gateway
