#ifndef __HD2312_H__
#define __HD2312_H__

struct hd2312_state {
	struct dvb_usb_device *dev;
	struct dvb_frontend frontend;
	u32 frequency;
	enum fe_status fe_status;
	u8 islock;
	u8 status[6];
	u8 statistics[4];
};

#define CYUSB_HD2312_SET_FREQ		0xFC
#define CYUSB_HD2312_GET_FRONTEND	0xE7
#define CYUSB_HD2312_GET_SNR		0xE8
#define CYUSB_HD2312_GET_QUALITY	0xE9
#define CYUSB_HD2312_GET_LOCK		0xEA
#define CYUSB_HD2312_GET_STRENGTH	0xEB
#define CYUSB_HD2312_GET_VERSION	0xED

#endif
