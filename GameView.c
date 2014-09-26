 // GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h" //... if you decide to use the Map ADT

#define TURN_LEN 8

typedef struct gameView {
    int round;
    int score; 
    int healthPoints[NUM_PLAYERS];
    LocationID playerLocations[NUM_PLAYERS][TRAIL_SIZE];
}gameView;

//=================================================================================================================//
//==================================================== Q used for trail ===========================================//

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
}queue, *qlist;

typedef struct trapVam{
    qlist trap;
    qlist vam;
}trapVam, *TrapVam;

qlist init()
{
    qlist newq = malloc(sizeof(queue));
    newq->head = newq->tail = NULL;
    newq->size = 0;
    return newq;
}

int isEmpty(qlist q)
{
    return !q->size;
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
    if (q->size == TRAIL_SIZE) {
        pop(q);
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
    assert(isEmpty(q[0]) == 1);

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
    node *curr = NULL;
    if (trapType){
        curr = tv->vam->head;
    }else{
        curr = tv->trap->head;
    }

    for (;; curr = curr->next){
        if (curr->playerloc == place)break;
    }
    return curr->trapNum;
}

//type indicates if the trap was encountered (so dismissed)
//or placed (so increment) 0 = encounter, 1 = place
//traptype 0 = trap,1 = vampire
void updateTrap(TrapVam tv,LocationID place,int type)
{
    if (type){
        node *loc = tv->trap->head;
        for (; loc != NULL; loc = loc->next){
            if (loc->playerloc == place)break;
        }

        if (loc){
            loc->trapNum++;
        } else{
            node *newtrap = malloc(sizeof(node));
            newtrap->playerloc = place;
            newtrap->trapNum = 1;
            newtrap->next = newtrap->prev = NULL;
            pushtrap(tv->trap, newtrap);
        }
    } else{
        movetrap(tv->trap, place);
    }
}

//type 0 = hunter encounter, 1 = vampire place, 2 = vampire mature
void updateVam(TrapVam tv, LocationID place, int type)
{
    if (type == 1){
        node *loc = tv->vam->head;
        for (; loc != NULL; loc = loc->next){
            if (loc->playerloc == place)break;
        }

        if (loc){
            loc->trapNum++;
        } else{
            node *newtrap = malloc(sizeof(node));
            newtrap->playerloc = place;
            newtrap->trapNum = 1;
            newtrap->next = newtrap->prev = NULL;
            pushtrap(tv->vam, newtrap);
        }
    } else if(type == 2){
        if (trapNum(tv, place, 1) <= 1){
            movetrap(tv->vam, place);
        } else{
            node *oldvam = tv->vam->head;
            for (;; oldvam = oldvam->next)
            if (oldvam->playerloc == place)break;
            oldvam->trapNum--;
        }
    } else{
        movetrap(tv->vam, place);
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

//==========================================================================================================================//
//====================================================== End for trap ======================================================//

//records the action of hunter "player" in one particular turn
void Hunter(GameView gv, qlist q, char *plays, PlayerID player, TrapVam tpvs)
{
    LocationID prevloc = peek(q, 1);
    LocationID currloc = abbrevToID(plays + 1);
    push(q, currloc);
    if (prevloc == currloc && gv->healthPoints[player] < GAME_START_HUNTER_LIFE_POINTS){
        gv->healthPoints[player] += LIFE_GAIN_REST;
        if (gv->healthPoints[player] > GAME_START_HUNTER_LIFE_POINTS)
            gv->healthPoints[player] = GAME_START_HUNTER_LIFE_POINTS;
    }
    if (plays[3] == 'T'){
//        printf("%d triggered a trap, lost 2 life point\n",player);
        gv->healthPoints[player] -= trapNum(tpvs,currloc,0) * LIFE_LOSS_TRAP_ENCOUNTER;
        updateTrap(tpvs, currloc, 0);
//        printf("%d now on -> %d\n", gv->healthPoints[player]);
    }
    if (plays[3] == 'V' || plays[4] == 'V')updateVam(tpvs, currloc, 0);
    if (plays[3] == 'D' || plays[4] == 'D' || plays[5] == 'D'){
//        printf("%d encountered dracula, lost 4 life point\n", player);
        gv->healthPoints[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
        gv->healthPoints[player] -= LIFE_LOSS_DRACULA_ENCOUNTER;
//        printf("%d now on -> %d\n",gv->healthPoints[player]);
    }

    if (gv->healthPoints[player] <= 0){
//        printf("%d died, resurrect with 9 life point\n", player);
        gv->healthPoints[player] = GAME_START_HUNTER_LIFE_POINTS;
        push(q, ST_JOSEPH_AND_ST_MARYS);
//        printf("%d now on -> %d\n", player,gv->healthPoints[player]);
        gv->score -= SCORE_LOSS_HUNTER_HOSPITAL;
    }
}

void Dracula(GameView gv, qlist q, char *play, TrapVam tpvs)
{
    LocationID currlocation = abbrevToID(play+1);
    if (play[1] == 'H' && play[2] == 'I'){
        if (peek(q,1) == CASTLE_DRACULA)
            gv->healthPoints[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
        push(q, HIDE);
    } else if (play[1] == 'D' && isdigit(play[2])){
        LocationID curloc = peek(q, play[2] - '0');
        push(q, 102 + (play[2] - '0'));
        if (curloc == TELEPORT){
            gv->healthPoints[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
        } else if (curloc == HIDE){
            curloc = (play[2] - '0' == 1) ? peek(q, 2) : peek(q, 1);
            if (curloc == TELEPORT || curloc == CASTLE_DRACULA)
                gv->healthPoints[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
        } else if (curloc != CITY_UNKNOWN && curloc != HIDE && curloc != NOWHERE &&
            (curloc == SEA_UNKNOWN || idToType(curloc) == 2)){
            gv->healthPoints[PLAYER_DRACULA] -= LIFE_LOSS_SEA;
        }
    } else if (play[1] == 'T' && play[2] == 'P'){
        push(q, TELEPORT);
        gv->healthPoints[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
    
    } else if (play[1] == 'C' && play[2] == '?'){
        push(q, CITY_UNKNOWN);
    
    } else if (play[1] == 'S' && play[2] == '?'){
        push(q, SEA_UNKNOWN);
        gv->healthPoints[PLAYER_DRACULA] -= LIFE_LOSS_SEA;
    
    } else{
        LocationID curloc = abbrevToID(play + 1);
        push(q, curloc);
        if (idToType(curloc) == 2)gv->healthPoints[PLAYER_DRACULA] -= LIFE_LOSS_SEA;
        else if (curloc == CASTLE_DRACULA)gv->healthPoints[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
    }

    if (play[3] == 'T'){
        updateTrap(tpvs, currlocation, 1);
    }

    if (play[4] == 'V'){
        updateVam(tpvs, currlocation, 1);
    }

    if (play[5] == 'M'){
        updateTrap(tpvs, currlocation, 0);
    } else if (play[5] == 'V'){
        gv->score -= SCORE_LOSS_VAMPIRE_MATURES;
        updateVam(tpvs, currlocation, 2);
    }
    gv->score -= SCORE_LOSS_DRACULA_TURN;
}

void newGame(GameView gv,qlist q[])
{
    gv->round = 0;
    gv->score = GAME_START_SCORE;
    int i = 0;
    while (i < NUM_PLAYERS - 1){
        gv->healthPoints[i] = GAME_START_HUNTER_LIFE_POINTS;
        push(q[i++],UNKNOWN_LOCATION);
    }
    gv->healthPoints[i] = GAME_START_BLOOD_POINTS;
    push(q[i],UNKNOWN_LOCATION);
}

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    GameView gv = malloc(sizeof(struct gameView));
    qlist history[NUM_PLAYERS];
    int i = 0,playerID = 0;;
    for (; i < NUM_PLAYERS; i++)history[i] = init();
    newGame(gv,history);
    gv->round = (strlen(pastPlays)+1) / (TURN_LEN * NUM_PLAYERS);

    TrapVam traps = malloc(sizeof(trapVam));
    traps->trap = init();
    traps->vam = init();

    for (i = 0; pastPlays[i] != '\0'; i += 8){
        if (pastPlays[i] == 'G')Hunter(gv, history[PLAYER_LORD_GODALMING], pastPlays + i, PLAYER_LORD_GODALMING,traps);
        else if (pastPlays[i] == 'S')Hunter(gv, history[PLAYER_DR_SEWARD], pastPlays + i, PLAYER_DR_SEWARD,traps);
        else if (pastPlays[i] == 'H')Hunter(gv, history[PLAYER_VAN_HELSING], pastPlays + i, PLAYER_VAN_HELSING,traps);
        else if (pastPlays[i] == 'M')Hunter(gv, history[PLAYER_MINA_HARKER], pastPlays + i, PLAYER_MINA_HARKER,traps);
        else Dracula(gv, history[PLAYER_DRACULA], pastPlays + i,traps);
    }
    
    for (; playerID < NUM_PLAYERS; playerID++){
        for (i = 0; i < TRAIL_SIZE && !isEmpty(history[playerID]);i++){
            gv->playerLocations[playerID][i] = spop(history[playerID]);
        }
        free(history[playerID]); //the list is empty after the above for loop so we don't need to free each node
    }
    freeTravm(traps);
    return gv;
}


// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    free(toBeDeleted);
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round getRound(GameView currentView)
{
    return currentView->round;
}


// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
    return currentView->round * NUM_PLAYERS % NUM_PLAYERS;
}
// Get the current score
int getScore(GameView currentView)
{
    return currentView->score;
}

// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    return(currentView->healthPoints[player]);
}

// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player)
{
    return currentView->playerLocations[player][0];
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,LocationID trail[TRAIL_SIZE])
{
    int i = 0;
    for(;i < TRAIL_SIZE; i++) {
        trail[i] = currentView->playerLocations[player][i]; 
    }
}

//// Functions that query the map to find information about connectivity

// Returns an array of LocationIDs for all directly connected locations

LocationID *connectedLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return NULL;
}
