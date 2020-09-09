// хэш-таблица
// перемешивание сложением 


#include "pch.h"
#include <iostream>
#pragma warning(disable:4996)

struct Info {
	int release; //номер версии
	char *info;
	struct Info *next;
};

struct Item {
	int busy; //занятость
	int key; //ключ эл-та таблицы
	struct Info *pinfo; //инфа для ключа

};

const int SZ = 11;
struct Table { //просматриваемая таблица - вектор
	int n; //размер талицы 
	Item vec[SZ]; //вектор фикс. размера
};

const char *msgs[] = { "0. Quit", "1. Add", "2. Find", "3. Delete", "4. Show" };
const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

const char *errmsgs[] = { "Ok", "Table overflow" };

int dialog(const char *msgs[], int);
int getInt(int*);
int D_Add(Table*),
	D_Find(Table*),
	D_Delete(Table*),
	D_Show(Table*);
char *getStr();
int insert(Table*, int, char *);
int(*fptr[])(Table*) = { NULL, D_Add, D_Find, D_Delete, D_Show};
int hash1(int);
int hash2(int);
int findkey(Table, int);
int findrel(Table, int, int);
int delkey(Table*);
int delrel(Table*);
int delTable(Table*);


char *getStr()
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

int getInt(int *n)
{
	int rc;
	do
	{
		rc = scanf_s("%d", n);
		if (rc <= 0)
		{
			if (rc == -1)
				exit(0);
			printf("Wrong input. Try again. \n");
			scanf_s("%*[^\n]");
		}
		scanf("%*c");
	} while (rc <= 0);
	//scanf_s("%*[^\n]");
	//добавить очистку буффера 
	return 1;
}

int dialog(const char *msgs[], int N) //указатель на указатель на 1ый эл-т массива
{
	const char *errmsg = "";
	int i, n = 0;
	do {
		puts(errmsg);
		errmsg = "U r wrong. Repeat, pls!";
		for (i = 0; i < N; ++i)
			puts(msgs[i]);
		puts("Make ur choice: -->");
		getInt(&n);
	} while (n < 0 || n >= N);
	return n;
}

int D_Find(Table *ptab) {
	int rc, k = 0, rel = 0;
	int choice = 0;
	printf("== Enter 1 to find by a key == \n== Enter 2 to find by a key and a release == \n");
	do
	{
		getInt(&choice);
		if ((choice != 1) && (choice != 2))
			printf("Wrong input. Try again. \n");
	} while ((choice != 1) && (choice != 2));
	if (choice == 1) {
		printf("Enter a key -->");
		getInt(&k);
		rc = findkey(*ptab, k);
		if (rc < 0)
			printf("\nNothing found\n");
	}
	if (choice == 2) {
		printf("Enter a key -->");
		getInt(&k);
		printf("\nEnter a release -->");
		getInt(&rel);
		rc = findrel(*ptab, k, rel);
		if (rc < 0)
			printf("\nNothing found\n");
	}
	return 1;
}

int findkey(Table ptab, int k) {
	Info *tmp;
	Table tabfind = { 0, NULL };
	int j, h, n = 0;
	j = hash1(k);
	h = hash2(k);
	while ((ptab.vec[j].busy != 0) && (n < SZ)) {
		if ((ptab.vec[j].busy == 1) && (ptab.vec[j].key == k)) {
			tmp = ptab.vec[j].pinfo;
			while (tmp != NULL) {
				insert(&tabfind, k, tmp->info);
				tmp = tmp->next;
			}
			printf("Table with found elements: \n");
			D_Show(&tabfind);
			delTable(&tabfind);
			return j;
		}
		j = (j + h) % SZ;
		n = n + 1;
	}
	return -1;
}

int findrel(Table ptab, int k, int rel) {
	Info *tmp;
	int j, h, n = 0;
	j = hash1(k);
	h = hash2(k);
	while ((ptab.vec[j].busy != 0) && (n < SZ)) {
		if ((ptab.vec[j].busy == 1) && (ptab.vec[j].key == k)) {
			tmp = ptab.vec[j].pinfo;
			while (tmp != NULL) {
				if (tmp->release == rel) {
					printf("\nFound element: \n");
					printf("\ninfo: \"%s\" ", tmp->info);
					printf("release: %d \n", tmp->release);
					return j;
				}
				tmp = tmp->next;
			}
		}
		j = (j + h) % SZ;
		n = n + 1;
	}
	return -1;
}

int D_Add(Table *ptab)
{
	int k = 1, rc;
	char *info = NULL;
	printf("Enter key: -->");
	getInt(&k);
	printf("Enter info:\n");
	do info = getStr();
	while (info == NULL);
	rc = insert(ptab, k, info);
	printf("%s: %d \"%s\" \n", errmsgs[rc], k, info);
	free(info);
	return 1;
}

