#ifndef H_QUEUE_H
#define H_QUEUE_H

#include "Places.h"
#include "GameView.h"

typedef struct queue *qlist;
typedef struct trapvam *TrapVam;

typedef struct node{
    LocationID playerloc;
    int trapNum;
    struct node *next;
    struct node *prev;
}node;

typedef struct queue{
    node *head;
    node *tail;
    int size;
}queue;

typedef struct trapVam{
    queue *trap;
    queue *vam;
}trapVam;


qlist init();
int isNotEmpty(qlist q);
LocationID spop(qlist q);
LocationID pop(qlist q);
LocationID peek(qlist q, int n);
void push(qlist q, LocationID it);
void pushtrap(qlist q, node *newt);
void movetrap(qlist q, LocationID place);
int trapNum(TrapVam tv, LocationID place, int trapType);
void updateTrap(TrapVam tv,LocationID place,int type);
void updateVam(TrapVam tv, LocationID place, int type);
void freeTravm(TrapVam ts);

#endif
