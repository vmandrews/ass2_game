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

qlist init()
{
    qlist newq = malloc(sizeof(queue));
    newq->head = newq->tail = NULL;
    newq->size = 0;
    return newq;
}

int isNotEmpty(qlist q)
{
    assert(q!=NULL);
    return q->size;
}

LocationID spop(qlist q)
{
    assert(q!=NULL);
    assert(q->tail!=NULL);

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
    assert(q!=NULL);
    assert(q->head!=NULL);
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
    assert(q!=NULL);
    if (q->tail == NULL)return -2;
    if (n == 1)return q->tail->playerloc;

    int i = 0;
    node *curr = q->head;
    for (; i < q->size - n && q->size > n && curr != NULL; i++, curr = curr->next);
    return curr->playerloc;
}

void push(qlist q, LocationID it)
{
    assert(q!=NULL);
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

void removeDuplicate(qlist q)
{
    qlist temp = init();
    LocationID place;
    int numDup,i;
    while (isNotEmpty(q)){
        place = pop(q);
        if ((numDup = trapNum(q, place))){
            for (i = 0; i < numDup; i++)movetrap(q, place);
        }
        push(temp, place);
    }

    while (isNotEmpty(temp)){
        push(q, pop(temp));
    }
    free(temp);
}

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
int trapNum(qlist tv, LocationID place)
{
    int trapNo = 0;
    node *curr = tv->head;

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
        int traps = trapNum(tv->trap, place), i = 0;
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
        int traps = trapNum(tv->vam, place), i = 0;
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
    free(curr);
    free(ts->trap);

    curr = ts->vam->head;
    temp = curr;
    while (curr != NULL){
        temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(curr);
    free(ts->vam);
}
