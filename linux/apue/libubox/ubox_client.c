#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <libubox/usock.h>

#include "libubox/uloop.h"

int c_fd;
struct uloop_timeout t;

void timeout_callback(struct uloop_timeout *t);

void timeout_callback(struct uloop_timeout *t)
{
    send(c_fd, "helloworld222", 10, 0);
    uloop_timeout_set(t, 1000);
}

int main()
{
    //struct sockaddr cli_addr;
    //socklen_t len = sizeof(struct sockaddr);
    int type = USOCK_TCP  | USOCK_NOCLOEXEC | USOCK_IPV4ONLY;
    const char *host = "192.168.1.111";
    const char *service = "8000";
    //char recv_buf[1024] = {-1};
    c_fd = usock(type, host, service);    /* create a linker socket*/
    if (c_fd < 0) {
        perror("usock");
        return -1;
    }
    uloop_init();
    t.cb = timeout_callback;
    uloop_timeout_add(&t);
    uloop_timeout_set(&t, 1000);
    //send(c_fd, "helloworld", 10, 0);

    uloop_run();
    uloop_done();
    close(c_fd);
    return 0;
}
