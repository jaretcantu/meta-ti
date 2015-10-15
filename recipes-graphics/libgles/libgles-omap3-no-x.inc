DESCRIPTION = "libGLES for the omap3"
LICENSE = "proprietary-binary"
# 'TSPA.txt' might not be the best file to md5sum
LIC_FILES_CHKSUM = "file://TSPA.txt;md5=c0d5d9c1e38b41677144c4e24d6ddee1"

INC_PR = "r38"

EXCLUDE_FROM_WORLD = "1"

COMPATIBLE_MACHINE = "(omap3|ti814x|ti816x|ti33x|ti43x)"

PVR_INIT ?= "pvrsrvinit"

PROVIDES += "virtual/egl virtual/libgles1 virtual/libgles2"

RREPLACES_${PN} = "libegl libgles1 libgles2"
RREPLACES_${PN}-dev = "libegl-dev libgles1-dev libgles2-dev"
RREPLACES_${PN}-dbg = "libegl-dbg libgles1-dbg libgles2-dbg"

RPROVIDES_${PN} = "libegl libgles1 libgles2"
RPROVIDES_${PN}-dev = "libegl-dev libgles1-dev libgles2-dev"
RPROVIDES_${PN}-dbg = "libegl-dbg libgles1-dbg libgles2-dbg"

RCONFLICTS_${PN} = "libegl libgles1 libgles2"
RCONFLICTS_${PN}-dev = "libegl-dev libgles1-dev libgles2-dev"
RCONFLICTS_${PN}-dbg = "libegl-dbg libgles1-dbg libgles2-dbg"

SRC_URI = "http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/gfxsdk/${SGXPV}/exports/${BINFILE} \
		   file://cputype \
		   file://rc.pvr \
		   file://99-bufferclass.rules  \
"

S = "${WORKDIR}/OMAP35x_Graphics_SDK_${SGXPV}"

# Logic to unpack installjammer file
TI_BIN_UNPK_CMDS="Y: qY:workdir:Y"
require ../../recipes-ti/includes/ti-eula-unpack.inc

export BINLOCATION ?= "${S}/gfx_rel"
export ES3LOCATION ?= "${S}/gfx_rel_es3.x"
export ES5LOCATION ?= "${S}/gfx_rel_es5.x"
export ES6LOCATION ?= "${S}/gfx_rel_es6.x"
export ES8LOCATION ?= "${S}/gfx_rel_es8.x"
export ES9LOCATION ?= "${S}/gfx_rel_es9.x"

LIBGLESWINDOWSYSTEM ?= "libpvrPVR2D_FRONTWSEGL.so.1"

do_configure() {
	# Attempt to fix up the worst offenders for file permissions
	for i in $(find ${S} -name "*.h") $(find ${S} -name "*.c") $(find ${S} -name "Make*") ; do
		chmod 0644 $i
	done

	# Attempt to create proper library softlinks
	for sofile in $(find ${S} -name "lib*Open*.so") $(find ${S} -name "lib*srv*.so") $(find ${S} -name "lib*gl*.so") $(find ${S} -name "libpvr*.so") $(find ${S} -name "lib*GL*.so"); do
		if [ "$(readlink -n ${sofile})" = "" ] ; then
			mv $sofile ${sofile}.${IMGPV}
			ln -sf $(basename ${sofile}.${IMGPV}) ${sofile}
			ln -sf $(basename ${sofile}.${IMGPV}) ${sofile}$(echo ${IMGPV} | awk -F. '{print "." $1}')
			ln -sf $(basename ${sofile}.${IMGPV}) ${sofile}$(echo ${IMGPV} | awk -F. '{print "." $1 "." $2}')
		fi
	done

	# Due to recursive make PLAT_* isn't always passed down correctly, so use sed to fix those
	for mak in $(find ${S} -name "*.mak") ; do
		sed -i -e s:arm-none-linux-gnueabi-:${TARGET_PREFIX}:g $mak
	done

	# clear out old stuff
	find Binaries/ | xargs rm -f || true
}

# Force in GNU_HASH and paths to libs
TARGET_CC_ARCH += " ${TARGET_LINK_HASH_STYLE} -Wl,-rpath-link,${BINLOCATION} -L${BINLOCATION} \
-L${STAGING_DIR_TARGET}${libdir} -Wl,-rpath-link,${STAGING_DIR_TARGET}${libdir}"
PARALLEL_MAKE = ""

