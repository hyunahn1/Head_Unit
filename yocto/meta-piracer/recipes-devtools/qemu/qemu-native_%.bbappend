FILESEXTRAPATHS:prepend := "${THISDIR}/qemu:"

SRC_URI:append = " file://0001-fix-gcc12-unreachable-stub-helper.patch"
