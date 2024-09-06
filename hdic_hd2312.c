// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2024 hanwckf <hanwckf@vip.qq.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/dvb/frontend.h>
#include <linux/types.h>

#include "dvb-usb.h"
#include "hd2312.h"

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

static struct dvb_usb_device_properties hd2312_properties;

struct dvb_frontend *hd2312_attach(struct dvb_usb_device *dev);

static int hd2312_frontend_attach(struct dvb_usb_adapter *adap)
{
	adap->fe_adap[0].fe = dvb_attach(hd2312_attach, adap->dev);

	if (!adap->fe_adap[0].fe)
		return -ENODEV;

	return 0;
}

static int hd2312_probe(struct usb_interface *intf,
			 const struct usb_device_id *id)
{
	int ret;
	struct usb_device *udev = interface_to_usbdev(intf);
	u8 *buf = kzalloc(4, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
					CYUSB_HD2312_GET_VERSION,
					USB_TYPE_VENDOR | USB_DIR_IN,
					0xFE, 0, buf, 4, 500);

	if (ret != 4)
		goto err;

	pr_info("hd2312: init done, ver=%d.%d.%d%d\n", buf[0], buf[1], buf[2], buf[3]);
	ret = dvb_usb_device_init(intf, &hd2312_properties,
		THIS_MODULE, NULL, adapter_nr);

err:
	kfree(buf);
	return ret;
}

static int hd2312_streaming_ctrl(struct dvb_usb_adapter *adpt, int onoff)
{
	int ret;
	struct usb_device *dev = adpt->dev->udev;

	pr_debug("%s: onoff=%d\n", __func__, onoff);

	ret = usb_control_msg(dev, usb_sndctrlpipe(dev, 0), (onoff ? 0xab : 0xac), USB_TYPE_VENDOR, 0xFE, 0, NULL, 0, 0);

	return ret;
}

static int hd2312_power_ctrl(struct dvb_usb_device *d, int onoff)
{
	int ret;
	struct usb_device *dev = d->udev;

	pr_debug("%s: onoff=%d\n", __func__, onoff);

	ret = usb_control_msg(dev, usb_sndctrlpipe(dev, 0), (onoff ? 0xad : 0xae), USB_TYPE_VENDOR, 0xFE, 0, NULL, 0, 0);

	return ret;
}

static struct usb_device_id hd2312_table[] = {
	{ USB_DEVICE(0x04b4, 0x1004) },
	{ }		/* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, hd2312_table);

static struct dvb_usb_device_properties hd2312_properties = {
	.usb_ctrl = DEVICE_SPECIFIC,

	.num_adapters = 1,
	.adapter = {{
		.num_frontends = 1,
		.fe = {{
		.frontend_attach = hd2312_frontend_attach,
		.streaming_ctrl = hd2312_streaming_ctrl,

		.stream = {
			.type = USB_BULK,
			.count = 8,
			.endpoint = 0x82,
			.u = {
				.bulk = {
					.buffersize = 4096,
				}
			}
		},
		}},
	}},

	.power_ctrl = hd2312_power_ctrl,

	.num_device_descs = 1,
	.devices = {
		{
			.name = "HDIC HD2312 DTMB",
			.cold_ids = { NULL },
			.warm_ids = { &hd2312_table[0], NULL },
		},
	}
};

static struct usb_driver hd2312_driver = {
	.name		= "dvb_usb_hd2312",
	.probe		= hd2312_probe,
	.disconnect	= dvb_usb_device_exit,
	.id_table	= hd2312_table,
};

module_usb_driver(hd2312_driver);

MODULE_DESCRIPTION("HDIC HD2312 USB2.0 DTMB driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hanwckf <hanwckf@vip.qq.com>");
