
#if !defined(XYMODEM_H)
#define XYMODEM_H

#define SOH		0x01
#define STX		0x02
#define EOT		0x04
#define ACK		0x06
#define NAK		0x15
#define CAN		0x18

#define XYSTAT_ERROR	0
#define XYSTAT_BINARY	1
#define XYSTAT_TEXT		2

typedef struct transinfo {
	char filename[FILENAME_MAX + 1];
	char status;
	long filesize;
	long timestamp;
} transinfo;

#endif	/* XYMODEM_H */