PLATFORM ?= "LinuxOMAP3"

do_compile() {
	export TOOLCHAIN="${TOOLCHAIN_PATH}"
	export PLAT_CC="${CC}"
	export PLAT_CPP="${CXX}"
	export PLAR_AR="${AR}"

	mkdir -p ${S}/demos/raw
	mkdir -p ${S}/trainingcourses/raw

	# Rebuild demos
	for demo in ${S}/GFX_Linux_SDK/OGLES/SDKPackage/Demos/* ; do
		cd $demo/OGLES/Build/LinuxGeneric
		oe_runmake Common=1 PLATFORM=${PLATFORM} X11BUILD=0
		rm $demo/OGLES/Build/${PLATFORM}/Release*/*.o

		install -m 0755 $demo/OGLES/Build/${PLATFORM}/ReleaseRaw/* ${S}/demos/raw || true
	done

	for demo in ${S}/GFX_Linux_SDK/OGLES2/SDKPackage/Demos/* ; do
		cd $demo/OGLES2/Build/LinuxGeneric
		oe_runmake Common=1 PLATFORM=${PLATFORM} X11BUILD=0
		rm -f $demo/OGLES2/Build/${PLATFORM}/Release*/*.o

		install -m 0755 $demo/OGLES2/Build/${PLATFORM}/ReleaseRaw/* ${S}/demos/raw || true
	done

	find ${S} -name "*_org" -delete

	# Build OGLES2 Trainingcourses
		for training in ${S}/GFX_Linux_SDK/OGLES2/SDKPackage/TrainingCourse/* ; do
			if [ -e $training/OGLES2/Build/LinuxGeneric/Makefile ] ; then
				cd $training/OGLES2/Build/LinuxGeneric
			fi

			if [ -e $training/OGLES2/Build/${PLATFORM}/Makefile ] ; then
				cd $training/OGLES2/Build/${PLATFORM}
			fi

			oe_runmake Common=1 PLATFORM=${PLATFORM} X11BUILD=0
			rm -f $training/OGLES2/Build/${PLATFORM}/Release*/*.o

			install -m 0755 $training/OGLES2/Build/${PLATFORM}/ReleaseRaw/* ${S}/trainingcourses/raw || true
		done

}

do_install () {
	install -d ${D}${libdir}
	cp -pPR ${BINLOCATION}/*.so* ${D}${libdir}
	rm ${D}${libdir}/libpvrPVR2D_DRIWSEGL.so*
	rm ${D}${libdir}/libsrv_um_dri.so*
	rm ${D}${libdir}/pvr_drv.so* 

	install -m 0644 ${BINLOCATION}/*.a ${D}${libdir}

	install -d ${D}${bindir}/
	install -m 0755 ${WORKDIR}/cputype ${D}${bindir}/

	install -m 0755 ${BINLOCATION}/*_test ${D}${bindir}/
	install -m 0755 ${BINLOCATION}/gl* ${D}${bindir}/
	install -m 0755 ${BINLOCATION}/${PVR_INIT} ${D}${bindir}/

	install -d ${D}${includedir}
	cp -pPR ${S}/GFX_Linux_KM/include4 ${D}${includedir}/
	cp -pPR ${S}/GFX_Linux_KM/services4 ${D}${includedir}/

	cp -pPr ${S}/include/pvr2d/*.h ${D}${includedir}
	cp -pPr ${S}/include/OGLES2/* ${D}${includedir}/
	cp -pPr ${S}/include/wsegl/*.h ${D}${includedir}/
	
	install -d ${D}${sysconfdir}/init.d/
	cp -pP ${WORKDIR}/rc.pvr ${D}${sysconfdir}/init.d/pvr-init
	chmod +x ${D}${sysconfdir}/init.d/pvr-init

	install -d ${D}${sysconfdir}
	echo "[default]" > ${D}${sysconfdir}/powervr.ini
	echo "WindowSystem=${LIBGLESWINDOWSYSTEM}" >> ${D}${sysconfdir}/powervr.ini

	install -d ${D}${bindir}/SGX/demos/Raw/
	install -m 0755 ${S}/demos/raw/* ${D}${bindir}/SGX/demos/Raw/
	install -d ${D}${bindir}/SGX/trainingcourses/Raw 2>/dev/null || true
	install -m 0755 ${S}/trainingcourses/raw/* ${D}${bindir}/SGX/trainingcourses/Raw/ 2>/dev/null || true


	# Delete objects and linker scripts hidden between the headers
	find ${D} -name "*.o" -delete
	find ${D} -name "*.o.cmd" -delete

	install -d ${D}${sysconfdir}/udev/rules.d
	install -m 0644 ${WORKDIR}/99-bufferclass.rules ${D}${sysconfdir}/udev/rules.d/

}

PACKAGES =+ "${PN}-rawdemos ${PN}-rawtrainingcourses ${PN}-tests"
# Package the base libraries per silicon revision

RPROVIDES_${PN} += "libGLESv2.so libEGL.so libGLES_CM.so libpvr2d.so libIMGegl.so libsrv_init.so libsrv_um.so libsrv_um_dri.so libglslcompiler.so"

PACKAGES =+ "${PN}-blitwsegl ${PN}-flipwsegl ${PN}-frontwsegl ${PN}-linuxfbwsegl"
FILES_${PN}-blitwsegl = "${libdir}/libpvrPVR2D_BLITWSEGL.so.*"
FILES_${PN}-flipwsegl = "${libdir}/libpvrPVR2D_FLIPWSEGL.so.*"
FILES_${PN}-frontwsegl = "${libdir}/libpvrPVR2D_FRONTWSEGL.so.*"
FILES_${PN}-linuxfbwsegl = "${libdir}/libpvrPVR2D_LINUXFBWSEGL.so.*"

CONFFILES_${PN} = "${sysconfdir}/powervr.ini"

FILES_${PN} = "${sysconfdir} ${libdir}/*.so* ${bindir}/${PVR_INIT} ${bindir}/cputype ${bindir}/*"

FILES_${PN}-tests = "${bindir}/*test*"
FILES_${PN}-dbg = "${libdir}/.debug/* ${bindir}/.debug/* \
                   ${bindir}/*/.debug\
                   ${bindir}/SGX/demos/*/.debug/* \
                   ${bindir}/SGX/trainingcourses/*/.debug/* \
"

FILES_${PN}-rawdemos = "${bindir}/SGX/demos/Raw/*"

FILES_${PN}-rawtrainingcourses = "${bindir}/SGX/trainingcourses/Raw/*"

# The libs need the kernel-modules
RRECOMMENDS_${PN} += "omap3-sgx-modules"

# The initscript calls fbset, cputype calls devmem2
RDEPENDS_${PN} += "fbset devmem2"

#HACK! These are binaries, so we can't guarantee that LDFLAGS match :(
INSANE_SKIP_${PN} = "ldflags dev-so"
INSANE_SKIP_${PN}-rawdemos = "ldflags dev-so useless-rpaths"
INSANE_SKIP_${PN}-blitwsegl = "ldflags"
INSANE_SKIP_${PN}-flipwsegl = "ldflags"
INSANE_SKIP_${PN}-frontwsegl = "ldflags"
INSANE_SKIP_${PN}-linuxfbwsegl = "ldflags"
INSANE_SKIP_${PN}-tests = "ldflags"

# Quality control is really poor on these SDKs, so hack around the latest madness:
FILES_${PN} += "${libdir}/*.so "
FILES_${PN}-dev = "${includedir}"

inherit update-rc.d

INITSCRIPT_NAME = "pvr-init"
INITSCRIPT_PARAMS = "start 30 5 2 . stop 40 0 1 6 ."

# Append to update-rc.d postinst
pkg_postinst_${PN}_append() {
rm -f $D${sysconfdir}/powervr-esrev

}

pkg_postinst_${PN}-blitwsegl() {
rm -f $D${sysconfdir}/powervr-esrev
}

pkg_postinst_${PN}-flipwsegl() {
rm -f $D${sysconfdir}/powervr-esrev
}

pkg_postinst_${PN}-frontwsegl() {
rm -f $D${sysconfdir}/powervr-esrev
}
pkg_postinst_${PN}-linuxfbwsegl() {
rm -f $D${sysconfdir}/powervr-esrev
}
