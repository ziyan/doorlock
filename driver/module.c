#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>

#include "doorlock.h"
#include "gn.h"
#include "usb.h"

int gid = 0;

//
// Module loading and unloading.
//

static int __init doorlock_module_init(void)
{
	int err = 0;

	err = gn_init();
	if (err)
	{
		goto failure_gn;
	}
	
	err = usb_init();
	if (err)
	{
		goto failure_usb;
	}

	DL_DEBUG("module loaded. gid = %d\n", gid);

failure_usb:
	gn_exit();
failure_gn:
	return err;
}

static void __exit doorlock_module_exit(void)
{
	usb_exit();
	gn_exit();
	DL_DEBUG("module unloaded.\n");
}

MODULE_AUTHOR(DOORLOCK_AUTHOR);
MODULE_LICENSE(DOORLOCK_LICENSE);
MODULE_DESCRIPTION(DOORLOCK_DESCRIPTION);
MODULE_VERSION(DOORLOCK_VERSION);

module_init(doorlock_module_init);
module_exit(doorlock_module_exit);
module_param(gid, int, 0);

