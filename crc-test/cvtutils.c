 

int asciiValueToBinary(int asciiInput)
{
	int res = 0, i = 1, rem;
        
	while (asciiInput > 0)
	{
		rem = asciiInput % 2;
		res = res + (i * rem);
		asciiInput = asciiInput / 2;
		i = i * 10;
	}

	return(res);
}

