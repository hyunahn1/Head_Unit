SUMMARY = "PiRacer Python library (vehicles, gamepads, sensors)"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://PKG-INFO;beginline=1;endline=1;md5=d41d8cd98f00b204e9800998ecf8427e"

inherit pypi python_setuptools_build_meta

PYPI_PACKAGE = "piracer-py"
PV = "0.1.1"

SRC_URI[sha256sum] = "060c0449097a7431b6482db50e95a9e993a5b721dc77c80af0edc0d6663eb9ef"

PYPI_SRC_URI = "https://files.pythonhosted.org/packages/py3/p/piracer_py/piracer_py-0.1.1-py3-none-any.whl"

RDEPENDS:${PN} += " \
    python3-core \
    python3-smbus2 \
"
