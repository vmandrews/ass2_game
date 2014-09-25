// GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h" //... if you decide to use the Map ADT
     
typedef struct gameView {
   int round;
   int score;
   int healthPoints[NUM_PLAYERS];   
   LocationID playerLocations[NUM_PLAYERS][TRAIL_SIZE];
}gameView;
typedef struct node{
   LocationID playerloc;
   struct node *next;
}node;

typedef struct queue{
   node *head;
   node *tail;
   int size;
}queue,*qlist;

qlist init()
{
   qlist newq = malloc(sizeof(queue));
   newq->head = newq->tail = NULL;
   newq->size = 0;
   return newq;
}

LocationID pop(qlist q)
{
   LocationID dest = q->head->playerloc;
   node *todel = q->head;
   if (!q->head)return -1;
   if (q->head == q->tail){
      q->head = q->tail = NULL;
   } else {
      q->head = q->head->next;
   }
   free(todel);
   q->size--;
   return dest;
}

void push(qlist q,LocationID it)
{
   node *n = malloc(sizeof(node));
   n->playerloc = it; n->next = NULL;
   if (q->size == 0){
      q->head = q->tail = n;
      q->size++;
   } else if(q->size == TRAIL_SIZE) {
      pop(q);
      q->tail->next = n;
   } else{
      q->tail->next = n;
      q->size++;
   }
}

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    GameView gameView = malloc(sizeof(struct gameView));
    gameView->round = strlen(pastPlays);
    
    /* pastPlays is a string which summarises ALL previous plays in the game.
     * Each play is 7 character:
     * For hunters: 
     *      1st letter: G,S,H,M represents the player
     *      2nd and 3rd letter together is appreviated city name
     *      Last 4 letters: T for traps, V for Vampire, D for Dracula,      
     *      or . for the remaining spaces
     *
     * For Dracula:
     *      1st letter: D
     *      2nd and 3rd Letter: Represents location of Dracula (city, sea etc.)
     *      4th and 5th Letter: Trap (T) or Vampire (V) placed
     *      6th Letter: Trap is left (malfunctions - M) or vampire matures (V) or .
     *      7th Letter: always .
     * 
     *
     * We need to take this string to update the GameView struct i think???
     */
    
    return gameView;
}
     
     
// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
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
    return(getRound(currentView) % NUM_PLAYERS);
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
void getHistory(GameView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
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
