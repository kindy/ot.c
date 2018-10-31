#ifndef _OPS_H
#define _OPS_H

#include <stdint.h>

#define COLLAB_MAX 64

enum optype {
        INSERT,
        REMOVE,
        NULLOP
};

typedef struct {
        enum optype type;
        char c;
        uint32_t pos;
} op;

typedef struct {
        uint32_t pid;
        uint32_t s[COLLAB_MAX];
        op o;
} operation;

op
op_transform(op a, op b, int pa, int pb);

char
comp_operation(void *, void *);

void
op_perform(operation *op);

#endif /* _OPS_H */
