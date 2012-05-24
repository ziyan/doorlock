#include <net/genetlink.h>

#include "doorlock.h"
#include "gn.h"

static int gn_access_control(struct sk_buff *skb, struct genl_info *info);
static int gn_rx(struct sk_buff *skb, struct genl_info *info);
static int gn_tx(u32 pid, u8 *data, int len);
static int gn_running(u32 pid);

//
// Attribute definitions.
//

enum {
	DOORLOCK_A_UNSPEC,
	DOORLOCK_A_IFADDR,
	DOORLOCK_A_IFNAME,
	DOORLOCK_A_DATA,
	__DOORLOCK_A_MAX,
};
#define DOORLOCK_A_MAX (__DOORLOCK_A_MAX - 1)

//
// Family definitions.
//

static struct genl_family gn_family = {
	.id = GENL_ID_GENERATE,
	.hdrsize = 0,
	.name = "doorlock",
	.version = DOORLOCK_VERSION_NR,
	.maxattr = DOORLOCK_A_MAX,
};

//
// Command definitions.
//

enum {
	DOORLOCK_C_UNSPEC,
	DOORLOCK_C_RX,
	DOORLOCK_C_TX,
	DOORLOCK_C_NOOP,
	__DOORLOCK_C_MAX,
};
#define DOORLOCK_C_MAX (__DOORLOCK_C_MAX - 1)

//
// Rx command.
//

static struct nla_policy gn_policy_rx[DOORLOCK_A_MAX + 1] = {
	[DOORLOCK_A_DATA] = { .type = NLA_BINARY, .len = 24 },
};

static struct genl_ops gn_ops_rx = {
	.cmd = DOORLOCK_C_RX,
	.flags = 0,
	.policy = gn_policy_rx,
	.doit = gn_rx,
	.dumpit = NULL,
};

//
// Init and exit functions.
//

int gn_init(void)
{
	int err = 0;

	err = genl_register_family(&gn_family);
	if (err)
	{
		goto failure_family;
	}

	err = genl_register_ops(&gn_family, &gn_ops_rx);
	if (err)
	{
		goto failure_ops_rx;
	}

	return 0;

failure_ops_rx:
	genl_unregister_family(&gn_family);
failure_family:
	return err;
}

void gn_exit(void)
{
	genl_unregister_ops(&gn_family, &gn_ops_rx);
	genl_unregister_family(&gn_family);
}

//
// Access control.
//

static int gn_access_control(struct sk_buff *skb, struct genl_info *info)
{
	if (!skb || !info) return -EINVAL;
	DL_DEBUG("pid = %d (%d), uid = %d, gid = %d\n", info->snd_pid, NETLINK_CB(skb).pid, NETLINK_CREDS(skb)->uid, NETLINK_CREDS(skb)->gid);
	if (NETLINK_CREDS(skb)->gid != gid) return -EACCES;
	return 0;
}

//
// Handlers.
//


static int gn_rx(struct sk_buff *skb, struct genl_info *info)
{
	int err = 0;
	
	err = gn_access_control(skb, info);
	if (err)
	{
		return err;
	}

	if (!info->attrs[DOORLOCK_A_DATA] || nla_len(info->attrs[DOORLOCK_A_DATA]) != 24)
	{
		err = -EINVAL;
		goto failure;
	}
	//return netdev_rx(info->snd_pid, nla_data(info->attrs[DOORLOCK_A_DATA]), nla_len(info->attrs[DOORLOCK_A_DATA]));

failure:
	return err;
}

static int gn_tx(u32 pid, u8 *data, int len)
{
	int err = 0;
	struct sk_buff *skb = NULL;
	void *msg_head = NULL;

	if (len != 24)
	{
		return -EINVAL;
	}

	skb = genlmsg_new(len, GFP_KERNEL);
	if (!skb)
	{
		return -ENOMEM;
	}

	msg_head = genlmsg_put(skb, 0, 0, &gn_family, 0, DOORLOCK_C_TX);
	if (!msg_head)
	{
		err = -ENOMEM;
		goto failure;
	}

	err = nla_put(skb, DOORLOCK_A_DATA, len, data);
	if (err)
	{
		goto failure;
	}

	err = genlmsg_end(skb, msg_head);
	if (err < 0)
	{
		goto failure;
	}

	err = genlmsg_unicast(skb, pid);
	return err;

failure:
	nlmsg_free(skb);
	return err;
}

static int gn_running(u32 pid)
{
	int err = 0;
	struct sk_buff *skb = NULL;
	void *msg_head = NULL;

	skb = genlmsg_new(0, GFP_KERNEL);
	if (!skb)
	{
		return -ENOMEM;
	}

	msg_head = genlmsg_put(skb, 0, 0, &gn_family, 0, DOORLOCK_C_NOOP);
	if (!msg_head)
	{
		err = -ENOMEM;
		goto failure;
	}

	err = genlmsg_end(skb, msg_head);
	if (err < 0)
	{
		goto failure;
	}

	err = genlmsg_unicast(skb, pid);
	return err == 0 ? 1 : 0;

failure:
	nlmsg_free(skb);
	return 0;
}


