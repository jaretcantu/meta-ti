SOC_FAMILY = "omap-a15"
require conf/machine/include/soc-family.inc

require conf/machine/include/tune-cortexa15.inc

# Increase this everytime you change something in the kernel
MACHINE_KERNEL_PR = "r3"

PREFERRED_PROVIDER_virtual/kernel = "linux-ti-staging"
PREFERRED_PROVIDER_virtual/bootloader = "u-boot-ti-staging"
PREFERRED_PROVIDER_u-boot = "u-boot-ti-staging"

KERNEL_IMAGETYPE = "zImage"

UBOOT_ARCH = "arm"
UBOOT_ENTRYPOINT = "0x80008000"
UBOOT_LOADADDRESS = "0x80008000"

EXTRA_IMAGEDEPENDS += "virtual/bootloader"

PREFERRED_PROVIDER_virtual/xserver = "xserver-xorg"
XSERVER = "xserver-xorg \
           xf86-input-evdev \
           xf86-video-fbdev"

GUI_MACHINE_CLASS = "smallscreen"

# Use the expected value of the ubifs filesystem's volume name in the kernel
# and u-boot.
UBI_VOLNAME = "rootfs"
