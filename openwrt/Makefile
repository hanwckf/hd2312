include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=dvb-hd2312
PKG_VERSION:=1.0

PKG_MAINTAINER:=hanwckf
PKG_LICENSE:=GPL-2.0

PKG_SOURCE_URL:=https://github.com/hanwckf/hd2312
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=72d5fa79bf8070977568c5fd3b0e03d33817a3e0

include $(INCLUDE_DIR)/package.mk

EXTRA_CFLAGS := -I$(LINUX_DIR)/drivers/media/usb/dvb-usb -I$(LINUX_DIR)/drivers/media/dvb-frontends

define KernelPackage/dvb-hd2312
  TITLE:=HD2312 DTMB kernel module
  SECTION:=kernel
  SUBMENU:=DVB Support
  DEPENDS:=+kmod-dvb-usb
  FILES:=$(PKG_BUILD_DIR)/hd2312.ko $(PKG_BUILD_DIR)/hd2312-fe.ko
  AUTOLOAD:=$(call AutoProbe,hd2312-fe hd2312)
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

define Build/Compile
	$(KERNEL_MAKE) M=$(PKG_BUILD_DIR) \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		CONFIG_DVB_USB=y \
		modules
endef

$(eval $(call KernelPackage,dvb-hd2312))
