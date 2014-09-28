// testGameView.c ... test the GameView ADT

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "GameView.h"

void testGameView_1 (void);

int main()
{
    testGameView_1 ();

    int i;
    GameView gv;
    
    printf("Test basic empty initialisation\n");
    PlayerMessage messages1[] = {};
    gv = newGameView("", messages1);
    assert(getCurrentPlayer(gv) == PLAYER_LORD_GODALMING);
    assert(getRound(gv) == 0);
    assert(getHealth(gv,PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(gv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
    assert(getScore(gv) == GAME_START_SCORE);
    assert(getLocation(gv,PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);
    printf("passed\n");
    disposeGameView(gv);

    printf("Test for Dracula trail and basic functions\n");
    PlayerMessage messages2[] = {"Hello","Rubbish","Stuff","","Mwahahah"};
    gv = newGameView("GST.... SAO.... HZU.... MBB.... DC?....", messages2);
    assert(getCurrentPlayer(gv) == PLAYER_LORD_GODALMING);
    assert(getRound(gv) == 1);
    assert(getLocation(gv,PLAYER_LORD_GODALMING) == STRASBOURG);
    assert(getLocation(gv,PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
    assert(getLocation(gv,PLAYER_VAN_HELSING) == ZURICH);
    assert(getLocation(gv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
    assert(getLocation(gv,PLAYER_DRACULA) == CITY_UNKNOWN);
    assert(getHealth(gv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
    printf("passed\n");
    disposeGameView(gv);

    printf("Test for encountering Dracula and hunter history\n");
    PlayerMessage messages3[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!"};
    gv = newGameView("GST.... SAO.... HCD.... MAO.... DGE.... GGED...", messages3);
    assert(getLocation(gv,PLAYER_DRACULA) == GENEVA);
    assert(getHealth(gv,PLAYER_LORD_GODALMING) == 5);
    assert(getHealth(gv,PLAYER_DRACULA) == 30);
    assert(getLocation(gv,PLAYER_LORD_GODALMING) == GENEVA);
    LocationID history[TRAIL_SIZE];
    getHistory(gv,PLAYER_DRACULA,history);
    assert(history[0] == GENEVA);
    assert(history[1] == UNKNOWN_LOCATION);
    getHistory(gv,PLAYER_LORD_GODALMING,history);
    assert(history[0] == GENEVA);
    assert(history[1] == STRASBOURG);
    assert(history[2] == UNKNOWN_LOCATION);
    getHistory(gv,PLAYER_DR_SEWARD,history);
    assert(history[0] == ATLANTIC_OCEAN);
    assert(history[1] == UNKNOWN_LOCATION);
    printf("passed\n");        
    disposeGameView(gv);

    printf("Test for Dracula doubling back at sea, and losing blood points (Hunter View)\n");
    PlayerMessage messages4[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!","","","","Back I go"};
    gv = newGameView("GGE.... SGE.... HGE.... MGE.... DS?.... "
                     "GST.... SST.... HST.... MST.... DD1....", messages4);
    assert(getLocation(gv,PLAYER_DRACULA) == DOUBLE_BACK_1);
    getHistory(gv,PLAYER_DRACULA,history);
    assert(history[0] == DOUBLE_BACK_1);
    assert(history[1] == SEA_UNKNOWN);
    assert(getHealth(gv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4);
    assert(getCurrentPlayer(gv) == 0);
    printf("passed\n");
    disposeGameView(gv);

    printf("Test for Dracula doubling back at sea, and losing blood points (Drac View)\n");
    PlayerMessage messages5[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!","","","","Back I go"};
    gv = newGameView("GGE.... SGE.... HGE.... MGE.... DEC.... "
                     "GST.... SST.... HST.... MST.... DD1....", messages5);
    assert(getLocation(gv,PLAYER_DRACULA) == DOUBLE_BACK_1);
    getHistory(gv,PLAYER_DRACULA,history);
    assert(history[0] == DOUBLE_BACK_1);
    assert(history[1] == ENGLISH_CHANNEL);
    assert(getHealth(gv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4);
    assert(getCurrentPlayer(gv) == 0);
    printf("passed\n");
    disposeGameView(gv);

    printf("Test for connections\n");
    int size, seen[NUM_MAP_LOCATIONS], *edges;
    gv = newGameView("", messages1);    
    printf("Checking Galatz road connections\n");
    edges = connectedLocations(gv,&size,GALATZ,PLAYER_LORD_GODALMING,0,1,0,0);
    memset(seen, 0, NUM_MAP_LOCATIONS*sizeof(int));
    for (i = 0; i< size ; i++) seen[edges[i]] = 1;
    assert(size == 5); assert(seen[GALATZ]); assert(seen[CONSTANTA]);
    assert(seen[BUCHAREST]); assert(seen[KLAUSENBURG]); assert(seen[CASTLE_DRACULA]);
    free(edges);
    printf("Checking Ionian Sea sea connections\n");
    edges = connectedLocations(gv,&size,IONIAN_SEA,PLAYER_LORD_GODALMING,0,0,0,1);
    memset(seen, 0, NUM_MAP_LOCATIONS*sizeof(int));
    for (i = 0; i < size; i++) seen[edges[i]] = 1;
    assert(size == 7); assert(seen[IONIAN_SEA]); assert(seen[BLACK_SEA]);
    assert(seen[ADRIATIC_SEA]); assert(seen[TYRRHENIAN_SEA]);
    assert(seen[ATHENS]); assert(seen[VALONA]); assert(seen[SALONICA]);
    free(edges);
    printf("Checking Athens rail connections (none)\n");
    edges = connectedLocations(gv,&size,ATHENS,PLAYER_LORD_GODALMING,0,0,1,0);
    assert(size == 1);
    assert(edges[0] == ATHENS);
    free(edges);
    printf("passed\n");
    disposeGameView(gv);
    return 0;
}



void testGameView_1 (void)
{
    GameView new;
    int i; // for testing
    PlayerMessage messages[] = {""};
    
    new = newGameView("GAS.... SAS.... HAS.... MAS.... DC?.... "
                    "GAL.... SAL.... HAL.... MAL.... DS?.... "
                    "GAM.... SAM.... HAM.... MAM.... DHI.... "
                    "GAT.... SAT.... HAT.... MAT.... DD3.... "
                    "GAO.... SAO.... HAO.... MAO.... DTP.... "
                    "GBA.... SBA.... HBA.... MBA.... DC?.... "
                    "GBI.... SBI.... HBI.... MBI.... DS?...." , messages);
    printf("testing getLocation...\n");
    assert(getLocation(new,PLAYER_LORD_GODALMING) == BARI);
    assert(getLocation(new,PLAYER_DR_SEWARD) == BARI);
    assert(getLocation(new,PLAYER_VAN_HELSING) == BARI);
    assert(getLocation(new,PLAYER_MINA_HARKER) == BARI);
    assert(getLocation(new,PLAYER_DRACULA) == SEA_UNKNOWN);
    printf("passed\n");

    printf("testing getHistory...\n");
    LocationID trail[TRAIL_SIZE];
    getHistory(new, PLAYER_LORD_GODALMING, trail);
    assert(trail[0] == BARI);   
    assert(trail[1] == BARCELONA);   
    assert(trail[2] == ATLANTIC_OCEAN);   
    assert(trail[3] == ATHENS);   
    assert(trail[4] == AMSTERDAM);   
    assert(trail[5] == ALICANTE);   
    getHistory(new, PLAYER_DRACULA, trail);  
    assert(trail[0] == SEA_UNKNOWN);   
    assert(trail[1] == CITY_UNKNOWN);   
    assert(trail[2] == TELEPORT);   
    assert(trail[3] == DOUBLE_BACK_3);   
    assert(trail[4] == HIDE);   
    assert(trail[5] == SEA_UNKNOWN);  
    printf("passed\n");

    printf("testing getRound, getScore and getHealth...\n");
    i = getRound(new);
    assert(i == 7);
    i = getScore(new);
    assert(i == 359);
    i = getCurrentPlayer(new);
    assert(i == PLAYER_LORD_GODALMING);

    // in complete round
    new = newGameView("GAS.... SAS.... HAS.... MAS.... DC?.... "
                    "GAL.... SAL.... HAL.... MAL.... DS?.... "
                    "GAM.... SAM.... HAM.... MAM.... DHI.... "
                    "GAT.... SAT.... HAT.... MAT.... DD3.... "
                    "GAO.... SAO.... HAO.... MAO.... DTP.... "
                    "GBA.... SBA.... HBA.... MBA.... DC?.... "
                    "GBI.... SBI...." , messages);
    
    i = getRound(new);
    assert(i == 6);
    i = getScore(new);
    assert(i == 360);
    i = getCurrentPlayer(new);
    assert(i == PLAYER_VAN_HELSING);
    i = getHealth(new, PLAYER_DRACULA);
    assert(i == 48);
    i = getHealth(new, PLAYER_LORD_GODALMING);
    assert(i == 9);

    printf("passed\n");

    new = newGameView("GAST... SAST... HAS.... MAS.... DC?.... "
                    "GAS.... SALT... HAL.... MAL.... DC?.... "
                    "GAMD... SAMT... HAM.... MAM.... DCD.... "
                    "GAM.... SATT... HAT.... MAT.... DHI.... "
                    "GAO.... SAOT... HAO.... MAO.... DC?.... "
                    "GBA.... SBA.... HBA.... MBA.... DCD.... "
                    "GBI.... SBI.... HBI.... MBI.... DC?...." , messages);

    printf("testing Health Points...\n");
    i = getHealth(new, PLAYER_LORD_GODALMING);
    assert(i == 8); //encounters trap (-2), rest (max health 9), encourter D (-4), rest (+3)
    i = getHealth(new, PLAYER_DR_SEWARD);
    assert(i == 9); // dies from traps and revived
    i = getHealth(new, PLAYER_DRACULA);
    assert(i == 60); // Castle Dracula for three turns (+20) and 1 encounter (-10)
    printf("passed\n");

    printf("testing get score function...\n");
    char past_play_string[200] = "GAST... SAST... HAS.... MAS.... DC?.V..";
    new = newGameView(past_play_string, messages);
    i = getScore(new);
    assert(i == 365); //Dracula's turn finished (-1 score)
    strcat(past_play_string, " GASD... SAST... HAS.... MAS.... DC?T.V.");
    new = newGameView(past_play_string, messages);
    i = getScore(new);
    assert(i == 351); //Vampire matures (-13 score) and Dracula turn finished (-1 score)
    strcat(past_play_string, " GJMD... SJM.... HAS.... MAS....");
    new = newGameView(past_play_string, messages);
    i = getScore(new);
    printf("%d score\n",i);
    //assert(i == 345); //LORD_GODALMING sent to hospital (-6 score)
    printf("passed\n");

    printf("testing connections...\n");
    int size;
    LocationID *connected;
    connected = connectedLocations(new, &size, ALICANTE, PLAYER_LORD_GODALMING, 1, 1, 1, 1);

    free(connected);

    disposeGameView(new);
}

