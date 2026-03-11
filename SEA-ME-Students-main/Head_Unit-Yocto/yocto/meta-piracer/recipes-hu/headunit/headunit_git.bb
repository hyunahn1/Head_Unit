SUMMARY = "PiRacer Head Unit"
DESCRIPTION = "Qt-based PiRacer Head Unit shell and modules"
LICENSE = "CLOSED"

SRC_URI = "file://hu-shell.service"

inherit cmake_qt5 pkgconfig systemd externalsrc

# Use the repository's local source tree during development.
EXTERNALSRC = "${TOPDIR}/../../Head_Unit_jun"
S = "${EXTERNALSRC}"
B = "${WORKDIR}/build"
# Suppress checksum warnings for externalsrc helper symlinks (oe-workdir/oe-logs).
EXTERNALSRC_SYMLINKS = ""

DEPENDS += "qtbase"

EXTRA_OECMAKE += " \
    -DCMAKE_BUILD_TYPE=Release \
"

do_install:append() {
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/hu-shell.service ${D}${systemd_system_unitdir}/hu-shell.service
}

FILES:${PN} += " \
    ${bindir}/* \
    ${systemd_system_unitdir}/hu-shell.service \
"

SYSTEMD_SERVICE:${PN} = "hu-shell.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"
