SUMMARY = "PiRacer Head Unit DomU image"
DESCRIPTION = "DomU image that contains the Head Unit application stack"
LICENSE = "MIT"

inherit core-image

IMAGE_FEATURES += "ssh-server-openssh"

IMAGE_INSTALL:append = " \
    headunit \
    qtbase-plugins \
    qtmultimedia-plugins \
    qtmultimedia \
    ttf-dejavu-sans \
    ttf-dejavu-common \
    ttf-noto-emoji-regular \
    fontconfig \
    fontconfig-utils \
"