int D_Show(Table *ptab)
{
	Info *tmp = NULL;
	int i = 0;
	for (; i < SZ; i++) {
		if (ptab->vec[i].busy == 1) {
			//printf("\n%d ", i);
			printf("key: %d \n", ptab->vec[i].key);
			tmp = ptab->vec[i].pinfo;
				while (tmp != NULL) {
					printf("info: \"%s\" ", tmp->info);
					printf("release: %d \n", tmp->release);
					tmp = tmp->next;
				}
		}
	}
	return 1;
}

int insert(Table *ptab, int k, char *str)
{
	int j, n = 0, h;
	Info *last;
	j = hash1(k);
	h = hash2(k);
	while ((ptab->vec[j].busy == 1) && (n < SZ) && (ptab->vec[j].key != k)) {

		j = (j + h) % SZ;
		n = n + 1;
	}
	if (n < SZ) {
		last = (Info*)malloc(sizeof(Info));
		last->info = strdup(str);
		//last->info = (char*)malloc(sizeof(char)*(strlen(str) + 1));
		//strcpy(last->info, str);
		//last->release = rel + 1;
		last->next = NULL;
		if (ptab->vec[j].busy == 1) {
			//rel = ptab->vec[j].pinfo->release;//
			//last->release = rel + 1;
			last->release = ptab->vec[j].pinfo->release + 1;
			last->next = ptab->vec[j].pinfo;
			ptab->vec[j].pinfo = last;
		}
		else {
			last->release = 1; //
			(*ptab).vec[j].busy = 1;
			(*ptab).vec[j].key = k;
			(*ptab).vec[j].pinfo = last;
		}
		++((*ptab).n);
	}
	else
		return 1;
	return 0;
}

int D_Delete(Table* ptab)
{
	int rc, k = 0, rel = 0;
	int choice = 0;
	printf("== Enter 1 to delete by a key == \n== Enter 2 to delete by a key and a release == \n");
	do
	{
		getInt(&choice);
		if ((choice != 1) && (choice != 2))
			printf("Wrong input. Try again. \n");
	} while ((choice != 1) && (choice != 2));
	if (choice == 1) {
		rc = delkey(ptab);
		if (rc < 0)
			printf("\nNothing found\n");
	}
	if (choice == 2) {
		rc = delrel(ptab);
		if (rc < 0)
			printf("\nNothing found\n");
	}
	return 1;
}

int delkey(Table *ptab) 
{
	Info *tmp = NULL, *ptr = NULL;
	int k = 0, j =0, n = 0;
	printf("Enter a key -->");
	getInt(&k);
	j = findkey(*ptab, k);
	if (j < 0)
		return -1;
	tmp = ptab->vec[j].pinfo;
	while (tmp != NULL) {
		ptr = tmp;
		tmp = tmp->next;
		free(ptr);
		ptab->n--;
	}
	printf("\nElements were deleted\n");
	ptab->vec[j].busy = -1;
	ptab->n--;
	return 1;
}

int delrel(Table *ptab)
{
	Info *tmp = NULL, *ptr = NULL, *tpn = NULL;
	int k = 0, j = 0, n = 0, rel = 0;
	printf("Enter a key -->");
	getInt(&k);
	printf("\nEnter a release -->");
	getInt(&rel);
	j = findrel(*ptab, k, rel); //не работает, если элемент последний в списке
	if (j < 0)
		return -1;
	tmp = ptab->vec[j].pinfo;
	if (tmp->release == rel) { //если 1ый элемент списка
		ptr = tmp;
		ptab->vec[j].pinfo = tmp->next;
		free(ptr);
		ptab->n--;
	}
	else { //другие элементы списка
		while (tmp->next != NULL) {
			tpn = tmp->next;
			if (tpn->release == rel) {
				tmp->next = tpn->next;
				ptr = tpn;
				tmp = tmp->next;
				free(ptr);
				printf("Element was deleted\n");
				ptab->n--;
				return 1;
			}
			tmp = tmp->next;
		}
	}
	if (ptab->vec[j].pinfo == NULL) 
		ptab->vec[j].busy = -1;
	printf("The last released element was deleted\n");
	return 1;
}

int hash1(int k) 
{
	return abs(k) % SZ;
}

int hash2(int k)
{
	return abs(k) % (SZ / 2) + 1;
}

int delTable(Table *ptab)
{
	Info *tmp = NULL, *ptr = NULL;
	int j = 0;
	for (; j < SZ; j++) {
		if (ptab->vec[j].busy == 1) {
			tmp = ptab->vec[j].pinfo;
			while (tmp != NULL) {
				ptr = tmp;
				tmp = tmp->next;
				free(ptr->info);
				free(ptr);
			}
		}
	}
	printf("\nTable was deleted\n");
	return 1;
}

int main()
{
	Table table = {0, NULL};
	int rc;
	while (rc = dialog(msgs, NMsgs))
		if (!fptr[rc](&table))
			break;
	delTable(&table);
	printf("\nbb\n");
	return 0;
}
