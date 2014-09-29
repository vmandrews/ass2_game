 // GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Queue.h"
#include "Map.h" //... if you decide to use the Map ADT

#define TURN_LEN 8

typedef struct gameView {
    int round;
    int score; 
    int currentPlayer;
    int healthPoints[NUM_PLAYERS];
    LocationID playerLocations[NUM_PLAYERS][TRAIL_SIZE];
}gameView;

//records the action of hunter "player" in one particular turn
void Hunter(GameView gv, qlist q, char *plays, PlayerID player)
{
    LocationID prevloc = peek(q, 1);
    LocationID currloc = abbrevToID(plays + 1);
    if (isNotEmpty(q) == 6)pop(q);
    push(q, currloc);
    if (prevloc == currloc && gv->healthPoints[player] < GAME_START_HUNTER_LIFE_POINTS){
        gv->healthPoints[player] += LIFE_GAIN_REST;
        if (gv->healthPoints[player] > GAME_START_HUNTER_LIFE_POINTS)
            gv->healthPoints[player] = GAME_START_HUNTER_LIFE_POINTS;
    }
    if (plays[3] == 'T'){
        int numTrap = 1;
        if (plays[4] == 'T')numTrap++;
        if (plays[5] == 'T')numTrap++;
        if (plays[6] == 'T')numTrap++;
        gv->healthPoints[player] -=numTrap * LIFE_LOSS_TRAP_ENCOUNTER;
    }
    if (plays[3] == 'D' || plays[4] == 'D' || plays[5] == 'D'||plays[6] == 'D'){
        gv->healthPoints[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
        gv->healthPoints[player] -= LIFE_LOSS_DRACULA_ENCOUNTER;
    }

    if (gv->healthPoints[player] <= 0){
        gv->healthPoints[player] = GAME_START_HUNTER_LIFE_POINTS;
        if (isNotEmpty(q) == 6)pop(q);
        push(q, ST_JOSEPH_AND_ST_MARYS);
        gv->score -= SCORE_LOSS_HUNTER_HOSPITAL;
    }
}

void Dracula(GameView gv, qlist q, char *play)
{
    if (play[1] == 'H' && play[2] == 'I'){
        if (peek(q, 1) == CASTLE_DRACULA || peek(q,1) == TELEPORT)
            gv->healthPoints[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
        if (isNotEmpty(q) == 6)pop(q);
        push(q, HIDE);
    }else if (play[1] == 'D' && isdigit(play[2])){
        LocationID curloc = peek(q, play[2] - '0');
        if (curloc == TELEPORT || curloc == CASTLE_DRACULA){
            gv->healthPoints[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
        }else if (curloc == HIDE){
            curloc = (play[2] - '0' == 1) ? peek(q, 2) : peek(q, 1);
            if (curloc == TELEPORT || curloc == CASTLE_DRACULA)
                gv->healthPoints[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
        }else if (curloc != CITY_UNKNOWN && curloc != HIDE && curloc != NOWHERE &&
            (curloc == SEA_UNKNOWN || idToType(curloc) == 2)){
            gv->healthPoints[PLAYER_DRACULA] -= LIFE_LOSS_SEA;
        }
        if (isNotEmpty(q) == 6)pop(q);
        push(q, 102 + (play[2] - '0'));
    }else if (play[1] == 'T' && play[2] == 'P'){
        if (isNotEmpty(q) == 6)pop(q);
        push(q, TELEPORT);
        gv->healthPoints[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;

    }else if (play[1] == 'C' && play[2] == '?'){
        if (isNotEmpty(q) == 6)pop(q);
        push(q, CITY_UNKNOWN);

    }else if (play[1] == 'S' && play[2] == '?'){
        if (isNotEmpty(q) == 6)pop(q);
        push(q, SEA_UNKNOWN);
        gv->healthPoints[PLAYER_DRACULA] -= LIFE_LOSS_SEA;

    }else{
        LocationID curloc = abbrevToID(play + 1);
        if (isNotEmpty(q) == 6)pop(q);
        push(q, curloc);
        if (idToType(curloc) == 2)gv->healthPoints[PLAYER_DRACULA] -= LIFE_LOSS_SEA;
        else if (curloc == CASTLE_DRACULA)gv->healthPoints[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
    }

    if (play[5] == 'V'){
        gv->score -= SCORE_LOSS_VAMPIRE_MATURES;
    }
    gv->score -= SCORE_LOSS_DRACULA_TURN;
}


void newGame(GameView gv,qlist q[])
{
    gv->round = 0;
    gv->score = GAME_START_SCORE;
    int i = 0,j;
    while (i < NUM_PLAYERS - 1){
        gv->healthPoints[i] = GAME_START_HUNTER_LIFE_POINTS;
        for (j = 0; j < TRAIL_SIZE; j++){
            if (isNotEmpty(q[i]) == 6)pop(q[i]);
            push(q[i], UNKNOWN_LOCATION);
        }
        i++;
    }
    gv->healthPoints[i] = GAME_START_BLOOD_POINTS;
    for (j = 0; j < TRAIL_SIZE; j++){
        if (isNotEmpty(q[i]) == 6)pop(q[i]);
        push(q[i], UNKNOWN_LOCATION);
    }
}

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    GameView gv = malloc(sizeof(struct gameView));
    qlist history[NUM_PLAYERS];
    int i = 0, playerID = 0;;
    for (; i < NUM_PLAYERS; i++)history[i] = init();
    newGame(gv, history);
    gv->round = (strlen(pastPlays) + 1) / (TURN_LEN * NUM_PLAYERS);
    gv->currentPlayer = ((strlen(pastPlays) + 1) / TURN_LEN) % NUM_PLAYERS;

    for (i = 0; pastPlays[i] != '\0'; i += 7){
        if (i)i++;
        if (pastPlays[i] == 'G')Hunter(gv, history[PLAYER_LORD_GODALMING], pastPlays + i, PLAYER_LORD_GODALMING);
        else if (pastPlays[i] == 'S')Hunter(gv, history[PLAYER_DR_SEWARD], pastPlays + i, PLAYER_DR_SEWARD);
        else if (pastPlays[i] == 'H')Hunter(gv, history[PLAYER_VAN_HELSING], pastPlays + i, PLAYER_VAN_HELSING);
        else if (pastPlays[i] == 'M')Hunter(gv, history[PLAYER_MINA_HARKER], pastPlays + i, PLAYER_MINA_HARKER);
        else Dracula(gv, history[PLAYER_DRACULA], pastPlays + i);
    }

    for (; playerID < NUM_PLAYERS; playerID++){
        for (i = 0; i < TRAIL_SIZE && isNotEmpty(history[playerID]); i++){
            gv->playerLocations[playerID][i] = spop(history[playerID]);
        }
        free(history[playerID]); //the list is empty after the above for loop so we don't need to free each node
    }
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
    return currentView->currentPlayer;
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

void filterH(TrapVam connect, Round round, PlayerID player)
{
    assert(connect->trap != NULL && connect->vam != NULL);
    int size = connect->trap->size;
    LocationID place;
    LocationID type;
    while (!(round+player)%4 && size > 0){
        place = pop(connect->trap);
        type = pop(connect->vam);
        size--;
        if (!(type == RAIL)){
            push(connect->trap,place);
            push(connect->vam, type);
        }
    }
}

void filterD(TrapVam connect, Round round, PlayerID player)
{
    assert(connect->trap != NULL && connect->vam != NULL);
    int size = connect->trap->size;
    LocationID place;
    LocationID type;
    while (size > 0){
        place = pop(connect->trap);
        type = pop(connect->vam);
        size--;
        if (!(type == RAIL) && !(place == ST_JOSEPH_AND_ST_MARYS)){
            push(connect->trap, place);
            push(connect->vam, type);
        }
    }
}

LocationID *connecteddirectLocations(int *numLocations,
    LocationID from, PlayerID player, Round round,
    int road, int rail, int sea)
{
    TrapVam connect = malloc(sizeof(trapVam));
    Map g = newMap();
    int boolType[] = { road, rail, sea };

    connect->trap = init(); // this q stores the locationID
    connect->vam = init(); //  this q stores the type of transport

    int len = connections(g, from, boolType);
    LocationID* places = neighbor(g, from, boolType, 0, len);
    TransportID* types = neighbor(g, from, boolType, 1, len);

    int i = 0;
    for (; i < len; i++){
        push(connect->trap, places[i]);
        push(connect->vam, types[i]);
    }

    free(places);
    free(types);

    if (player == PLAYER_DRACULA)filterD(connect, round, player);
    else filterH(connect, round, player);

    removeDuplicate(connect->trap);
    push(connect->trap, from);
    *numLocations = connect->trap->size;
    LocationID *tobereturn = malloc(sizeof(LocationID)*connect->trap->size);

    for (i = 0; isNotEmpty(connect->trap); i++){
        tobereturn[i] = pop(connect->trap);
    }

    freeTravm(connect);
    return tobereturn;
}

//// Functions that query the map to find information about connectivity

// Returns an array of LocationIDs for all directly connected locations
void getConnect(qlist q, LocationID connects[],int len,int modmove,PlayerID player,Round round)
{
    LocationID *connectRail;
    if (modmove == 0)return;
    int railLen,j;
    for (j = 0; j < len; j++){
       push(q, connects[j]);
       connectRail = connecteddirectLocations(&railLen, connects[j],player,round,0,1,0);
       getConnect(q, connectRail, railLen, modmove - 1, player, round);
       free(connectRail);
    }
}

LocationID *connectedLocations(GameView currentView, int *numLocations,
    LocationID from, PlayerID player, Round round,
    int road, int rail, int sea)
{
 assert(from >= 0 && from <= 70);
    qlist q = init();
    int i, modmove, temp, tempRail;
    LocationID *connectRail;
    LocationID *connect = connecteddirectLocations(&temp,from, player, currentView->round, road, rail, sea);
    *numLocations = temp;
    if (player == PLAYER_DRACULA || rail == 0){
        free(q); return connect;
    }

    modmove = (currentView->round + player) % 4;
    if (modmove == 0 || modmove == 1){
        free(q); return connect;
    }

    for (i = 0; i < temp; i++)push(q, connect[i]);
    free(connect);

    connectRail = connecteddirectLocations(&tempRail, from, player, currentView->round, 0, 1, 0);
    getConnect(q, connectRail, tempRail, modmove, player, round);

    removeDuplicate(q);
    free(connectRail);
    
    movetrap(q, from);
    push(q,from);

    *numLocations = q->size;
    connect = malloc(sizeof(LocationID)*q->size);
    for (i = 0; i < *numLocations; i++){
        connect[i] = pop(q);
    }
    free(q);
    return connect;
}
