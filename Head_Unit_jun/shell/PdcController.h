#ifndef PDCCONTROLLER_H
#define PDCCONTROLLER_H

#include "PdcTypes.h"

#include <QObject>
#include <QTimer>

class IPdcSensorProvider;

class PdcController : public QObject
{
    Q_OBJECT

public:
    explicit PdcController(IPdcSensorProvider *provider, QObject *parent = nullptr);

    const PdcState &state() const { return m_state; }
    void setActive(bool active);
    void setVehicleSpeed(float kmh);

signals:
    void stateChanged(const PdcState &state);

private slots:
    void onReadingsChanged(const QVector<PdcSensorReading> &readings);
    void onProviderFault(const QString &message);
    void markStale();

private:
    PdcWarningLevel levelForDistance(float distanceCm) const;
    void publishState();

    IPdcSensorProvider *m_provider = nullptr;
    PdcState m_state;
    QTimer m_staleTimer;
    float m_vehicleSpeedKmh = 0.0f;

    static constexpr float kMaxActiveSpeedKmh = 10.0f;
    static constexpr int kStaleTimeoutMs = 450;
};

#endif // PDCCONTROLLER_H
