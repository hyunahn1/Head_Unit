/**
 * @file ModuleController.cpp
 */

#include "ModuleController.h"
#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QDebug>

ModuleController::ModuleController(const QString &moduleName,
                                   const QString &executableName,
                                   const QString &socketPath,
                                   QObject *parent)
    : QObject(parent)
    , m_name(moduleName)
    , m_execName(executableName)
    , m_socketPath(socketPath)
    , m_restartTimer(new QTimer(this))
{
    m_restartTimer->setSingleShot(true);
    m_restartTimer->setInterval(RESTART_DELAY_MS);
    connect(m_restartTimer, &QTimer::timeout, this, &ModuleController::launch);
}

void ModuleController::launch()
{
    const QString binDir = QCoreApplication::applicationDirPath();
    const QString exe    = binDir + "/" + m_execName;

    m_process = new QProcess(this);
    m_process->setProgram(exe);
    m_process->setArguments({ m_socketPath });
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("QT_QPA_PLATFORM", "xcb");
    // Ensure DISPLAY is set for XWayland cross-process X11 embedding
    if (env.value("DISPLAY").isEmpty())
        env.insert("DISPLAY", ":0");
    m_process->setProcessEnvironment(env);

    connect(m_process,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ModuleController::onProcessFinished);

    m_process->start();
    qDebug() << "[ModuleController]" << m_name << "started, pid=" << m_process->processId();
    emit moduleStarted(m_name);
}

void ModuleController::terminate()
{
    if (!m_process) return;
    m_restartCount = MAX_RESTARTS; // 재시작 억제
    m_process->terminate();
    if (!m_process->waitForFinished(2000))
        m_process->kill();
}

bool ModuleController::isRunning() const
{
    return m_process && m_process->state() == QProcess::Running;
}

void ModuleController::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    qWarning() << "[ModuleController]" << m_name
               << "exited with code" << exitCode
               << (status == QProcess::CrashExit ? "(CRASH)" : "");
    emit moduleExited(m_name, exitCode);

    m_process->deleteLater();
    m_process = nullptr;

    if (m_restartCount < MAX_RESTARTS) {
        ++m_restartCount;
        qDebug() << "[ModuleController] scheduling restart" << m_restartCount
                 << "/" << MAX_RESTARTS << "for" << m_name;
        m_restartTimer->start();
    } else {
        qCritical() << "[ModuleController]" << m_name
                    << "exceeded max restarts — giving up";
    }
}
