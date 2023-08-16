#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "platform.h"

#include "util.h"
#include "net.h"


#define DUMMY_MTU UINT16_MAX /* maximum size of IP datagram */

#define DUMMY_IRQ INTR_IRQ_BASE


static int dummy_transmit(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst) {
    debugf("data(type=0x%04x, len=%zu), device(name=%s)", type, len, dev->name);
    debugdump(data, len);

    /* drop data */
    intr_raise_irq(DUMMY_IRQ);

    return 0;
}


static int dummy_isr(unsigned int irq, void *id) {
    struct net_device *dev = (struct net_device *)id;

    debugf("received irq(number=%u, name=%s)", irq, dev->name);

    return 0;
}


static struct net_device_ops dummy_ops = {
    .transmit = dummy_transmit
};

struct net_device *dummy_init(void) {
    struct net_device *dev;

    dev = net_device_alloc();
    if (dev == NULL) {
        errorf("net_device_alloc() failed");
        return NULL;
    }

    dev->type = NET_DEVICE_TYPE_DUMMY;
    dev->mtu = DUMMY_MTU;
    dev->hlen = 0;  /* no header */
    dev->alen = 0;  /* no address */
    dev->ops = &dummy_ops;
    if (net_device_register(dev) == -1) {
        errorf("net_device_register() failure");
        return NULL;
    }

    intr_request_irq(DUMMY_IRQ, dummy_isr, INTR_IRQ_SHARED, dev->name, dev);

    debugf("dev(name=%s) has been successfully initialized", dev->name);
    return dev;
}

