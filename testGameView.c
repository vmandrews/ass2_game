// testGameView.c ... test the GameView ADT

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "GameView.h"

#define MAX_STRING 14640

void testGameView_1 (void);
void testGamView_TL();
static char *idToPlayerName(int i);
static void printhistory(GameView gv);
static void printConnects(GameView gv);
void printLocationID(LocationID *connects, int num);

int main()
{
    testGameView_1 ();
    testGameView_TL();
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
    char past_play_string[200] = "GAL.... SBI.... HBI.... MBI.... DASTV..";
    new = newGameView(past_play_string, messages);
    i = getScore(new);
    assert(i == 365); //Dracula's turn finished (-1 score)
    strcat(past_play_string, " GASTD.. SBI.... HBI.... MBI.... DC?T.V.");
    new = newGameView(past_play_string, messages);
    i = getScore(new);
    assert(i == 351); //Vampire matures (-13 score) and Dracula turn finished (-1 score)
    strcat(past_play_string, " GJMD... SBI.... HBI.... MBI....");
    new = newGameView(past_play_string, messages);
    i = getScore(new);
    assert(i == 345); //LORD_GODALMING sent to hospital (-6 score)
    printf("passed\n");

    int size;
    i = ADRIATIC_SEA;
    LocationID *connected;
    printf("testing road connections...\n");
    
    FILE *output;
    output = fopen("road_connections.txt", "w+");
    fclose(output);
    int x;
    
    while(i < NUM_MAP_LOCATIONS) {
        connected = connectedLocations(new, &size, i, PLAYER_LORD_GODALMING, 0, 1, 0, 0);
        char *from_string = idToName(i);
        for(x = 0; x < size; x++){
            output = fopen("road_connections.txt", "a");
            char *location_string = idToName(connected[x]);
            fputs(from_string, output);
            fprintf(output, " ");
            fputs(location_string, output);
            fprintf(output, "\n");
            fclose(output); 
        }
        i++;
        x = 0;
    }
    



/*

Road moves: a Hunter can move to any city directly connected to the current city by a road.
Sea moves: a Hunter can move from a port to an adjacent sea, or a sea to an adjacent sea, or a sea to an adjacent port city.
Rail moves: The maximum distance that can be moved via rail is determined by the sum of the round number (0..366) and the Hunter number (0..3)
sum mod 4 is 0: No train move is permitted for hunters this turn.
sum mod 4 is 1: Hunters may move to any city adjacent to the current city via a rail link.
sum mod 4 is 2: Hunters may move to any city adjacent to the current city via a rail link, or any city adjacent via rail to such a city.
sum mod 4 is 3: Hunters may move to any city adjacent to the current city via a rail link, or any city adjacent via rail to such a city, or any city adjacent via rail to such a city. (IE move up to three steps by rail)
when the rail move is to a non-adjacent city the Hunter does not actually enter the intermediate cities, so any encounters there are not encountered etc

*/


    free(connected);

    disposeGameView(new);
}

/*
   "GBE.... SBR.... HLO.... MCA.... DC?.V.."
   "GBE.... SBR.... HLO.... MCA.... DSJ.V.. GSJVD.."
   G S H M D
*/

//takes in playerID and returns name of player
char *idToPlayerName(int i)
{
    char *name = malloc(sizeof(char)* 15);
    if (i == 0)return "LORD GODALMING";
    if (i == 1)return "DR_SEWARD";
    if (i == 2)return "VAN_HELSING";
    if (i == 3)return "MINA_HARKER";
    if (i == 4)return "DRACULA";
    return name;
}
void printGameStatus(GameView gv)
{
    int i;
    printf("current round is %d and currentPlayer is %s\n", getRound(gv),
                                    idToPlayerName(getCurrentPlayer(gv)));
    printf("Current score is %d\n", getScore(gv));
    for (i = 0; i < NUM_PLAYERS; i++){
        printf("%s now on %d health\n", idToPlayerName(i), getHealth(gv, i));
    }
}

