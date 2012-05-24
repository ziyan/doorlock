// *************************************************************************************************
// Includes
#include "doorlock.h"
#include "usb.h"

#include <linux/usb.h>

// *************************************************************************************************
// Define section
struct usb_data {
};


// *************************************************************************************************
// Prototypes
int usb_probe(struct usb_interface*, const struct usb_device_id*);
void usb_disconnect(struct usb_interface*);

// *************************************************************************************************
// Global variables
static struct usb_device_id id_table[] = {
	{ USB_DEVICE(DOORLOCK_USB_VENDOR_ID, DOORLOCK_USB_PRODUCT_ID) },
	{ },
};
MODULE_DEVICE_TABLE (usb, id_table);

static struct usb_driver usb_driver = {
	.name = "doorlock",
	.probe = usb_probe,
	.disconnect = usb_disconnect,
	.id_table = id_table,
};


// *************************************************************************************************
// Implementation
int usb_init(void)
{
	int err = 0;
	err = usb_register(&usb_driver);
	return err;
}

void usb_exit(void)
{
	usb_deregister(&usb_driver);
}

int usb_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	return -ENODEV;
}

void usb_disconnect(struct usb_interface *intf)
{
}


