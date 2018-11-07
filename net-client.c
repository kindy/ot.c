#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "doc-client.h"
#include "net.h"
#include "net-client.h"
#include "ops.h"

static int sk;

int
net_client_init()
{
        struct addrinfo hints, *res;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        getaddrinfo("localhost", PORT, &hints, &res);

        if ((sk = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
                perror("Failed to create socket");
                return -1;
        }

        freeaddrinfo(res);

        if (connect(sk, res->ai_addr, res->ai_addrlen)) {
                perror("Failed to connect\n");
                return -1;
        }

        return 0;
}

void
net_client_free()
{
        int i;

        close(sk);
}

void
net_client_send(op *o)
{
        // TODO package op as operation
        // TODO send operation to server
}

void
net_client_drain()
{
        // TODO recv acks, new operations
        // TODO send operations to doc-client
}
