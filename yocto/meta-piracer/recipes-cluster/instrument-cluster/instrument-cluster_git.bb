SUMMARY = "PiRacer Instrument Cluster"
DESCRIPTION = "Qt-based PiRacer instrument cluster dashboard"
LICENSE = "CLOSED"

SRC_URI = "file://piracer-cluster.service"

inherit cmake_qt5 pkgconfig systemd externalsrc

# Use the repository's local source tree during development.
EXTERNALSRC = "${TOPDIR}/../../Head_Unit_jun/instrument_cluster"
S = "${EXTERNALSRC}"
B = "${WORKDIR}/build"
# Suppress checksum warnings for externalsrc helper symlinks (oe-workdir/oe-logs).
EXTERNALSRC_SYMLINKS = ""

DEPENDS += "qtbase qtserialport"

EXTRA_OECMAKE += " \
    -DCMAKE_BUILD_TYPE=Release \
"

do_install:append() {
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/piracer-cluster.service ${D}${systemd_system_unitdir}/piracer-cluster.service
}

FILES:${PN} += " \
    ${bindir}/* \
    ${systemd_system_unitdir}/piracer-cluster.service \
"

SYSTEMD_SERVICE:${PN} = "piracer-cluster.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"
