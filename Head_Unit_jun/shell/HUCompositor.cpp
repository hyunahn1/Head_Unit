/**
 * @file HUCompositor.cpp
 */

#include "HUCompositor.h"

#include <QWaylandOutput>
#include <QWaylandXdgShell>
#include <QWaylandXdgSurface>
#include <QWaylandXdgToplevel>
#include <QWaylandSurface>
#include <QDebug>

HUCompositor::HUCompositor(QObject *parent)
    : QWaylandCompositor(parent)
{
    // Socket name is relative to XDG_RUNTIME_DIR
    // Module processes must have WAYLAND_DISPLAY=wayland-hu
    setSocketName("wayland-hu");
}

void HUCompositor::setupOutput(const QSize &screenSize)
{
    if (!m_output) {
        m_output = new QWaylandOutput(this, nullptr);
    }
    QRect geo(QPoint(0, 0), screenSize);
    m_output->setAvailableGeometry(geo);

    // Advertise a mode so clients know the screen resolution
    QWaylandOutputMode mode(screenSize, 60000); // 60 Hz
    m_output->addMode(mode, true /*preferred*/);
    m_output->setCurrentMode(mode);
}

void HUCompositor::create()
{
    m_xdgShell = new QWaylandXdgShell(this);
    connect(m_xdgShell, &QWaylandXdgShell::toplevelCreated,
            this, &HUCompositor::onXdgToplevelCreated);

    QWaylandCompositor::create();
    qDebug() << "[HUCompositor] created, socket=" << socketName()
             << "XDG_RUNTIME_DIR=" << qgetenv("XDG_RUNTIME_DIR");
}

void HUCompositor::onXdgToplevelCreated(QWaylandXdgToplevel *toplevel,
                                         QWaylandXdgSurface  *xdgSurface)
{
    QWaylandSurface *surface = xdgSurface->surface();

    // Send fullscreen configure so clients resize to fill the output
    if (m_output) {
        const QSize sz = m_output->geometry().size();
        toplevel->sendFullscreen(sz);
    }

    const QString title = toplevel->title();
    if (!title.isEmpty()) {
        registerSurface(title, surface);
    } else {
        connect(toplevel, &QWaylandXdgToplevel::titleChanged,
                this, [this, toplevel, surface]() {
            const QString t = toplevel->title();
            if (!t.isEmpty() && !m_surfaces.contains(t))
                registerSurface(t, surface);
        });
    }
}

void HUCompositor::registerSurface(const QString &name, QWaylandSurface *surface)
{
    if (m_surfaces.contains(name)) return;

    m_surfaces.insert(name, surface);
    qDebug() << "[HUCompositor] module surface registered:" << name
             << "pid=" << surface->client()->processId();

    connect(surface, &QWaylandSurface::destroyed, this, [this, name]() {
        m_surfaces.remove(name);
        qDebug() << "[HUCompositor] module surface destroyed:" << name;
        emit moduleSurfaceDestroyed(name);
    });

    emit moduleSurfaceCreated(name, surface);
}

QWaylandSurface *HUCompositor::surfaceForModule(const QString &moduleName) const
{
    return m_surfaces.value(moduleName, nullptr);
}
