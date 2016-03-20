#include<iostream>
#include<stdio.h>
#include<fstream>
using namespace std;
int main()
{
	char a;
	int cnt=0;
	fstream colorfile("1.dat", ios_base::in);
	while (colorfile >> a)
	{
		if(cnt==0)
		{
			cnt++;
			continue;
		}
		cout << a << endl;
		cnt++;
	}
}
