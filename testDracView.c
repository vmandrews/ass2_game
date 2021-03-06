// testDracView.c ... test the DracView ADT
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "DracView.h"
#include "Places.h"
#define LARGE_ENOUGH 13960

void testDracView_1(void);
void testDracView_given(void);
void testDracView_2(void);

int main()
{   
    testDracView_1();
    testDracView_given();
    testDracView_2();
    puts("sickkkk all testing has passed");
	 return EXIT_SUCCESS;
}

void testDracView_2(void)
{

   DracView drac;

   puts("===== Testing The Fundamentals ======");

   PlayerMessage messagez[] = {"420", "BlazeIt", "Erryday", ""};

   drac = newDracView("GAL.... SAT.... HBE.... MGE....", messagez);

   assert(!giveMeTheRound(drac)); // round is currently 0 as Dracula has yet to take a turn
   assert(giveMeTheScore(drac) == 366); // score has not increased or decreased
   assert(whereIs(drac,PLAYER_LORD_GODALMING) == ALICANTE);
   assert(whereIs(drac,PLAYER_DR_SEWARD) == ATHENS);
   assert(whereIs(drac,PLAYER_VAN_HELSING) == BELGRADE);
   assert(whereIs(drac,PLAYER_MINA_HARKER) == GENEVA);
   assert(whereIs(drac,PLAYER_DRACULA) == UNKNOWN_LOCATION);

   disposeDracView(drac);

   puts("===== Testing Score / Health ======");

   drac = newDracView("GAL.... SAT.... HBE.... MGE.... DED....", messagez);

   assert(giveMeTheRound(drac) == 1); // first round as Dracula has made his first move
   assert(giveMeTheScore(drac) == 365); // when Dracula moves, score decreases by 1

   assert(howHealthyIs(drac,PLAYER_DRACULA) == 40);
   assert(howHealthyIs(drac,PLAYER_LORD_GODALMING) == 9);
   assert(howHealthyIs(drac,PLAYER_DR_SEWARD) == 9);
   assert(howHealthyIs(drac,PLAYER_VAN_HELSING) == 9);
   assert(howHealthyIs(drac,PLAYER_MINA_HARKER) == 9);

   disposeDracView(drac);

   puts("===== Testing Multiple Turns ======");

   drac = newDracView("GGO.... SGO.... HGO.... MGO.... DED.V.. "
                      "GFL.... SMR.... HMR.... MMR.... DTP.... "
                      "GVE.... STO.... HTO.... MTO.... DKLT... "
                      "GVI.... SSR.... HSR.... MSR.... DBDT... "
                      "GBDT... SSR.... HMA.... MMA.... DKL.... "
                      "GKLT... SSR.... HSN.... MSN.... DCD..V. "
                      "GCDD... SSR.... HSR.... MSR....", messagez);

   assert(giveMeTheRound(drac) == 6); // round 7 starts when Dracula finishes his turn, which he has yet to
   assert(giveMeTheScore(drac) == 347); // 6 turns have passed and one vampire has matured
   assert(howHealthyIs(drac,PLAYER_DRACULA) == 50); // 40 + 10 (teleport scroll back to homebase) + 10 (waled back to homebase) - 10 (confrontation with Lord Godalming)
   assert(howHealthyIs(drac,PLAYER_LORD_GODALMING) == 1); // Loss of 8 health = Encounter with Dracula (4) and two traps (4)
   assert(howHealthyIs(drac,PLAYER_DR_SEWARD) == 9); // Health cannot go above 9 even after resting
   assert(howHealthyIs(drac,PLAYER_VAN_HELSING) == 9);
   assert(howHealthyIs(drac,PLAYER_MINA_HARKER) == 9);

   LocationID goldamingPath[TRAIL_SIZE];
   giveMeTheTrail(drac,PLAYER_LORD_GODALMING,goldamingPath);
   assert(goldamingPath[0] == CASTLE_DRACULA);
   assert(goldamingPath[1] == KLAUSENBURG);
   assert(goldamingPath[2] == BUDAPEST);
   assert(goldamingPath[3] == VIENNA);
   assert(goldamingPath[4] == VENICE);
   assert(goldamingPath[5] == FLORENCE);

   LocationID sewardPath[TRAIL_SIZE];
   giveMeTheTrail(drac,PLAYER_DR_SEWARD,sewardPath);
   assert(sewardPath[0] == SARAGOSSA);
   assert(sewardPath[1] == SARAGOSSA);
   assert(sewardPath[2] == SARAGOSSA);
   assert(sewardPath[3] == SARAGOSSA);
   assert(sewardPath[4] == TOULOUSE);
   assert(sewardPath[5] == MARSEILLES);

   LocationID helsingPath[TRAIL_SIZE];
   giveMeTheTrail(drac,PLAYER_VAN_HELSING,helsingPath);
   assert(helsingPath[0] == SARAGOSSA);
   assert(helsingPath[1] == SANTANDER);
   assert(helsingPath[2] == MADRID);
   assert(helsingPath[3] == SARAGOSSA);
   assert(helsingPath[4] == TOULOUSE);
   assert(helsingPath[5] == MARSEILLES);

   LocationID harkerPath[TRAIL_SIZE];
   giveMeTheTrail(drac,PLAYER_MINA_HARKER,harkerPath);
   assert(harkerPath[0] == SARAGOSSA);
   assert(harkerPath[1] == SANTANDER);
   assert(harkerPath[2] == MADRID);
   assert(harkerPath[3] == SARAGOSSA);
   assert(harkerPath[4] == TOULOUSE);
   assert(harkerPath[5] == MARSEILLES);

   LocationID draculaPath[TRAIL_SIZE];
   giveMeTheTrail(drac,PLAYER_DRACULA,draculaPath);
   assert(draculaPath[0] == CASTLE_DRACULA);
   assert(draculaPath[1] == KLAUSENBURG);
   assert(draculaPath[2] == BUDAPEST);
   assert(draculaPath[3] == KLAUSENBURG);
   assert(draculaPath[4] == TELEPORT);
   assert(draculaPath[5] == EDINBURGH);

   disposeDracView(drac);

   puts("===== Testing Long Games ======");

   char testString[LARGE_ENOUGH] = "GJM.... SBI.... HBI.... MBI.... DCD.... ";
   int i;
   for(i = 0; i < 300; i++) {
      strcat(testString, "GJM.... SBI.... HBI.... MBI.... DCD.... ");
   }
   strcat(testString, "GJM.... SBI.... HBI.... MBI....");

   drac = newDracView(testString, messagez);
   assert(howHealthyIs(drac, PLAYER_DRACULA) == 3050);
   assert(giveMeTheScore(drac) == 65);
   assert(giveMeTheRound(drac) == 301);

   disposeDracView(drac);

}

