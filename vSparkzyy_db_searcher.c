/*

	Simple DB Searcher by vSparkzyy
	Shoutout To @mcknowyouraddy for the idea

*/

#include <stdio.h>

int main(void)
{	
	char *grep[1024];
	char *file[1024];
	char *yn[1024];
	char *command1[1024];
	search:;
	printf("\e[2J\e[H"); //clears screen 
	printf("DB Searcher By vSparkzyy\n");
	printf("\x1b[32mEnter String:\e[0m  ");
	scanf("%s", &grep);
	printf("\x1b[32mEnter Filename:\e[0m ");
	scanf("%s", &file);
	sprintf(command1, "cat %s | grep %s", file, grep);
	system(command1);
	printf("\x1b[32mWould You like to Search again? (y/n):\e[0m ");
	scanf("%s", &yn);
	if(strstr(yn, "y"))
	{
		goto search;
	}
	else
	{
		return;
	}
}