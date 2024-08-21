LINUXTV_MENU := DVB Drivers

define KernelPackage/dvb-core
  SUBMENU := $(LINUXTV_MENU)
  TITLE := DVB core support
  KCONFIG := \
	CONFIG_DVB_CORE \
	CONFIG_DVB_DEMUX_SECTION_LOSS_LOG=n \
	CONFIG_DVB_DYNAMIC_MINORS=y \
	CONFIG_DVB_ULE_DEBUG=n \
	CONFIG_DVB_PLATFORM_DRIVERS=n \
	CONFIG_DVB_MAX_ADAPTERS=16 \
	CONFIG_DVB_NET=y \
	CONFIG_DVB_MMAP=n \
	CONFIG_MEDIA_DIGITAL_TV_SUPPORT=y \
	CONFIG_MEDIA_ATTACH=y
  FILES := $(LINUX_DIR)/drivers/media/dvb-core/dvb-core.ko
  DEPENDS := +kmod-video-core
  AUTOLOAD := $(call AutoProbe,dvb-core)
endef

define KernelPackage/dvb-core/description
 Kernel modules for DVB support.
endef

$(eval $(call KernelPackage,dvb-core))

define KernelPackage/dvb-usb
  SUBMENU := $(LINUXTV_MENU)
  TITLE := Support for various USB DVB devices
  KCONFIG := CONFIG_MEDIA_USB_SUPPORT=y \
  CONFIG_DVB_USB
  FILES := $(LINUX_DIR)/drivers/media/usb/dvb-usb/dvb-usb.ko
  AUTOLOAD := $(call AutoProbe,dvb-usb)
  DEPENDS := +kmod-dvb-core +kmod-i2c-core +kmod-multimedia-input +kmod-usb-core
endef

define KernelPackage/dvb-usb/description
 By enabling this you will be able to choose the various supported
 USB1.1 and USB2.0 DVB devices.

 Almost every USB device needs a firmware.

 For a complete list of supported USB devices see the LinuxTV DVB Wiki:
 <http://www.linuxtv.org/wiki/index.php/DVB_USB>
endef

$(eval $(call KernelPackage,dvb-usb))
