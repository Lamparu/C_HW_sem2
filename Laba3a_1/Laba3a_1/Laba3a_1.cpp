// создание новой строки
// начинающиеся на ту же букву, что первое слово

#include <iostream>
#include "pch.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#include "Laba3a_1.h"
/*
int main()
{
	char *s = NULL, *p = NULL;
	while (puts("Enter a string: "), s = getstr())
	{
		if (*s == NULL)
			printf("Your string is empty \n");
		else
		{
			printf("Your string: \"%s\"\n", s);
			p = changing(s);
			printf("The result: \"%s\"\n", p);
			free(s);
			free(p);
		}
	}
	puts("The end!");
	return 0;
}
*/
char *getstr()
{
	char *ptr = (char*)malloc(1);
	char buf[81];
	int n, len = 0;
	*ptr = '\0';
	do
	{
		n = scanf_s("%80[^\n]", buf, 81);
		if (n < 0)
		{
			free(ptr);
			ptr = NULL;
			continue;
		}
		if (n == 0)
			scanf_s("%*c");
		else
		{
			len += strlen(buf);
			ptr = (char*)realloc(ptr, len + 1);
			strcat(ptr, buf);
		}
	} while (n > 0);
	return ptr;
}

char *changing(char *s)
{
	char *p = (char*)malloc(strlen(s) + 1);
	char *ss = (char*)malloc(strlen(s) + 1);
	char *tmp = p, first[2];
	*first = '\0';
	*tmp = '\0';
	*ss = '\0';
	strcpy(ss, s);
	ss = skipSpace(ss);
	strncat(first, ss, 1);
	while (*(ss = skipSpace(ss)))
	{
		if (strncmp(ss, first, 1) == 0)
			tmp = copyWord(ss, tmp);
		ss = skipWord(ss);
	}
	if (p != tmp)
		*--tmp = '\0';
	p = (char*)realloc(p, strlen(p) + 1);
	return p;
}

char *skipSpace(char *s)
{
	int k = strspn(s, " \t");
	return s + k;
}

char *skipWord(char *s)
{
	int k = strcspn(s, " \t");
	return s + k;
}

char *copyWord(char *from, char *to)
{
	int k = strcspn(from, " \t");
	strncat(to, from, k);
	to += k;
	*to++ = ' ';
	*to = '\0';
	return to;
}
