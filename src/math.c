int clamp(int x, int lower, int upper)
{
	if(x >= upper) return upper;
	if(x <= lower) return lower;
	return x;
}

int nearest_power(int x, int pow)
{
	int num = pow;

	while(num < x) num *= pow;

	return num;
}

int hash_map_mapper(char* string, int max)
{
	unsigned long hash = 5381;
    int c;

    while ((c = *string++))
    {
        hash = ((hash << 5) + hash) + c; 
    }

    return hash % max;
}