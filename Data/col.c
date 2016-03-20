#include<stdio.h>
float t[3000000][3];
int main()
{
	int i,n1,n2;
	scanf("%d%d",&n1,&n2);
	printf("%d\n",n2*9);
	for(i=0;i<n1;i++)
	{
		float a,b,c,d,e,f,g,h;
		scanf("%f%f%f%f%f%f%f%f",&a,&b,&c,&d,&e,&f,&g,&h);
		t[i][0]=d;
		t[i][1]=e;
		//t[i][2]=c;
	}
	for(i=0;i<n2*3;i++)
	{
		int s;
		scanf("%d",&s);
		if(t[s][0]>0.9 && t[s][1]> 0.9)
		{
			printf("%f %f %f\n",20/255.0,20/255.0,20/255.0);
		}
		else
		{	
			printf("%f %f %f\n",(t[s][0]),t[s][1],0.0);
			
		}
	}
}

