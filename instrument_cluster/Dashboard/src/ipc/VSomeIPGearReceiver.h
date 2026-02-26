/**
 * @file VSomeIPGearReceiver.h
 * @brief VSOMEIP gear receiver for Instrument Cluster
 * @author Ahn Hyunjun
 * @date 2026-02-26
 */

#ifndef VSOMEIPGEARRECEIVER_H
#define VSOMEIPGEARRECEIVER_H

#include <QObject>

#ifdef IC_HAS_VSOMEIP
#include <vsomeip/vsomeip.hpp>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#endif

class VSomeIPGearReceiver : public QObject
{
    Q_OBJECT

public:
    explicit VSomeIPGearReceiver(QObject *parent = nullptr);
    ~VSomeIPGearReceiver() override;

signals:
    void gearReceived(const QString &gear);

private:
#ifdef IC_HAS_VSOMEIP
    void onState(vsomeip::state_type_e state);
    void onMessage(const std::shared_ptr<vsomeip::message> &msg);

    std::shared_ptr<vsomeip::application> m_app;
    std::thread m_worker;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_registered{false};
#endif
};

#endif // VSOMEIPGEARRECEIVER_H