void testDracView_given(void)
{
    int i;
    DracView dv;

    printf("Test for basic functions, just before Dracula's first move\n");
    PlayerMessage messages1[] = {"Hello","Rubbish","Stuff",""};
    dv = newDracView("GST.... SAO.... HZU.... MBB....", messages1);
    assert(giveMeTheRound(dv) == 0);
    assert(whereIs(dv,PLAYER_LORD_GODALMING) == STRASBOURG);
    assert(whereIs(dv,PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
    assert(whereIs(dv,PLAYER_VAN_HELSING) == ZURICH);
    assert(whereIs(dv,PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
    assert(whereIs(dv,PLAYER_DRACULA) == UNKNOWN_LOCATION);
    assert(howHealthyIs(dv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
    printf("passed\n");
    disposeDracView(dv);

    printf("Test for encountering Dracula and hunter history\n");
    PlayerMessage messages2[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!","","",""};
    dv = newDracView("GST.... SAO.... HCD.... MAO.... DGE.... "
                     "GGED... SAO.... HCD.... MAO....", messages2);
    assert(giveMeTheRound(dv) == 1);
    assert(whereIs(dv,PLAYER_DRACULA) == GENEVA);
    assert(howHealthyIs(dv,PLAYER_LORD_GODALMING) == 5);
    assert(howHealthyIs(dv,PLAYER_DRACULA) == 30);
    assert(whereIs(dv,PLAYER_LORD_GODALMING) == GENEVA);
    LocationID history[TRAIL_SIZE];
    giveMeTheTrail(dv,PLAYER_DRACULA,history);
    assert(history[0] == GENEVA);
    assert(history[2] == UNKNOWN_LOCATION);
    giveMeTheTrail(dv,PLAYER_LORD_GODALMING,history);
    assert(history[0] == GENEVA);
    assert(history[1] == STRASBOURG);
    assert(history[2] == UNKNOWN_LOCATION);
    giveMeTheTrail(dv,PLAYER_DR_SEWARD,history);
    assert(history[0] == ATLANTIC_OCEAN);
    assert(history[1] == ATLANTIC_OCEAN);
    assert(history[2] == UNKNOWN_LOCATION);
    printf("passed\n");        
    disposeDracView(dv);

    printf("Test for Dracula leaving minions\n");
    PlayerMessage messages3[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!","","","","Drop a V","Party in Strasbourg","Party","Party","Party"};
    dv = newDracView("GGE.... SGE.... HGE.... MGE.... DED.V.. "
                     "GST.... SST.... HST.... MST.... DMNT... "
                     "GST.... SST.... HST.... MST....", messages3);
    int nT, nV;
    whatsThere(dv,EDINBURGH,&nT,&nV);
    assert(nT == 0 && nV == 1);
    whatsThere(dv,MANCHESTER,&nT,&nV);
    assert(nT == 1 && nV == 0);
    assert(whereIs(dv,PLAYER_DRACULA) == MANCHESTER);
    giveMeTheTrail(dv,PLAYER_DRACULA,history);
    assert(history[0] == MANCHESTER);
    assert(history[1] == EDINBURGH);
    assert(history[2] == UNKNOWN_LOCATION);
    giveMeTheTrail(dv,PLAYER_MINA_HARKER,history);
    assert(history[0] == STRASBOURG);
    assert(history[1] == STRASBOURG);
    assert(history[2] == GENEVA);
    assert(history[3] == UNKNOWN_LOCATION);
    printf("passed\n");
    disposeDracView(dv);

    printf("Test for connections\n");
    int size, seen[NUM_MAP_LOCATIONS], *edges;

    printf("Checking Galatz road connections\n");
    PlayerMessage messages5[] = {"Gone to Galatz"};
    dv = newDracView("GGA....", messages5);
    edges = whereCanTheyGo(dv,&size,PLAYER_LORD_GODALMING,1,0,0);
    memset(seen, 0, NUM_MAP_LOCATIONS*sizeof(int));
    for (i = 0; i< size ; i++) seen[edges[i]] = 1;
    assert(size == 5); assert(seen[GALATZ]); assert(seen[CONSTANTA]);
    assert(seen[BUCHAREST]); assert(seen[KLAUSENBURG]); assert(seen[CASTLE_DRACULA]);
    free(edges);
    disposeDracView(dv);

    printf("Checking Ionian Sea sea connections\n");
    PlayerMessage messages6[] = {"Sailing the Ionian"};
    dv = newDracView("GIO....", messages6);
    edges = whereCanTheyGo(dv,&size,PLAYER_LORD_GODALMING,0,0,1);
    memset(seen, 0, NUM_MAP_LOCATIONS*sizeof(int));
    for (i = 0; i < size; i++) seen[edges[i]] = 1;
    assert(size == 7); assert(seen[IONIAN_SEA]); assert(seen[BLACK_SEA]);
    assert(seen[ADRIATIC_SEA]); assert(seen[TYRRHENIAN_SEA]);
    assert(seen[ATHENS]); assert(seen[VALONA]); assert(seen[SALONICA]);
    free(edges);
    disposeDracView(dv);

    printf("Checking Athens rail connections (none)\n");
    PlayerMessage messages7[] = {"Leaving Athens by train"};
    dv = newDracView("GAT....", messages7);
    edges = whereCanTheyGo(dv,&size,PLAYER_LORD_GODALMING,0,1,0);
    assert(size == 1);
    assert(edges[0] == ATHENS);
    free(edges);
    disposeDracView(dv);

    printf("passed\n");
}


void testDracView_1(void)
{
    int i;
    DracView new;
    PlayerMessage messages[] = {""};
    new = newDracView("GGE.... SGE.... HGE.... MGE.... DED....",messages);

    printf("testing round number and score...\n");
    i = giveMeTheRound(new); assert(i == 1);
    i = giveMeTheScore(new); assert(i == 365);
    new = newDracView("GGE.... SGE.... HGE.... MGE.... DED.... " 
                    "GGE.... SGE.... HGE.... MGE.... DED....",messages);
    i = giveMeTheRound(new); assert(i == 2);
    i = giveMeTheScore(new); assert(i == 364);
    new = newDracView("GGE.... SGE.... HGE.... MGE.... DED.... " 
                    "GGE.... SGE.... HGE.... MGE....",messages);
    i = giveMeTheRound(new); assert(i == 1); //round 2 starts when dracula finishes turn
    i = giveMeTheScore(new); assert(i == 365);
    printf("passed\n");

    printf("testing health points...\n");
    new = newDracView("GAS.... SAS.... HAS.... MAS.... DED.... "
                    "GAL.... SAL.... HAL.... MAL.... DTP.... "
                    "GAM.... SAM.... HAM.... MAM.... DED.... "
                    "GATT... SAT.... HAT.... MAT.... DED.... "
                    "GAO.... SAO.... HAO.... MAO.... DED.... "
                    "GBA.... SBA.... HBA.... MBA.... DED.... "
                    "GBI.... SBI.... HBA.... MBA....", messages); // incomplete round
    i = giveMeTheRound(new); assert(i == 6);
    i = giveMeTheScore(new); assert(i == 360);
    i = howHealthyIs(new, PLAYER_DRACULA); assert(i == 50); //teleported (+10 life)
    i = howHealthyIs(new, PLAYER_LORD_GODALMING); assert(i == 7); //encounter trap 

    
    new = newDracView("GAST... SAST... HAS.... MAS.... DCD.... "
                    "GAS.... SALT... HAL.... MAL.... DHI.... "
                    "GAMD... SAMT... HAM.... MAM.... DAM.... "
                    "GAM.... SATT... HAT.... MAT.... DED.... "
                    "GAOT... SAO.... HAO.... MAO.... DAS.... "
                    "GBA.... SBA.... HBA.... MBA.... DCD.... "
                    "GBI.... SBI.... HBI.... MBI...." , messages);
    i = howHealthyIs(new, PLAYER_LORD_GODALMING);
    assert(i == 6); //encounters trap (-2), rest (max health 9), encourter D (-4), rest (+3), trap (-2)
    i = howHealthyIs(new, PLAYER_DR_SEWARD);
    assert(i == 1); // damages from traps
    i = howHealthyIs(new, PLAYER_DRACULA);
    assert(i == 58); // Castle Dracula for three turns (+20) and 1 encounter (-10) and sea (-2)
    printf("passed\n");

    printf("testing score function...\n");
    char past_play_string_1[LARGE_ENOUGH] = "GAL.... SBI.... HBI.... MBI.... DBATV..";
    new = newDracView(past_play_string_1, messages);
    i = giveMeTheScore(new);
    assert(i == 365); //Dracula's turn finished (-1 score)
    strcat(past_play_string_1, " GBATD.. SBI.... HBI.... MBI.... DBAT.V.");
    new = newDracView(past_play_string_1, messages);
    i = giveMeTheScore(new);
    assert(i == 351); //Vampire matures (-13 score) and Dracula turn finished (-1 score)
    strcat(past_play_string_1, " GJMD... SBI.... HBI.... MBI.... DAS....");
    new = newDracView(past_play_string_1, messages);
    i = giveMeTheScore(new);
    assert(i == 344); //LORD_GODALMING sent to hospital (-6 score) and dracula's turn finished
    printf("passed\n");

    printf("testing for super long game...\n");
    for(i = 0; i < 344; i++){
        strcat(past_play_string_1, " GJM.... SBI.... HBI.... MBI.... DAS....");
    }
    new = newDracView(past_play_string_1, messages);
    i = giveMeTheScore(new);
    assert(i == 0);
    strcat(past_play_string_1, " GJM.... SBO.... HBI.... MMU.... DAS....");
    new = newDracView(past_play_string_1, messages);
    i = giveMeTheScore(new);
    // assert(i == 0); // no negative scores allowed
    printf("passed\n");

    printf("testing for where is function\n");
    new = newDracView("GAS.... SGE.... HGW.... MMU.... DCD....", messages);
    i = whereIs(new, PLAYER_LORD_GODALMING); assert(i == ADRIATIC_SEA);
    i = whereIs(new, PLAYER_DR_SEWARD); assert(i == GENEVA);
    i = whereIs(new, PLAYER_VAN_HELSING); assert(i == GALWAY);
    i = whereIs(new, PLAYER_MINA_HARKER); assert(i == MUNICH);
    i = whereIs(new, PLAYER_DRACULA); assert(i == CASTLE_DRACULA);
    printf("passed\n");

    printf("testing last move function\n");
    new = newDracView("GAS.... SGE.... HGW.... MMU.... DCD.... "
                    "GGE.... SAS.... HMU.... MGW.... DBI....", messages);
    LocationID start, end;
    lastMove(new, PLAYER_LORD_GODALMING, &start, &end);
    assert(start == ADRIATIC_SEA && end == GENEVA);
    lastMove(new, PLAYER_VAN_HELSING, &start, &end);
    assert(start == GALWAY && end == MUNICH);
    lastMove(new, PLAYER_DRACULA, &start, &end);
    assert(start == CASTLE_DRACULA && end == BARI);
    new = newDracView("GAS.... SGE.... HGW.... MMU.... DCD....", messages);
    lastMove(new, PLAYER_LORD_GODALMING, &start, &end);
    assert(start == UNKNOWN_LOCATION && end == ADRIATIC_SEA);
    lastMove(new, PLAYER_VAN_HELSING, &start, &end);
    assert(start == UNKNOWN_LOCATION && end == GALWAY);
    printf("passed\n");

    printf("testing trail...\n");
    new = newDracView("GAST... SAST... HAS.... MAS.... DCD.... "
                    "GBI.... SALT... HAL.... MAL.... DHI.... "
                    "GAMD... SAMT... HAM.... MAM.... DAM.... "
                    "GAM.... SATT... HAT.... MAT.... DED.... "
                    "GAOT... SAO.... HAO.... MAO.... DAS.... "
                    "GBA.... SBA.... HBA.... MBA.... DCD.... "
                    "GBI.... SBI.... HBI.... MBI...." , messages);
    LocationID trail[TRAIL_SIZE];
    giveMeTheTrail(new, PLAYER_LORD_GODALMING,trail);
    assert(trail[0] == BARI);
    assert(trail[1] == BARCELONA);
    assert(trail[2] == ATLANTIC_OCEAN);
    assert(trail[3] == AMSTERDAM);
    assert(trail[4] == AMSTERDAM);
    assert(trail[5] == BARI);
    printf("passed\n");

    printf("testing where can I go function...\n");
    int *connected;
    int size = 0;
    new = newDracView("GAS.... SAS.... HAS.... MAS.... DZA....", messages);
    connected = whereCanIgo(new, &size, 1, 1);
    for(i = 0; i < size; i++){ // to check none of the avilable locations is hospital 
        assert(connected[i] != ST_JOSEPH_AND_ST_MARYS);
    }
    connected = whereCanIgo(new, &size, 0, 0);
	printf("%d,%s\n",size,idToName(connected[0]));
    assert(size == 1 && connected[0] == ZAGREB); // original location should always be in array
    printf("passed\n");


    free(connected);
    disposeDracView(new);
}
