SUMMARY = "PiRacer Head Unit DomU image"
DESCRIPTION = "DomU image that contains the Head Unit application stack"
LICENSE = "MIT"

inherit core-image

IMAGE_FEATURES += "ssh-server-openssh"

IMAGE_INSTALL:append = " \
    headunit \
    instrument-cluster \
    qtbase-plugins \
    qtwayland \
    qtwayland-plugins \
    vsomeip3 \
    ttf-dejavu-sans \
    ttf-dejavu-common \
    fontconfig \
"

# weston 제거: hu_shell이 QtWayland Compositor로 동작하므로 별도 Weston 불필요
# hu_shell은 eglfs로 DRM/KMS에 직접 접근하는 Wayland 컴포지터입니다.

# WiFi 설정 + network interfaces 후처리로 덮어쓰기
ROOTFS_POSTPROCESS_COMMAND:append = " setup_wifi_config; "

setup_wifi_config() {
    printf 'ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\nupdate_config=1\n\nnetwork={\n    ssid="SEA:ME WiFi Access"\n    psk="1fy0u534m3"\n}\n' > ${IMAGE_ROOTFS}/etc/wpa_supplicant.conf
    chmod 600 ${IMAGE_ROOTFS}/etc/wpa_supplicant.conf
    grep -q "^auto wlan0" ${IMAGE_ROOTFS}/etc/network/interfaces || sed -i 's/^iface wlan0 inet dhcp/auto wlan0\niface wlan0 inet dhcp/' ${IMAGE_ROOTFS}/etc/network/interfaces
}