//prints the trail of each player
void printhistory(GameView gv)
{
    int i, j;
    LocationID history[TRAIL_SIZE];

    for (i = 0; i < NUM_PLAYERS; i++){
        printf("Player %s's history: ", idToPlayerName(i));
        getHistory(gv, i, history);
        if (history[0] >= 0 && history[0] <= 70){
            printf("%s", idToName(history[0]));
        }
        else printf("%d", history[0]);

        for (j = 1; j < TRAIL_SIZE; j++){
            if (history[j] >= 0 && history[j] <= 70){
                printf("<-%s", idToName(history[j]));
            }
            else printf("<-%d", history[j]);
        }
        printf("\n");
    }
}


void printLocationID(LocationID *connects, int num)
{
    int j;
    for (j = 0; j < num - 1; j++){
        printf("%s\n", idToName(connects[j]));
    }
}

//prints connections
void printConnects(GameView gv)
{
    int num,i;
    for (i = 0; i < NUM_PLAYERS; i++){
        LocationID place = getLocation(gv, i);
        if (place < 0 || place > 70){
            printf("%s is not at a valid location: %d\n",idToPlayerName(i),place);
            continue;
        }
        LocationID *connects = connectedLocations(gv, &num, place, i, getRound(gv), 1, 1, 1);
        printf("%s is at %s with connections to:\n",idToPlayerName(i), idToName(place));
        printLocationID(connects, num);
        free(connects);
    }
}

