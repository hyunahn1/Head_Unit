SUMMARY = "PiRacer Python library (vehicles, gamepads, sensors)"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=d9a80b81dcffe04b0bf5bcb2ebe68b2c"

inherit pypi setuptools3

PYPI_PACKAGE = "piracer-py"

SRC_URI[sha256sum] = "24cf0acd26812eee416b9c01454260b5dc860c819379589d3bf49cdee42bc950"

RDEPENDS:${PN} += " \
    python3-core \
    python3-misc \
    python3-io \
    python3-logging \
"
