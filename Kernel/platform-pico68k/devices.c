#include <kernel.h>
#include <version.h>
#include <kdata.h>
#include <tinydisk.h>
#include <tinysd.h>
#include "printf.h"
#include <devsys.h>
#include <tty.h>

struct devsw dev_tab[] =	/* The device driver switch table */
{
// minor    open         close        read      write       ioctl
// -----------------------------------------------------------------
	/* 0: /dev/hd         Disc block devices  */
	{ td_open, no_close, td_read, td_write, td_ioctl },
	/* 1: /dev/fd         Hard disc block devices (absent) */
	{ nxio_open, no_close, no_rdwr, no_rdwr, no_ioctl },
	/* 2: /dev/tty        TTY devices */
	{ tty_open, tty_close, tty_read, tty_write, tty_ioctl },
	/* 3: /dev/lpr        Printer devices */
	{ no_open, no_close, no_rdwr, no_rdwr, no_ioctl },
	/* 4: /dev/mem etc    System devices (one offs) */
	{ no_open, no_close, sys_read, sys_write, sys_ioctl },
	/* Pack to 7 with nxio if adding private devices and start at 8 */
};

bool validdev(uint16_t dev)
{
	/* This is a bit uglier than needed but the right hand side is
	   a constant this way */
	if (dev > ((sizeof(dev_tab) / sizeof(struct devsw)) << 8) - 1)
		return false;
	else
		return true;
}

void device_init(void)
{
	uint8_t t = sd_init();
	if (t == 0)
		return;
	kputs("sd: ");
	if (t & CT_BLOCK)
		sd_shift[0] = 0;
	else
		sd_shift[0] = 9;
	td_register(sd_xfer, 1);
}
