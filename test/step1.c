#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "util.h"
#include "net.h"

#include "driver/dummy.h"

#include "test.h"

static volatile sig_atomic_t terminates;

static void on_signal(int s) {
    (void)s;
    terminates = 1;
}

int main (int argc, char *argv[]) {
    return 0;
}

