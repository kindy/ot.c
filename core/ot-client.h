#ifndef _OT_CLIENT_H
#define _OT_CLIENT_H

#include "ops.h"

int ot_client_init();
void ot_client_free();

void ot_client_drain();
void ot_client_put_user_op(operation *);
void ot_client_put_serv_msg(message *);

void print_pend(FILE *);

#endif /* _OT_CLIENT_H */
