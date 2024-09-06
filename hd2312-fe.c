// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2024 hanwckf <hanwckf@vip.qq.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/dvb/frontend.h>
#include <linux/types.h>
#include <linux/version.h>

#include "dvb-usb.h"
#include "hd2312.h"

static const struct dvb_frontend_ops hd2312_ops;

struct dvb_frontend *hd2312_attach(struct dvb_usb_device *dev)
{
	struct hd2312_state *state;
	struct dtv_frontend_properties *c;

	state = kzalloc(sizeof(struct hd2312_state), GFP_KERNEL);
	if (!state)
		return NULL;

	state->dev = dev;

	memcpy(&state->frontend.ops, &hd2312_ops,
		sizeof(struct dvb_frontend_ops));
	state->frontend.demodulator_priv = state;

	c = &state->frontend.dtv_property_cache;
	c->strength.len = 1;
	c->cnr.len = 1;

	return &state->frontend;
}
EXPORT_SYMBOL_GPL(hd2312_attach);

static void hd2312_release(struct dvb_frontend *fe)
{
	kfree(fe->demodulator_priv);
}

static int hd2312_init(struct dvb_frontend *fe)
{
	struct hd2312_state *state = fe->demodulator_priv;

	state->frequency = 0;
	return 0;
}

static int hd2312_set_frontend(struct dvb_frontend *fe)
{
	struct hd2312_state *state = fe->demodulator_priv;
	struct usb_device *dev;
	u32 freq;
	int ret;
	u8 *buf = kzalloc(4, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	freq = fe->dtv_property_cache.frequency;
	dev = state->dev->udev;

	buf[0] = (freq >> 24) & 0xff;
	buf[1] = (freq >> 16) & 0xff;
	buf[2] = (freq >> 8) & 0xff;
	buf[3] = freq & 0xff;

	pr_debug("hd2312: set freq %d Hz\n", freq);

	ret = usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
		CYUSB_HD2312_SET_FREQ,
		USB_TYPE_VENDOR,
		0xFE, 0, buf, 4, 500);

	state->frequency = fe->dtv_property_cache.frequency;

	kfree(buf);
	return ret == 4;
}

static int hd2312_get_tune_settings(struct dvb_frontend *fe, struct dvb_frontend_tune_settings *fe_tune_settings)
{
	fe_tune_settings->min_delay_ms = 1000;
	fe_tune_settings->step_size = 0;
	fe_tune_settings->max_drift = 0;
	return 0;
}

static int hd2312_get_frontend(struct dvb_frontend *fe,
				   struct dtv_frontend_properties *c)
{
	struct hd2312_state *state = fe->demodulator_priv;
	struct usb_device *dev;
	int ret;

	u8 *data = state->status;

	c->frequency = state->frequency;

	dev = state->dev->udev;

	ret = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
		CYUSB_HD2312_GET_FRONTEND,
		USB_TYPE_VENDOR | USB_DIR_IN,
		0xFE, 0, data, 6, 500);

	if (ret != 6)
		return 0;

	pr_debug("%s: fe status: %02X, %02X, %02X, %02X, %02X, %02X\n", __func__,
		data[0], data[1], data[2], data[3] , data[4], data[5]);

	switch (data[0]) {
		case 0:
			c->transmission_mode = TRANSMISSION_MODE_C1;
			break;
		case 1:
			c->transmission_mode = TRANSMISSION_MODE_C3780;
			break;
	}

	switch (data[1]) {
		case 0:
			c->guard_interval = GUARD_INTERVAL_PN945;
			break;
		case 1:
			c->guard_interval = GUARD_INTERVAL_PN595;
			break;
		case 2:
			c->guard_interval = GUARD_INTERVAL_PN420;
			break;
	}

	switch (data[2]) {
		case 0:
			c->fec_inner = FEC_2_5;
			break;
		case 1:
			c->fec_inner = FEC_3_5;
			break;
		case 2:
			c->fec_inner = FEC_4_5;
			break;
	}

	switch (data[3]) {
		case 0:
			c->interleaving = INTERLEAVING_720;
			break;
		case 1:
			c->interleaving = INTERLEAVING_240;
			break;
	}

	switch (data[4]) {
		case 0:
			c->modulation = QAM_4_NR;
			break;
		case 1:
			c->modulation = QPSK;
			break;
		case 2:
			c->modulation = QAM_16;
			break;
		case 3:
			c->modulation = QAM_32;
			break;
		case 4:
			c->modulation = QAM_64;
			break;
	}

	return 0;
}

