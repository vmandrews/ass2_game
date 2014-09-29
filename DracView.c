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
    return connectedLocations((GameView)currentView, numLocations, whereIs(currentView, player), player,giveMeTheRound(currentView), road, rail, sea);
}
