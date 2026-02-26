/**
 * @file VSomeIPClient.cpp
 * @brief TODO: Integrate VSOMEIP library
 * - vsomeip::runtime::get()->create_application()
 * - request_service(0x1234, 0x0001)
 * - subscribe(0x8001, 0x8002, 0x8003)
 * - notify(0x8002) on gear touch
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "VSomeIPClient.h"

#ifdef HU_HAS_VSOMEIP
#include <vector>
#endif

#ifdef HU_HAS_VSOMEIP
namespace {
constexpr vsomeip::service_t kServiceId = 0x1234;
constexpr vsomeip::instance_t kInstanceId = 0x0001;
constexpr vsomeip::method_t kGearMethodId = 0x8002;
}
#endif

VSomeIPClient::VSomeIPClient(QObject *parent)
    : IVehicleDataProvider(parent)
    , m_speed(0.0f)
    , m_gear(GearState::P)
    , m_batteryVoltage(7.8f)
    , m_batteryPercent(85.0f)
    , m_connected(false)
{
#ifdef HU_HAS_VSOMEIP
    m_app = vsomeip::runtime::get()->create_application("HeadUnit");
    if (!m_app) {
        return;
    }
    m_app->register_state_handler([this](vsomeip::state_type_e state) {
        onState(state);
    });
    m_running = true;
    m_worker = std::thread([this]() {
        if (!m_app->init()) {
            return;
        }
        m_app->start();
    });
#endif
}

VSomeIPClient::~VSomeIPClient()
{
#ifdef HU_HAS_VSOMEIP
    m_running = false;
    if (m_app) {
        m_app->stop();
    }
    if (m_worker.joinable()) {
        m_worker.join();
    }
#endif
}

float VSomeIPClient::speed() const { return m_speed; }
GearState VSomeIPClient::gear() const { return m_gear; }
float VSomeIPClient::batteryVoltage() const { return m_batteryVoltage; }
float VSomeIPClient::batteryPercent() const { return m_batteryPercent; }
bool VSomeIPClient::isConnected() const { return m_connected; }

void VSomeIPClient::publishGear(GearState gear)
{
    m_gear = gear;
#ifdef HU_HAS_VSOMEIP
    if (!m_app || !m_registered.load()) {
        return;
    }
    auto payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> data(1);
    data[0] = static_cast<vsomeip::byte_t>(gear);
    payload->set_data(data);

    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(kServiceId);
    request->set_instance(kInstanceId);
    request->set_method(kGearMethodId);
    request->set_payload(payload);
    m_app->send(request);
#endif
}

#ifdef HU_HAS_VSOMEIP
void VSomeIPClient::onState(vsomeip::state_type_e state)
{
    if (!m_app) {
        return;
    }
    if (state == vsomeip::state_type_e::ST_REGISTERED) {
        m_registered = true;
        m_connected = true;
        m_app->request_service(kServiceId, kInstanceId);
        return;
    }
    m_registered = false;
    m_connected = false;
}
#endif
