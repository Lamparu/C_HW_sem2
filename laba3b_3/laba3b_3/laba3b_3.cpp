// Laba3b_3.cpp 

#include "pch.h"
#include <iostream>
#include <malloc.h>
#include <stdio.h>
#pragma warning(disable:4996)

typedef struct Item {
	char c;
	struct Item *next;
} Item;

int getList(Item**);
void putList(char*, Item*);
Item *deleteList(Item*);
Item *skipSpace(Item*);
Item *skipWord(Item*);
int checkWord(Item*);
Item *copyWord(Item *);
Item *reorg(Item *, Item *);
Item *findSpace(Item *);


int getList(Item **pptr)
{
	char buf[81], *str;
	Item head = { '*', NULL };
	Item *last = &head;
	int n, rc = 1;
	do {
		n = scanf("%80[^\n]", buf);
		if (n < 0)
		{
			deleteList(head.next);
			head.next = NULL;
			rc = 0;
			continue;
		}
		if (n > 0)
		{
			for (str = buf; *str != '\0'; ++str)
			{
				last->next = (Item*)malloc(sizeof(Item));
				last = last->next;
				last->c = *str;
			}
			last->next = NULL;
		}
		else
			scanf("%*c");
	} while (n > 0);
	*pptr = head.next;
	return rc;
}

Item *deleteList(Item *ptr)
{
	Item *tmp = NULL;
	while (ptr != NULL) {
		tmp = ptr;
		ptr = ptr->next;
		free(tmp);
	}
	return ptr;
}

void putList(const char *msg, Item *ptr)
{
	printf("%s:\"", msg);
	for (; ptr != NULL; ptr = ptr->next)
		printf("%c", ptr->c);
	printf("\"\n");
}

Item *findSpace(Item *ptr)
{
	while (ptr->c != ' ')
		ptr = ptr->next;
	return ptr;
}

Item *reorg(Item *p, Item *n)
{
	Item head = { '\0', p }, *last = &head, *delspace = NULL;
	Item headn = { '\0', n }, *lastn = &headn;
	while (last && (last->next = skipSpace(last->next)))
	{
		if (checkWord(last->next) == 1)
		{
			lastn->next = copyWord(last->next);
			lastn = findSpace(lastn->next);
		}
		last->next = skipWord(last->next);
	}
	lastn = headn.next;
	if (lastn != NULL)
	{
		while (lastn->next->next != NULL)
			lastn = lastn->next;
		delspace = lastn->next;
		free(delspace);
		lastn->next = NULL;
	}
	return headn.next;
}

Item *skipSpace(Item *ptr)
{
	Item *tmp = NULL;
	while ((ptr != NULL) && ((ptr->c == ' ') || (ptr->c == '\t')))
		ptr = ptr->next;
	return ptr;
}

int checkWord(Item *ptr)
{
	Item *firstlet = NULL, *lastlet = NULL;
	firstlet = ptr;
	while (((ptr->next) != NULL) && (((ptr->next)->c != ' ') && ((ptr->next)->c != '\t')))
		ptr = ptr->next;
	lastlet = ptr;
	if (lastlet->c == firstlet->c)
		return 1;
	else
		return 0;
}

Item *skipWord(Item *ptr)
{
	while ((ptr != NULL) && ((ptr->c != ' ') && (ptr->c != '\t')))
		ptr = ptr->next;
	return ptr;
}

Item *copyWord(Item *ptr)
{
	Item direct = { '*',NULL }, *to = &direct;
	while ((ptr != NULL) && ((ptr->c != ' ') && (ptr->c != '\t')))
	{
		to->next = (Item *)malloc(sizeof(Item));
		to = to->next;
		to->c = ptr->c;
		ptr = ptr->next;
	}
	to->next = (Item *)malloc(sizeof(Item));
	to = to->next;
	to->c = ' ';
	to->next = NULL;
	return direct.next;
}

int main()
{
	Item *st, *newl = NULL;
	while (puts("\n Enter string: "), getList(&st))
	{
		putList("Entered string", st);
		newl = reorg(st, newl);
		putList("Reorg string", newl);
		putList("Cheking", st);
		st = deleteList(st);
		newl = deleteList(newl);
	}
	return 0;
}

