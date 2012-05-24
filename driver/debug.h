#ifndef _DOORLOCK_DEBUG_H_
#define _DOORLOCK_DEBUG_H_

#ifdef DEBUG_DOORLOCK
#include <linux/kernel.h>
#define DL_DEBUG(msg, args...) do { printk(KERN_DEBUG "[%s:%d] " msg , __func__, __LINE__, ##args); } while (0)
#define DL_ASSERT(expr) if (!(expr)) { printk(KERN_ERR "[%s:%d] DL_ASSERTION FAILED! %s\n", __func__, __LINE__, #expr); dump_stack(); }
#else
#define DL_DEBUG(msg, args...)
#define DL_ASSERT(expr)
#endif
#endif

