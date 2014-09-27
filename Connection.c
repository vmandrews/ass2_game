// Returns number of direct connections between two nodes
//type is a 3 element array contianing the bools for road,rail,boat
int connections(Map g, LocationID start,int type[])
{
	VList curr = g->connections[start];
	int i = 0;
	for (; curr != NULL; curr = curr->next){
		if (type[curr->type - 1]){
			i++;
		}
	}
	return i;
}

//what indicates if we want location or transport
LocationID *neighbor(Map g, LocationID start, int type[], int what,int len)
{
	VList curr = g->connections[start];
	int i = 0;
	LocationID* Locortype = malloc(sizeof(LocationID)*len);
	for (; curr != NULL; curr = curr->next){
		if (type[curr->type - 1]){
			if (what == 0)Locortype[i++] = curr->v;
			else Locortype[i++] = curr->type;
		}
	}
	return Locortype;
}
