int clamp(int x, int lower, int upper)
{
	if(x >= upper) return upper;
	if(x <= lower) return lower;
	return x;
}
