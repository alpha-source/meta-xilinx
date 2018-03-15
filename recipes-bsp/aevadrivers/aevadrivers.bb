DESCRIPTION = "Aeva Drivers" 
SECTION = "examples" 
LICENSE = "CLOSED" 

inherit module

SRCREV = "${AUTOREV}"

SRC_URI = "git://git@bitbucket.org/sandesh-aeva/linux-drivers.git;protocol=ssh;branch=yocto-build"

S = "${WORKDIR}/git"

do_install () {
    install -d ${D}${base_libdir}/
    install -m 0644 ${S}/src/dma/aeva_dma.ko ${D}${base_libdir}
    install -m 0644 ${S}/src/spi/aeva_spi.ko ${D}${base_libdir}
    install -m 0644 ${S}/src/interrupt_handler/aeva_interrupt_handler.ko ${D}${base_libdir}
    install -m 0644 ${S}/src/dma/aeva_dma2.ko ${D}${base_libdir}
    install -m 0644 ${S}/src/dma/axidmatest.c.ko ${D}${base_libdir}
}

FILES_${PN} += " \
        ${base_libdir}/aeva_dma.ko \
        ${base_libdir}/aeva_spi.ko \
        ${base_libdir}/aeva_interrupt_handler.ko \
        ${base_libdir}/aeva_dma2.ko \
        ${base_libdir}/axidmatest.ko \
"

COMPATIBLE_MACHINE = "(zcu102-zynqmp)"