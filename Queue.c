//=================================================================================================================//
//==================================================== Q used for trail ===========================================//
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Queue.h"

typedef struct queue{
    node *head;
    node *tail;
    int size;
}queue;

typedef struct trapVam{
    qlist trap;
    qlist vam;
}trapVam;

qlist init()
{
    qlist newq = malloc(sizeof(queue));
    newq->head = newq->tail = NULL;
    newq->size = 0;
    return newq;
}

int isNotEmpty(qlist q)
{
    return q->size;
}

LocationID spop(qlist q)
{
    LocationID dest = q->tail->playerloc;
    node *todel = q->tail;
    if (!todel)return -2;
    if (q->head == q->tail)q->head = q->tail = NULL;
    else {
        q->tail = q->tail->prev;
        q->tail->next = NULL;
    }
    free(todel);
    q->size--;
    return dest;
}


LocationID pop(qlist q)
{
    node *todel = q->head;
    LocationID dest = q->head->playerloc;
    if (!q->head)return -2;
    if (q->head == q->tail){
        q->head = q->tail = NULL;
    } else {
        q->head = q->head->next;
        q->head->prev = NULL;
    }
    free(todel);
    q->size--;
    return dest;
}

LocationID peek(qlist q, int n)
{
    if (q->tail == NULL)return -2;
    if (n == 1)return q->tail->playerloc;

    int i = 0;
    node *curr = q->head;
    for (; i < q->size - n && q->size > n && curr != NULL; i++, curr = curr->next);
    return curr->playerloc;
}

void push(qlist q, LocationID it)
{
    node *n = malloc(sizeof(node));
    n->playerloc = it; n->next = NULL; n->prev = NULL;
    if (q->size == 0){
        q->head = q->tail = n;
        q->size++;
        return;
    }
    
    n->prev = q->tail;
    q->tail->next = n;
    q->tail = n;
    q->size++;
}

#if(0)
void testQueue()
{
    qlist q[4];
    for (int i = 0; i < 4; i++){
        q[i] = init();
    }
    assert(q[0]->head == NULL && q[0]->tail == NULL && q[0]->size == 0);
    assert(q[1]->head == NULL && q[1]->tail == NULL && q[1]->size == 0);
    assert(q[2]->head == NULL && q[2]->tail == NULL && q[2]->size == 0);
    assert(q[3]->head == NULL && q[3]->tail == NULL && q[3]->size == 0);
    assert(isNotEmpty(q[0]) == 0);

    push(q[0], 1);
    assert(peek(q[0],1) == 1);
    assert(q[0]->head->playerloc == 1 && q[0]->tail->playerloc == 1 && q[0]->size == 1);

    push(q[0], 2);
    assert(peek(q[0], 1) == 2);
    assert(q[0]->head->playerloc == 1 && q[0]->tail->playerloc == 2 && q[0]->size == 2);
    
    assert(pop(q[0]) == 1);
    assert(peek(q[0], 1) == 2);
    assert(q[0]->head->playerloc == 2 && q[0]->tail->playerloc == 2 && q[0]->size == 1);

    push(q[0], 1);
    push(q[0], 3);
    push(q[0], 4);
    push(q[0], 5);
    push(q[0], 6);
    push(q[0], 7);

    assert(q[0]->head->playerloc = 1 && q[0]->tail->playerloc == 7 && q[0]->size == 6);
    assert(spop(q[0]) == 7);
    assert(q[0]->tail->playerloc == 6 && peek(q[0], 1) == 6 && q[0]->size == 5);
    assert(peek(q[0], 2) == 5);
    assert(peek(q[0], 3) == 4);
    assert(peek(q[0], 4) == 3);
    assert(peek(q[0], 5) == 1);
}
#endif
//============================================================= Q ends Here ================================================//
//==========================================================================================================================//

//==========================================================================================================================//
//====================================================== Functions for trap ================================================//

//add trap
void pushtrap(qlist q, node *newt)
{
    if (q->head == q->tail){
        q->head =q->tail = newt;
    } else{
        q->tail->next = newt;
        newt->prev = q->tail;
        q->tail = newt;
    }
}

//remove trap
void movetrap(qlist q, LocationID place)
{
    node *curr = q->head;
    if (curr == NULL)return;
    if (curr->next == NULL){
        q->head = q->tail = NULL;
        free(curr);
        return;
    }

    for (;; curr = curr->next){
        if (curr->playerloc == place)break;
    }

    curr->prev->next = curr->next;
    if (curr->next != NULL){
        curr->next->prev = curr->prev;
    }
    free(curr);
}


//0 = trap, 1 = vampire
int trapNum(TrapVam tv, LocationID place, int trapType)
{
    int trapNo = 0;
    node *curr = NULL;
    if (trapType){
        curr = tv->vam->head;
    }else{
        curr = tv->trap->head;
    }

    for (;curr != NULL;curr = curr->next){
        if (curr->playerloc == place)trapNo++;
    }
    return trapNo;
}

//type indicates if the trap was encountered (so dismissed)
//or placed (so increment) 0 = encounter, 1 = place,2 = mature
//traptype 0 = trap,1 = vampire
void updateTrap(TrapVam tv,LocationID place,int type)
{
    if (type == 0){
        int traps = trapNum(tv, place, 0), i = 0;
        for (; i < traps; i++){
            movetrap(tv->trap, place);
        }
    } else if (type == 1){
        node *newt = malloc(sizeof(node));
        newt->next = newt->prev = NULL;
        newt->playerloc = place;
        pushtrap(tv->trap, newt);
    } else{
        pop(tv->trap);
    }
}

//type 0 = hunter encounter, 1 = vampire place, 2 = vampire mature
void updateVam(TrapVam tv, LocationID place, int type)
{
    if (type == 0){
        int traps = trapNum(tv, place, 1), i = 0;
        for (; i < traps; i++){
            movetrap(tv->vam, place);
        }
    } else if (type == 1){
        node *newt = malloc(sizeof(node));
        newt->next = newt->prev = NULL;
        newt->playerloc = place;
        pushtrap(tv->vam, newt);
    } else{
        pop(tv->vam);
    }
}


void freeTravm(TrapVam ts)
{
    node *curr = ts->trap->head;
    node *temp = curr;
    while (curr != NULL){
        temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(temp);
    free(ts->trap);

    curr = ts->vam->head;
    temp = curr;
    while (curr != NULL){
        temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(temp);
    free(ts->vam);
}
