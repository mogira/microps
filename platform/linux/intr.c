#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "platform.h"

#include "util.h"


struct irq_entry {
    struct irq_entry *next;
    unsigned int irq;  // number of IRQ
    irq_handler_t handler;
    int flags;
    char name[16];
    void *dev;
};

/* NOTE: if you want to add/delete the entries after intr_run(), you need to protect these lists with a mutex. */
static struct irq_entry *irqs;


static sigset_t sigmask;

static pthread_t tid;
static pthread_barrier_t barrier;


int intr_request_irq(unsigned int irq, irq_handler_t handler, int flags, const char *name, void *dev) {
}

int intr_raise_irq(unsigned int irq) {
}

static void *intr_thread(void *arg) {
}


int intr_run(void) {
}

void intr_shutdown(void) {
}

int intr_init(void) {
}