static int hd2312_read_status(struct dvb_frontend *fe, enum fe_status *status)
{
	struct hd2312_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	struct usb_device *dev;
	int ret;

	u8 *isLock = &state->islock;
	u8 *buf = state->statistics;

	*status = 0;

	dev = state->dev->udev;

	ret = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
		CYUSB_HD2312_GET_LOCK,
		USB_TYPE_VENDOR | USB_DIR_IN,
		0xFE, 0, isLock, 1, 500);

	pr_debug("%s: isLock: %02X\n", __func__, *isLock);

	if (ret == 1 && *isLock == 0x1) {
		*status |= FE_HAS_SIGNAL | FE_HAS_CARRIER |
			FE_HAS_VITERBI | FE_HAS_SYNC | FE_HAS_LOCK;
	}

	state->fe_status = *status;

	/* fe is not lock, set signal strength and snr to not available */

	if (!(state->fe_status & FE_HAS_LOCK)) {
		c->strength.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
		c->cnr.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
		return 0;
	}

	/* get signal strength */
	ret = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
		CYUSB_HD2312_GET_STRENGTH,
		USB_TYPE_VENDOR | USB_DIR_IN,
		0xFE, 0, buf, 4, 500);

	if (ret == 4) {
		pr_debug("%s: strength: %02X, %02X, %02X, %02X\n", __func__, buf[0], buf[1], buf[2], buf[3]);

		c->strength.stat[0].scale = FE_SCALE_RELATIVE;
		/* scale to 0xffff */
		c->strength.stat[0].uvalue = buf[3] * 0xffff / 100;
	}

	/* get snr */
	ret = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
		CYUSB_HD2312_GET_SNR,
		USB_TYPE_VENDOR | USB_DIR_IN,
		0xFE, 0, buf, 2, 500);

	if (ret == 2) {
		pr_debug("%s: snr: %02X, %02X\n", __func__, buf[0], buf[1]);

		c->cnr.stat[0].scale = FE_SCALE_DECIBEL;
		c->cnr.stat[0].svalue = (buf[0] * 1000) + (buf[1] * 10);
	}

	return 0;
}

static int hd2312_read_ucblocks(struct dvb_frontend *fe, u32 *ucblocks)
{
	*ucblocks = 0;
	return 0;
}

static const struct dvb_frontend_ops hd2312_ops = {
	.delsys = { SYS_DVBT },
	.info = {
		.name = "HDIC HD2312",
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 15, 0)
		.frequency_min_hz = 474 * MHz,
		.frequency_max_hz = 858 * MHz,
		.frequency_stepsize_hz = 10000,
#else
		.frequency_min = 474000000,
		.frequency_max = 858000000,
		.frequency_stepsize = 10000,
#endif
		.caps = FE_CAN_FEC_AUTO | FE_CAN_QAM_AUTO | FE_CAN_TRANSMISSION_MODE_AUTO 
			| FE_CAN_BANDWIDTH_AUTO | FE_CAN_GUARD_INTERVAL_AUTO | FE_CAN_HIERARCHY_AUTO,
	},
	.release = hd2312_release,
	.init = hd2312_init,
	.set_frontend = hd2312_set_frontend,
	.get_tune_settings = hd2312_get_tune_settings,
	.get_frontend = hd2312_get_frontend,
	.read_status = hd2312_read_status,
	.read_ucblocks = hd2312_read_ucblocks,
};

MODULE_DESCRIPTION("hd2312-fe");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hanwckf <hanwckf@vip.qq.com>");
