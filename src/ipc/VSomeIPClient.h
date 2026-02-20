/**
 * @file VSomeIPClient.h
 * @brief VSOMEIP implementation - TODO: implement by Sonnet/GPT
 * Replace MockVehicleDataProvider in MainWindow when ready.
 * Architecture: docs/ARCHITECTURE_DESIGN.md §4
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#ifndef VSOMEIPCLIENT_H
#define VSOMEIPCLIENT_H

#include "IVehicleDataProvider.h"

/**
 * @class VSomeIPClient
 * @brief Subscribe: 0x8001 speed, 0x8002 gear, 0x8003 battery
 *        Publish: 0x8002 gear (on touch)
 * Service: 0x1234, Instance: 0x0001
 * Config: /etc/vsomeip/vsomeip_headunit.json
 */
class VSomeIPClient : public IVehicleDataProvider
{
    Q_OBJECT

public:
    explicit VSomeIPClient(QObject *parent = nullptr);

    float speed() const override;
    GearState gear() const override;
    float batteryVoltage() const override;
    float batteryPercent() const override;
    bool isConnected() const override;

    void publishGear(GearState gear);

private:
    float m_speed;
    GearState m_gear;
    float m_batteryVoltage;
    float m_batteryPercent;
    bool m_connected;
};

#endif // VSOMEIPCLIENT_H
