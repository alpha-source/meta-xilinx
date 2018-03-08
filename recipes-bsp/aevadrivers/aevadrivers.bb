DESCRIPTION = "Aeva Drivers" 
SECTION = "examples" 
LICENSE = "CLOSED" 

inherit module

SRCREV = "${AUTOREV}"

SRC_URI = "git://git@bitbucket.org/sandesh-aeva/linux-drivers.git;protocol=ssh;branch=yocto-build"

S = "${WORKDIR}/git"

COMPATIBLE_MACHINE = "(zcu102-zynqmp)"