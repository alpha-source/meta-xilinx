DESCRIPTION = "Aeva Utils" 
SECTION = "examples" 
LICENSE = "CLOSED" 

#SRC_URI = "git://https://github.com/zafrullahsyed/cameracapture.git;protocol=https;tag=v0.1"
SRCREV = "${AUTOREV}"
SRC_URI = "git://git@bitbucket.org/sandesh-aeva/system-software-utils.git;protocol=ssh;branch=utils"
S = "${WORKDIR}/git"



inherit pkgconfig cmake

EXTRA_OECMAKE = ""