SUMMARY = "PiRacer Head Unit"
DESCRIPTION = "Qt-based PiRacer Head Unit shell and modules (QtWayland Compositor + multi-process)"
LICENSE = "CLOSED"

SRC_URI = " \
    file://hu-shell.init \
"

inherit cmake_qt5 pkgconfig update-rc.d externalsrc

# Use the repository's local source tree during development.
EXTERNALSRC = "${TOPDIR}/../../Head_Unit_jun"
S = "${EXTERNALSRC}"
B = "${WORKDIR}/build"
EXTERNALSRC_SYMLINKS = ""

# qtwayland 추가: compositor API + wayland QPA 플러그인 (모듈 클라이언트용)
DEPENDS += "qtbase vsomeip3 boost qtwayland"

EXTRA_OECMAKE += " \
    -DCMAKE_BUILD_TYPE=Release \
    -DHU_HAS_VSOMEIP=ON \
"

# SysVinit init.d 스크립트 설정
INITSCRIPT_NAME = "hu-shell"
INITSCRIPT_PARAMS = "defaults 99"

do_install:append() {
    # init.d 스크립트 설치
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/hu-shell.init ${D}${sysconfdir}/init.d/hu-shell
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
"
