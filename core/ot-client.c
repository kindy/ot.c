#include <stdlib.h>
#include <string.h>

#include "ot-client.h"
#include "ops.h"
#include "queue.h"

static queue *pend, *outq;
static char ot_client_inflight = 0;
static int pid;

/*
 * Print the pending operation queue the the file stream.
 * Meant for debugging purposes.
 */
void
print_pend(FILE *f)
{
        int i;
        operation *op;

        fprintf(f, "{");
        for (i=0; i<pend->n; i++) {
                op = pend->arr[i];
                fprintf(f, "(%d, %c, %u), ", op->type, op->c, op->pos);
        }
        fprintf(f, "}\n");
}

/*
 * Initialize this module.
 */
int
ot_client_init(int _pid)
{
        pend = q_alloc(8);
        pid = _pid;

        return 0;
}

/*
 * Destroy this module.
 */
void
ot_client_free()
{
        q_free(pend);
}

/*
 * Receive an operation to apply immediately from the ui.
 */
static void
ot_client_put_user_msg(message *msg)
{
        operation *op = malloc(sizeof(operation));
        *op = msg->op;

        // push to pending operations
        q_push(pend, op);
}

/*
 * Receive an operation from the server to transform pending operations against.
 */
static void
ot_client_put_serv_msg(message *msg)
{
        operation *pop, newop;
        message *newMsg;
        int i;

        if (pid == msg->pid) {
                ot_client_inflight = 0;
                free(q_pop(pend));
                return;
        }

        newop = msg->op;

        // transform all operations in 'pend' against op
        for (i=0; i<pend->n; i++) {
                pop = pend->arr[i];
                newop = op_transform(newop, *pop, msg->pid, pid);
                *pop = op_transform(*pop, msg->op, pid, msg->pid);
                msg->op = newop;
        }

        newMsg = malloc(sizeof(message));
        newMsg->pid = -1;
        newMsg->rev = 0;
        newMsg->op.type = newop.type;
        newMsg->op.c = newop.c;
        newMsg->op.pos = newop.pos;

        q_push(outq, newMsg);
}

/*
 * Get the next pending operation to send to the server.
 * If there is already an operation in flight, or the
 * pending queue is empty, do nothing.
 */
static void
ot_client_drain()
{
        operation *op;
        message *msg;

        if (NULL != (op = q_peek(pend)) && 0 == ot_client_inflight) {
                if (NULLOP != op->type) {
                        ot_client_inflight = 1;

                        msg = malloc(sizeof(message));
                        msg->pid = pid;
                        msg->rev = revision;
                        msg->op.type = op->type;
                        msg->op.c = op->c;
                        msg->op.pos = op->pos;

                        q_push(outq, msg);
                }
                else {
                        free(q_pop(pend));
                }
        }
}

void
ot_client_consume(FILE *stream)
{
        message msg;

        while (5 == fscanf(stream, "%d,%u,%d,%c,%u", &msg.pid, &msg.rev,
                                        &msg.op.type, &msg.op.c, &msg.op.pos)) {
                if (-2 == msg.pid)
                        break;

                if (-1 == msg.pid)
                        ot_client_put_user_msg(&msg);
                else
                        ot_client_put_serv_msg(&msg);
        }
}

void
ot_client_produce(FILE *stream)
{
        message *msg;

        ot_client_drain();

        while (NULL != (msg = q_pop(outq))) {
                fprintf(stream, "%d,%u,%d,%c,%u\n", msg->pid, msg->rev,
                                                msg->op.type, msg->op.c, msg->op.pos);
                fflush(stream);
                free(msg);
        }
}
