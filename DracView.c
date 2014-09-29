// DracView.c ... DracView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"
#include "Map.h" //... if you decide to use the Map ADT
#include "Queue.h"

typedef struct dracView {
    Round round;
    int score;
    int healthPoint[NUM_PLAYERS];
    LocationID trail[NUM_PLAYERS][TRAIL_SIZE];
    TrapVam tpvs;
}dracView;

void trapHistoryH(char *pastPlay, TrapVam tpvs)
{
    if (pastPlay[3] == 'T')
        updateTrap(tpvs, abbrevToID(pastPlay + 1), 0);

    if (pastPlay[3] == 'V'|| pastPlay[4] == 'V')
        updateVam(tpvs, abbrevToID(pastPlay + 1), 0);
}

void trapHistoryD(char *pastPlay, TrapVam tpvs)
{
    LocationID currlocation = abbrevToID(pastPlay+1);
    
    if(pastPlay[3] == 'T' || pastPlay[4] == 'V'){
        assert(currlocation >= ADRIATIC_SEA && currlocation <= ZURICH);
        assert(idToType(currlocation) != SEA);
    }
    
    if (pastPlay[3] == 'T'){
        updateTrap(tpvs, currlocation, 1);
    }

    if (pastPlay[4] == 'V'){
        updateVam(tpvs, currlocation, 1);
    }

    if (pastPlay[5] == 'M'){
        updateTrap(tpvs, currlocation, 2);
    } else if (pastPlay[5] == 'V'){
        updateVam(tpvs, currlocation, 2);
    }
}

// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
    DracView dracView = malloc(sizeof(struct dracView));
    dracView->tpvs = malloc(sizeof(trapVam));
    dracView->tpvs->trap = init();
    dracView->tpvs->vam = init();

    GameView gv = newGameView(pastPlays, messages);

    dracView->round = getRound(gv);
    dracView->score = getScore(gv);

    int i = PLAYER_LORD_GODALMING;
    for (; i <= PLAYER_DRACULA; i++){
        getHistory(gv,i,dracView->trail[i]);
        dracView->healthPoint[i] = getHealth(gv, i);
    }

    for (i = 0; pastPlays[i] != '\0'; i += 7){
        if (i)i++;
        if (pastPlays[i] == 'D')trapHistoryD(pastPlays + i, dracView->tpvs);
        else trapHistoryH(pastPlays + i, dracView->tpvs);
    }

    return dracView;
}
     
     
// Frees all memory previously allocated for the DracView toBeDeleted
void disposeDracView(DracView toBeDeleted)
{
    freeTravm(toBeDeleted->tpvs);
    free( toBeDeleted );
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(DracView currentView)
{
    return currentView->round;
}

// Get the current score
int giveMeTheScore(DracView currentView)
{
    return currentView->score;
}

// Get the current health points for a given player
int howHealthyIs(DracView currentView, PlayerID player)
{
    return currentView->healthPoint[player];
}

// Get the current location id of a given player
LocationID whereIs(DracView currentView, PlayerID player)
{
    return currentView->trail[player][0];
}

// Get the most recent move of a given player
void lastMove(DracView currentView, PlayerID player,
                 LocationID *start, LocationID *end)
{
    *start = (currentView->trail[player][1]);
    *end = (currentView->trail[player][0]);
}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                         int *numTraps, int *numVamps)
{
    *numTraps = trapNum(currentView->tpvs->trap, where);
    *numVamps = trapNum(currentView->tpvs->vam, where);
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(DracView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    int i = 0;
    for (; i < TRAIL_SIZE; i++) {
        trail[i] = currentView->trail[player][i];
    }
}

//// Functions that query the map to find information about connectivity

// What are my (Dracula's) possible next moves (locations)
LocationID *whereCanIgo(DracView currentView, int *numLocations, int road,int sea)
{
    int temp;
    LocationID *connect = connectedLocations((GameView)currentView, &temp, whereIs(currentView, PLAYER_DRACULA), PLAYER_DRACULA, currentView->round, road, 0,sea);
    return connect;
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(DracView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    qlist q = init();
    int i,j,z,modmove,temp,tempRail;
    LocationID *connectRail;
    LocationID *connect = connectedLocations((GameView)currentView, &temp, whereIs(currentView, player), player, currentView->round, road, rail, sea);
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

    connectRail = connectedLocations((GameView)currentView, &tempRail, whereIs(currentView, player), player, currentView->round, 0, 1, 0);
    for (i = 1; i < modmove; i++){
        for (j = 0; j < tempRail; j++){
            connect = connectedLocations((GameView)currentView, &temp, connectRail[j], player, currentView->round, 0, 1, 0);
            for (z = 0; z < temp; z++)push(q, connect[z]);
        }
        free(connectRail);
        connectRail = connect;
        tempRail = temp;
    }

    removeDuplicate(q);
    free(connect);
    *numLocations = q->size;
    connect = malloc(sizeof(LocationID)*q->size);
    for (i = 0; i < q->size; i++){
        connect[i] = pop(q);
    }
    free(q);
    return connect;
}
