// Map.h ... interface to Map data type

#ifndef MAP_H
#define MAP_H

#include "Places.h"

typedef struct edge{
    LocationID  start;
    LocationID  end;
    TransportID type;
} Edge;

// graph representation is hidden 
typedef struct MapRep *Map; 

// operations on graphs 
Map  newMap();  
void disposeMap(Map g); 
void showMap(Map g); 
int  numV(Map g);
int  numE(Map g, TransportID t);
// Returns number of direct connections between two nodes 
// TransportID type is a 3 element array 
// type[0] is transport by ROAD
// type[1] is transport by RAIL
// type[2] is transport by BOAT
int connections(Map g, LocationID start, TransportID type[]);
LocationID *neighbor(Map g, LocationID start, int type[], int what, int len);

#endif
