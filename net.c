#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "platform.h"

#include "util.h"
#include "net.h"


/* NOTE: If you want to add/delete the entries after net_run(), you need to protect these lists with a mutex. */
static struct net_device *devices;


struct net_device *net_device_alloc(void) {
    struct net_device *dev;

    dev = memory_alloc(sizeof(struct net_device));
    if (dev == NULL) {
        errorf("memory_alloc() failure");
        return NULL;
    }

    return dev;
}

/* NOTE: must not be call after net_run() */
int net_device_register(struct net_device *dev) {
    static unsigned int index = 0;

    dev->index = index++;
    snprintf(dev->name, sizeof(dev->name), "net%d", dev->index);
    dev->next = devices;
    devices = dev;

    infof("device(name=%s, type=0x%04x) has been successfully registered", dev->name, dev->type);
    return 0;
}


static int net_device_open(struct net_device *dev) {
    if (NET_DEVICE_IS_UP(dev)) {
        errorf("trying to open device(name=%s), but it's already opened", dev->name);
        return -1;
    }

    if (dev->ops->open) {
        if (dev->ops->open(dev) == -1) {
            errorf("failed to open device(name=%s)", dev->name);
            return -1;
        }
    }
    dev->flags |= NET_DEVICE_FLAG_UP;

    infof("device(name=%s, state=%s) has been successfully opened", dev->name, NET_DEVICE_STATE(dev));
    return 0;
}

static int net_device_close(struct net_device *dev) {
    if (!NET_DEVICE_IS_UP(dev)) {
        errorf("device(name=%s) is not opened", dev->name);
        return -1;
    }

    if (dev->ops->close) {
        if (dev->ops->close(dev) == -1) {
            errorf("failed to close device(name=%s)", dev->name);
            return -1;
        }
    }
    dev->flags &= ~NET_DEVICE_FLAG_UP;

    infof("device(name=%s, state=%s) has been successfully closed", dev->name, NET_DEVICE_STATE(dev));
    return 0;
}


int net_device_output(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst) {
    if (!NET_DEVICE_IS_UP(dev)) {
        errorf("device(name=%s) is not opened", dev->name);
        return -1;
    }

    if (len > dev->mtu) {
        errorf("data length(%zu) is over MTU(%u) of device(name=%s)", len, dev->mtu, dev->name);
        return -1;
    }

    debugf("data(type=0x%04x, len=%zu) will be output from device(name=%s)", type, len, dev->name);
    debugdump(data, len);

    if (dev->ops->transmit(dev, type, data, len, dst) == -1) {
        errorf("failed to transmit data(len=%zu) from device(name=%s)", len, dev->name);
        return -1;
    }

    return 0;
}

// pass the packet to the protocol stack when the device receives a packet
int net_input_handler(uint16_t type, const uint8_t *data, size_t len, struct net_device *dev) {
    debugf("Received data(type=0x%04x, len=%zu) on device(name=%s)", type, len, dev->name);
    debugdump(data, len);

    return 0;
}


int net_run(void) {
    struct net_device *dev;

    debugf("open all devices...");
    for (dev = devices; dev != NULL; dev = dev->next) {
        net_device_open(dev);
    }

    debugf("running...");
    return 0;
}

void net_shutdown(void) {
    struct net_device *dev;

    debugf("close all devices...");
    for (dev = devices; dev != NULL; dev = dev->next) {
        net_device_close(dev);
    }

    debugf("shutting down");
}

int net_init(void) {
    infof("initialized");
    return 0;
}

