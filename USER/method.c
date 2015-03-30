


void MemClear(char* buf, int num)
{
	int i;
	for(i=0;i<num;i++)
	{
		*buf = 0;
		buf++;
	}
}
