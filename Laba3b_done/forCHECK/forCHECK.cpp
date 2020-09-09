// реорганизация введенной
// слова исходной, начинающиеся и заканчивающиеся на одну букву

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
Item *delSpace(Item*);
Item *delWord(Item*);
Item *skipWord(Item*);


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

Item *reorg(Item *p)
{
	Item head = { '\0', p }, *last = &head, *prev = NULL;
	Item *lastlet = '\0';
	while (last && (last->next = delSpace(last->next))) {
		lastlet = skipWord(last->next);
		if (last->next->c != lastlet->c) 
		{
			last->next = delWord(last->next);
		}
		else
		{
			prev = lastlet;
			last = lastlet->next;
			if (last)
				last->c = ' ';
		}
	}
	if (last && prev)
	{
		prev->next = NULL;
		free(last);
	}
	return head.next;
}

Item *delSpace(Item *ptr)
{
	Item *tmp = NULL;
	while ((ptr != NULL) && ((ptr->c == ' ') || (ptr->c == '\t')))
	{
		tmp = ptr;
		ptr = ptr->next;
		free(tmp);
	}
	return ptr;
}


Item *skipWord(Item *ptr)
{
	while ((ptr->next != NULL) && ((ptr->next->c != ' ') && (ptr->next->c != '\t')))
		ptr = ptr->next;
	return ptr;
}

Item *delWord(Item *ptr)
{
	Item *tmp = NULL;
	while ((ptr != NULL) && ((ptr->c != ' ') && (ptr->c != '\t')))
	{
		tmp = ptr;
		ptr = ptr->next;
		free(tmp);
	}
	return ptr;
}


int main()
{
	Item *st;
	while (puts("Enter string: "), getList(&st))
	{
		putList("Entered string", st);
		st = reorg(st);
		putList("Reorg string", st);
		st = deleteList(st);
	}
	return 0;
}

