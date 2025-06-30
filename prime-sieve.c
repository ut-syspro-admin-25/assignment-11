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

#define N       100

static void sink(int rfd) {
        int err, n, prime = 0;
        int fds[2], pid = -1;

        for (;;) {
                syscall_piperead(rfd, (char*)&n, sizeof n);

                if (n >= N)
                        break;

                if (prime) {
                        if (n % prime == 0)
                                continue;
                        syscall_pipewrite(fds[1], (char*)&n, sizeof n);
                } else {
                        prime = n;
                        syscall_puts("pid: ");
                        putn32(syscall_getpid(), 10, 0);
                        syscall_puts(" prime number: ");
                        putn32(prime, 10, 0);
                        syscall_puts("\n");
                }

                if (pid < 0) {
                        syscall_pipe(fds);
                        pid = syscall_fork();
                        if (pid == 0) {
                                syscall_pipeclose(fds[1]);
                                sink(fds[0]);
                        } else {
                                syscall_pipeclose(fds[0]);
                        }
                }
        }
}

static void source(int wfd) {
        for (int i = 2; ; i++) {
                syscall_pipewrite(wfd, (char*)&i, sizeof i);
        }
}

void main(void) {
        int fds[2];
        int pid, status;
        
        syscall_pipe(fds);

        pid = syscall_fork();

        if (pid == 0) {
                syscall_pipeclose(fds[1]);
                sink(fds[0]);
        } else {
                syscall_pipeclose(fds[0]);
                source(fds[1]);
        }
        syscall_exit(0);
}
