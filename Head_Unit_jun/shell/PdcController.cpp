#include "PdcController.h"

#include "IPdcSensorProvider.h"

#include <QDebug>
#include <limits>

PdcController::PdcController(IPdcSensorProvider *provider, QObject *parent)
    : QObject(parent)
    , m_provider(provider)
{
    qRegisterMetaType<PdcState>("PdcState");
    qRegisterMetaType<QVector<PdcSensorReading>>("QVector<PdcSensorReading>");

    m_staleTimer.setSingleShot(true);
    m_staleTimer.setInterval(kStaleTimeoutMs);
    connect(&m_staleTimer, &QTimer::timeout, this, &PdcController::markStale);

    if (m_provider) {
        m_provider->setParent(this);
        connect(m_provider, &IPdcSensorProvider::readingsChanged,
                this, &PdcController::onReadingsChanged);
        connect(m_provider, &IPdcSensorProvider::faultChanged,
                this, &PdcController::onProviderFault);
        m_provider->start();
    }
}

void PdcController::setActive(bool active)
{
    if (m_state.active == active) {
        return;
    }

    m_state.active = active;
    if (!active) {
        m_state.warningLevel = PdcWarningLevel::Off;
        m_state.nearestDistanceCm = -1.0f;
    }
    publishState();
}

void PdcController::setVehicleSpeed(float kmh)
{
    m_vehicleSpeedKmh = kmh;
}

void PdcController::onReadingsChanged(const QVector<PdcSensorReading> &readings)
{
    m_state.rearSensors = readings;
    m_state.stale = false;
    m_state.fault.clear();

    float nearest = std::numeric_limits<float>::max();
    for (const PdcSensorReading &reading : readings) {
        if (reading.valid && reading.distanceCm < nearest) {
            nearest = reading.distanceCm;
        }
    }

    if (nearest == std::numeric_limits<float>::max()) {
        m_state.nearestDistanceCm = -1.0f;
        m_state.warningLevel = PdcWarningLevel::Off;
    } else {
        m_state.nearestDistanceCm = nearest;
        const bool speedAllowsAlert = (m_vehicleSpeedKmh <= kMaxActiveSpeedKmh);
        m_state.warningLevel = (m_state.active && speedAllowsAlert)
            ? levelForDistance(nearest)
            : PdcWarningLevel::Off;
    }

    m_staleTimer.start();
    publishState();
}

void PdcController::onProviderFault(const QString &message)
{
    qWarning() << "[PDC]" << message;
    m_state.fault = message;
    markStale();
}

void PdcController::markStale()
{
    m_state.stale = true;
    m_state.nearestDistanceCm = -1.0f;
    m_state.warningLevel = PdcWarningLevel::Off;
    publishState();
}

PdcWarningLevel PdcController::levelForDistance(float distanceCm) const
{
    if (distanceCm < 0.0f) return PdcWarningLevel::Off;
    if (distanceCm < 30.0f) return PdcWarningLevel::Critical;
    if (distanceCm < 60.0f) return PdcWarningLevel::Caution;
    if (distanceCm < 120.0f) return PdcWarningLevel::Near;
    return PdcWarningLevel::Far;
}

void PdcController::publishState()
{
    emit stateChanged(m_state);
}
