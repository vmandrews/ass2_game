// HunterView.c ... HunterView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Queue.h"

// #include "Map.h" ... if you decide to use the Map ADT
     
typedef struct hunterView {
    Round round;
    int score;
    int currentPlayer;
    int healthPoint[NUM_PLAYERS];
    LocationID trail[NUM_PLAYERS][TRAIL_SIZE];
}hunterView;
     

// Creates a new HunterView to summarise the current state of the game
HunterView newHunterView(char *pastPlays, PlayerMessage messages[])
{
    HunterView hunterView = malloc(sizeof(struct hunterView));
    GameView gv = newGameView(pastPlays,messages);

    hunterView->round = getRound(gv);
    hunterView->score = getScore(gv);
    hunterView->currentPlayer = getCurrentPlayer(gv);
    
    int i = PLAYER_LORD_GODALMING;
    for (; i <= PLAYER_DRACULA; i++){
        getHistory(gv, i, hunterView->trail[i]);
        hunterView->healthPoint[i] = getHealth(gv, i);
    }

    disposeGameView(gv);
    return hunterView;
}
     
     
// Frees all memory previously allocated for the HunterView toBeDeleted
void disposeHunterView(HunterView toBeDeleted)
{
    free( toBeDeleted );
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(HunterView currentView)
{ 
    return currentView->round;
}

// Get the id of current player
PlayerID whoAmI(HunterView currentView)
{
    return currentView->currentPlayer;
}

// Get the current score
int giveMeTheScore(HunterView currentView)
{
    return currentView->score;
}

// Get the current health points for a given player
int howHealthyIs(HunterView currentView, PlayerID player)
{
    return currentView->healthPoint[player];
}

// Get the current location id of a given player
LocationID whereIs(HunterView currentView, PlayerID player)
{
//    assert(player != PLAYER_DRACULA);
    return currentView->trail[player][0];
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(HunterView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    int i = 0;
    for (; i < TRAIL_SIZE; i++) {
        trail[i] = currentView->trail[player][i];
    }
}

//// Functions that query the map to find information about connectivity

// What are my possible next moves (locations)
LocationID *whereCanIgo(HunterView currentView, int *numLocations, int road, int rail,int sea)
{
    return whereCanTheyGo(currentView, numLocations, whoAmI(currentView), road, rail, sea);
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(HunterView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    return connectedLocations((GameView)currentView, numLocations, whereIs(currentView, player), player,giveMeTheRound(currentView), road, rail, sea);
}
