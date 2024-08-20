hd2312-objs := hdic_hd2312.o

ifndef CONFIG_DVB_USB
hd2312-objs += dvb-usb-init.o dvb-usb-i2c.o dvb-usb-urb.o dvb-usb-dvb.o usb-urb.o dvb-usb-firmware.o
ifdef CONFIG_RC_CORE
hd2312-objs += dvb-usb-remote.o
endif
endif

obj-m := hd2312.o hd2312-fe.o

KVERSION := $(shell uname -r)
CURDIR := $(shell pwd)

all:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(CURDIR) modules

clean:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(CURDIR) clean
