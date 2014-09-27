#define NUM_TRANS 3

// Returns number of direct connections between two nodes 
// TransportID type is a 3 element array 
// type[0] is transport by ROAD
// type[1] is transport by RAIL
// type[2] is transport by BOAT
int connections(Map g, LocationID start, LocationID end, TransportID type[])
{
    VList curr = g->connections[start];
    int i = 0;
    for (; curr != NULL; curr = curr->next){
        if (curr->v == end){
            type[curr->type - 1]++;
            i++;
        }
    }
    return i;
}

void resetType(TransportID type[])
{
    type[0] = 0;
    type[1] = 0;
    type[2] = 0;
}

//// Functions that query the map to find information about connectivity
// Returns an array of LocationIDs for all directly connected locations
LocationID *connectedLocations(GameView currentView, int *numLocations,
    LocationID from, PlayerID player, Round round,
    int road, int rail, int sea)
{
    TrapVam connect = malloc(sizeof(trapVam));
    Map g = newMap();
    TransportID type[NUM_TRANS] = { 0 }; //stores type of transport, look down for more info
    connect->trap = init(); // this q stores the locationID
    connect->vam = init(); //  this q stores the type of transport

    int i = ADRIATIC_SEA, j, numConnect; //i is a location counter, j is a Transport type counter used to index array
    //j = 0 is ROAD, = 1 is RAIL, = 2 is BOAT
    //e.g type[0] = 1 means there is 1 road tranport, type[1] = 0 means there is no rail transport
    for (; i <= ZURICH; i++){ //loops all the locationID
        numConnect = connections(g, from, i, type); //numConnect stores the number of connections
        for (j = 0; numConnect && j < NUM_TRANS; j++){ //numConnect && j < NUM_TRANS means that the loop will run if numConnect != 0(so another way of wrting this
            //would be putting a if(numConnect != 0) outside the for loop
            if (type[j]){ //if there is this way of transport
                push(connect->trap, i);//trap stores the locationID
                push(connect->vam, j); //vam stores the type of transport
            }
        }
        resetType(type); //reset the types = 0
    }
    /*
    Down here you might first want to use if statements to filter the round limited moves
    so maybe pop both trap and vam. check if vam is a valid transport if yes push it back in, if no disgard it

    then you want to rule out duplicates... simple way would be
    1. Create another newq
    LocationID it = pop(trap);
    if(trapNum(trap,it) == 0) //this means that "it" only occured once (we took it out so originally it was 1)
    push(newq,it);
    else //so more than 1 occurence(duplicate)
    for(counter = trapNum(trap,it);counter > 0;counter--){ //trapNum returns how many of this location is still in the q
    moveTrap(stuff); //remove it 1 by 1 (moveTrap removes 1 element a time)
    }

    repeat the above until q is empty

    then push "from" into the newq and get the size of this q
    the size would be the number of locations and probably the number you want to malloc in order to get the locationID string.
    */
    return NULL;
}
