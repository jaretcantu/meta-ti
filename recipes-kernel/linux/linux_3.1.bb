require linux.inc

DESCRIPTION = "Linux kernel for TI processors"
KERNEL_IMAGETYPE = "uImage"

DEFAULT_PREFERENCE = "-99"

PV = "3.0+3.1rc"
SRCREV_pn-${PN} = "140d0b2108faebc77c6523296e211e509cb9f5f9"

# The main PR is now using MACHINE_KERNEL_PR, for omap3 see conf/machine/include/omap3.inc
MACHINE_KERNEL_PR_append = "a"

SRC_URI += "git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git;protocol=git \
            file://beagle/0001-UNFINISHED-OMAP3-beagle-add-support-for-expansionboa.patch \
            file://beagle/0002-HACK-OMAP3-beagle-switch-to-GPTIMER1.patch \
            file://beagle/0003-OMAP3-beagle-HACK-add-in-1GHz-OPP.patch \
            file://madc/0001-Enabling-Hwmon-driver-for-twl4030-madc.patch \
            file://madc/0002-mfd-twl-core-enable-madc-clock.patch \
            file://sgx/0001-ARM-L2-Add-and-export-outer_clean_all.patch \
            file://fixes/vout.patch \
            file://defconfig"

SRC_URI_append_beagleboard = " file://logo_linux_clut224.ppm \
"

S = "${WORKDIR}/git"
