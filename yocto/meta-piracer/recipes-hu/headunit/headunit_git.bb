SUMMARY = "PiRacer Head Unit"
DESCRIPTION = "Qt-based PiRacer Head Unit shell and modules"
LICENSE = "CLOSED"

SRC_URI = " \
    file:///home/seame2026/piracer/Head_Unit_jun \
    file://hu-shell.service \
"

S = "${WORKDIR}/home/seame2026/piracer/Head_Unit_jun"

inherit cmake pkgconfig systemd

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
