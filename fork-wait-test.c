#include "syscalls.h"

static void putn32 (int num, int base, int sign) {
        char buf[sizeof (num) * 8 + 1] = {0};
        char *end = buf + sizeof (buf);
        char *cur = end - 1;
        int neg = 0;
        unsigned int unum;

        if (sign && num < 0) {
                unum = (unsigned int)(-(num + 1)) + 1;
                neg = 1;
        } else {
                unum = (unsigned int)num;
        }
        do {
                *--cur = "0123456789abcdef"[unum % base];
        } while (unum /= base);

        if (neg) {
                *--cur = '-';
        }

        syscall_puts (cur);
}

void main(void) {
        char buf[128];
        int n, pid, status;

        for (int i = 0; i < 10; i++) {
                pid = syscall_fork();
                if (pid == 0)
                        syscall_exit(i);
        }

        syscall_puts("fork done\n");

        for (int i = 0; i < 10; i++) {
                if (syscall_wait(&status) < 0) {
                        syscall_puts("error!\n");
                }
                syscall_puts("child exit: ");
                putn32(status, 10, 0);
                syscall_puts("\n");
        }

        syscall_puts("wait done\n");

        if (syscall_wait(&status) != -1) {
                syscall_puts("error!\n");
        }

        syscall_puts("ok\n");

        for (;;)
                ;
}

