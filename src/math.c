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