void testGameView_TL()
{
    char pastPlay[MAX_STRING] = {""};
    //round 0
    PlayerMessage message[] = {"Hi"};
    puts("test input string = \"GMU....\"");
    strcat(pastPlay, "GMU....");
    GameView gv = newGameView(pastPlay, message);

    puts("basic setup checks");
    assert(getCurrentPlayer(gv) == PLAYER_DR_SEWARD);
    assert(getRound(gv) == 0);
    assert(getLocation(gv, PLAYER_LORD_GODALMING) == MUNICH);
    assert(getLocation(gv, PLAYER_DRACULA) == UNKNOWN_LOCATION);
    assert(getLocation(gv, PLAYER_MINA_HARKER) == UNKNOWN_LOCATION);
    assert(getHealth(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
    assert(getHealth(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(gv, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(gv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(gv, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS);

    puts("History log check");
    LocationID history[TRAIL_SIZE];
    getHistory(gv, PLAYER_DRACULA, history);
    assert(history[0] == UNKNOWN_LOCATION);
    assert(history[1] == UNKNOWN_LOCATION);
    getHistory(gv, PLAYER_LORD_GODALMING, history);
    assert(history[0] == MUNICH);
    assert(history[1] == UNKNOWN_LOCATION);
    assert(history[2] == UNKNOWN_LOCATION);
    getHistory(gv, PLAYER_DR_SEWARD, history);
    assert(history[0] == UNKNOWN_LOCATION);
    assert(history[1] == UNKNOWN_LOCATION);

    puts("passed");
    disposeGameView(gv);

    strcat(pastPlay, " SMR.... HPA.... MBR....");
    printf("\ntest string \"%s\"\n", pastPlay);
    gv = newGameView(pastPlay,message);
    printhistory(gv);
    printConnects(gv);
    disposeGameView(gv);

    strcat(pastPlay, " DST....");
    printf("\ntest string \"%s\"\n", pastPlay);
    gv = newGameView(pastPlay, message);
    assert(getRound(gv) == 1);
    printhistory(gv);
    printConnects(gv);
    disposeGameView(gv);

    strcat(pastPlay, " GSTD... SPA.... HSTD... MHA....");
    printf("\ntest string \"%s\"\n", pastPlay);
    gv = newGameView(pastPlay, message);
    assert(getHealth(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 2 * LIFE_LOSS_HUNTER_ENCOUNTER);
    assert(getHealth(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
    assert(getHealth(gv, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
    assert(getCurrentPlayer(gv) == PLAYER_DRACULA);
    assert(getRound(gv) == 1);
    printGameStatus(gv);
    printhistory(gv);
    printConnects(gv);

    strcat(pastPlay, " DZUT...");
    printf("\ntest string \"%s\"\n", pastPlay);
    gv = newGameView(pastPlay, message);
    printGameStatus(gv);
    printhistory(gv);
    printConnects(gv);
    disposeGameView(gv);

    //H dies this round, he is originally on 5 health due to Dracula encounter
    //then he steps onto the trap so -2
    //then encounter dracula -4 and dies
    //dracula teleports to castle and increases health by 10 so health should remain unchange
    strcat(pastPlay, " GBU.... SBU.... HZUTD.. MLI.... DTP....");
    printf("\ntest string \"%s\"\n", pastPlay);
    gv = newGameView(pastPlay, message);
    assert(getScore(gv) == GAME_START_SCORE - getRound(gv) - SCORE_LOSS_HUNTER_HOSPITAL);
    assert(getHealth(gv, PLAYER_DRACULA) == 20);
    assert(getHealth(gv, PLAYER_VAN_HELSING) == 9);
    printGameStatus(gv);
    printhistory(gv);
    printConnects(gv);

    int num;
    puts("\nConnection test Dracula Castle for Hunter");
    LocationID *connects = connectedLocations(gv, &num, CASTLE_DRACULA, PLAYER_LORD_GODALMING, getRound(gv), 1, 1, 1);
    printf("%s is at %s with connections to:\n", idToPlayerName(PLAYER_LORD_GODALMING), idToName(CASTLE_DRACULA));
    printLocationID(connects, num);
    free(connects);

    puts("\nConnection test Milan for Dracula");
    connects = connectedLocations(gv, &num, MILAN, PLAYER_DRACULA, getRound(gv), 1, 1, 1);
    printf("%s is at %s with connections to:\n", idToPlayerName(PLAYER_DRACULA), idToName(MILAN));
    printLocationID(connects, num);

    puts("\nConnection test Milan for Hunter");
    connects = connectedLocations(gv, &num, MILAN, PLAYER_LORD_GODALMING, getRound(gv), 1, 1, 1);
    printf("%s is at %s with connections to:\n", idToPlayerName(PLAYER_LORD_GODALMING), idToName(MILAN));
    printLocationID(connects, num);
    free(connects);
    disposeGameView(gv);

    strcat(pastPlay, " GST.... SLE.... HZA.... MHA.... DHITVV.");
    printf("\ntest string \"%s\"\n", pastPlay);
    gv = newGameView(pastPlay, message);
    printGameStatus(gv);
    printhistory(gv);
    printConnects(gv);

    strcat(pastPlay, " GCFTTTT SEDTTTT HGATTTT MCGTTTT DD2....");
    printf("\ntest string \"%s\"\n", pastPlay);
    gv = newGameView(pastPlay, message);
    printGameStatus(gv);
    printhistory(gv);
    printConnects(gv);

    strcat(pastPlay," GVI.... SVA.... HTS....");
    printf("\ntest string \"%s\"\n", pastPlay);
    gv = newGameView(pastPlay, message);
    printGameStatus(gv);
    printhistory(gv);

    strcat(pastPlay, " MCD.... DCD.VV. GSJ....");
    printf("\ntest string \"%s\"\n", pastPlay);
    gv = newGameView(pastPlay, message);
    printGameStatus(gv);
    printhistory(gv);
    printConnects(gv);

    strcat(pastPlay, " SSR.... HPR.... MPL.... DC?T.M.");
    printf("\ntest string \"%s\"\n", pastPlay);
    gv = newGameView(pastPlay, message);
    printGameStatus(gv);
    printhistory(gv);
    printConnects(gv);

    strcat(pastPlay, " SSR.... HPR.... MPL.... DD2T.V.");
    printf("\ntest string \"%s\"\n", pastPlay);
    gv = newGameView(pastPlay, message);
    printGameStatus(gv);
    printhistory(gv);
}
