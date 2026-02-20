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

VSomeIPClient::VSomeIPClient(QObject *parent)
    : IVehicleDataProvider(parent)
    , m_speed(0.0f)
    , m_gear(GearState::P)
    , m_batteryVoltage(7.8f)
    , m_batteryPercent(85.0f)
    , m_connected(false)
{
    // TODO: vsomeip init, subscribe
}

float VSomeIPClient::speed() const { return m_speed; }
GearState VSomeIPClient::gear() const { return m_gear; }
float VSomeIPClient::batteryVoltage() const { return m_batteryVoltage; }
float VSomeIPClient::batteryPercent() const { return m_batteryPercent; }
bool VSomeIPClient::isConnected() const { return m_connected; }

void VSomeIPClient::publishGear(GearState gear)
{
    // TODO: vsomeip notify 0x8002
    m_gear = gear;
}
