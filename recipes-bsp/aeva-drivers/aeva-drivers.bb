SUMMARY = "Simplest hello world kernel module."
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit module

SRC_URI = " \
    file://Makefile \
    file://src/Makefile \
    file://src/spi/Makefile \
    file://src/spi/aeva_spi.c \
    file://src/dma/Makefile \
    file://src/dma/aeva_dma.c \
    file://src/interrupt_handler/Makefile \
    file://src/interrupt_handler/aeva_interrupt_handler.c \
"

S = "${WORKDIR}"


do_install () {
    install -d ${D}${base_libdir}/
    install -m 0644 ${S}/src/dma/aeva_dma.ko ${D}${base_libdir}
    install -m 0644 ${S}/src/spi/aeva_spi.ko ${D}${base_libdir}
    install -m 0644 ${S}/src/interrupt_handler/aeva_interrupt_handler.ko ${D}${base_libdir}
}


FILES_${PN} += " \
        ${base_libdir}/aeva_dma.ko \
        ${base_libdir}/aeva_spi.ko \
        ${base_libdir}/aeva_interrupt_handler.ko \
"

COMPATIBLE_MACHINE = "(zcu102-zynqmp)"