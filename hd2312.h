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

#endif
