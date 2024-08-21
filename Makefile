KVERSION ?= $(shell uname -r)
KDIR ?= /lib/modules/$(KVERSION)/build
CURDIR := $(shell pwd)

hd2312-objs := hdic_hd2312.o

include $(KDIR)/.config

FLAGS := -I$(KDIR)/drivers/media/usb/dvb-usb -I$(KDIR)/drivers/media/dvb-frontends -I$(CURDIR)/dvb-usb

ifndef CONFIG_DVB_USB
hd2312-objs += dvb-usb/dvb-usb-init.o dvb-usb/dvb-usb-i2c.o dvb-usb/dvb-usb-urb.o dvb-usb/dvb-usb-dvb.o dvb-usb/usb-urb.o dvb-usb/dvb-usb-firmware.o
ifdef CONFIG_RC_CORE
hd2312-objs += dvb-usb/dvb-usb-remote.o
endif
endif

obj-m := hd2312.o hd2312-fe.o

all:
	$(MAKE) -C $(KDIR) M=$(CURDIR) EXTRA_CFLAGS="$(FLAGS)" modules

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean
