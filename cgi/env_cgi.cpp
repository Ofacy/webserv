#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
	size_t i = 0;
	printf("Status: 200 OK\r\n\r\n");
	while (*envp != NULL)
	{
		printf("%s\n", *envp);
		envp++;
		i++;
	}
	printf("Number of env variables: %zu\n", i);
	return 0;
}