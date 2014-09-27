#include "Queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void testQueueSimple()
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
    assert(peek(q[0], 1) == 1);
    assert(q[0]->head->playerloc == 1 && q[0]->tail->playerloc == 1 && q[0]->size == 1);

    push(q[0], 2);
    assert(peek(q[0], 1) == 2);
    assert(q[0]->head->playerloc == 1 && q[0]->tail->playerloc == 2 && q[0]->size == 2);

    assert(pop(q[0]) == 1);
    assert(peek(q[0], 1) == 2);
    assert(q[0]->head->playerloc == 2 && q[0]->tail->playerloc == 2 && q[0]->size == 1);
}

void testQueueMed()
{
    TrapVam tpvs = malloc(sizeof(trapVam));
    tpvs->trap = init();
    tpvs->vam = init();

    puts("testing NULL traps");
    assert(trapNum(tpvs->trap, 1) == 0);
    assert(trapNum(tpvs->vam, 1) == 0);

    puts("\ntesting 1 trap");
    push(tpvs->trap, 12);
    push(tpvs->trap, 13);
    push(tpvs->trap, 1);
    push(tpvs->trap, 2);
    assert(trapNum(tpvs, 12) == 1);
    assert(trapNum(tpvs->trap, 13) == 1);
    assert(trapNum(tpvs->trap, 1) == 1);
    assert(trapNum(tpvs->trap, 2) == 1);
    assert(trapNum(tpvs->trap, 99) == 0);

    puts("\ntesting 2 traps");
    push(tpvs->trap, 12);
    assert(trapNum(tpvs->trap, 12) == 2);
    assert(trapNum(tpvs->trap, 13) == 1);
    assert(trapNum(tpvs->trap, 1) == 1);
    assert(trapNum(tpvs->trap, 2) == 1);
    assert(trapNum(tpvs->trap, 99) == 0);

    puts("\ntesting 3 traps");
    push(tpvs->trap,99);
    push(tpvs->trap, 12);
    push(tpvs->trap, 93);
    assert(trapNum(tpvs->trap, 12) == 3);
    assert(trapNum(tpvs->trap, 13) == 1);
    assert(trapNum(tpvs->trap, 1) == 1);
    assert(trapNum(tpvs->trap, 2) == 1);
    assert(trapNum(tpvs->trap, 99) == 1);

    puts("\ntesting Free");
    freeTravm(tpvs);
}
