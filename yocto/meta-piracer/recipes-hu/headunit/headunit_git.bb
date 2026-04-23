SUMMARY = "PiRacer Head Unit"
DESCRIPTION = "Qt-based PiRacer Head Unit shell and modules (QtWayland Compositor + multi-process)"
LICENSE = "CLOSED"

SRC_URI = " \
    file://hu-shell.init \
    file://hu-shell.service \
"

inherit cmake_qt5 pkgconfig update-rc.d systemd externalsrc

# Use the repository's local source tree during development.
EXTERNALSRC = "${TOPDIR}/../../Head_Unit_jun"
S = "${EXTERNALSRC}"
B = "${WORKDIR}/build"
EXTERNALSRC_SYMLINKS = ""

# qtwayland: compositor API + wayland QPA 플러그인 (모듈 클라이언트용)
# qtmultimedia: ReverseCameraWindow live preview (QCamera/QCameraViewfinder)
DEPENDS += "qtbase qtmultimedia vsomeip3 boost qtwayland gstreamer1.0 gstreamer1.0-plugins-base"

EXTRA_OECMAKE += " \
    -DCMAKE_BUILD_TYPE=Release \
    -DHU_HAS_VSOMEIP=ON \
"

# SysVinit 설정
INITSCRIPT_NAME = "hu-shell"
INITSCRIPT_PARAMS = "defaults 98"

# systemd 설정
SYSTEMD_SERVICE:${PN} = "hu-shell.service"
SYSTEMD_AUTO_ENABLE = "enable"

do_install:append() {
    # init.d 스크립트 설치
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/hu-shell.init ${D}${sysconfdir}/init.d/hu-shell

    # systemd 서비스 설치
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/hu-shell.service ${D}${systemd_system_unitdir}/hu-shell.service
}

FILES:${PN} += " \
    ${bindir}/hu_shell \
    ${bindir}/hu_module_media \
    ${bindir}/hu_module_youtube \
    ${bindir}/hu_module_call \
    ${bindir}/hu_module_navigation \
    ${bindir}/hu_module_ambient \
    ${bindir}/hu_module_settings \
    ${bindir}/config/ \
    ${sysconfdir}/init.d/hu-shell \
    ${systemd_system_unitdir}/hu-shell.service \
"
