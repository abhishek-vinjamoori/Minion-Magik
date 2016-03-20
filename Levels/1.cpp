#include <stdio.h>
#include <stdlib.h>


int main()
{
	int s1;
	FILE * fp;

	fp = fopen ("1.dat", "r");
	//fputs("We are in 2012", fp);
	//while((s1=getchar())!=EOF)
	while(((s1 = fgetc(fp)) != EOF))
	{
		putchar(s1);
		//fscanf(fp, "%c",&s1);
		//printf("%c",s1);
	}
	fclose(fp);

	return(0);
}
