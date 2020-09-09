// хэш-таблица
// перемешивание сложением 
// 2 файла 

#include "pch.h"
#include <iostream>
#pragma warning(disable:4996)

struct Info {
	int release; //номер версии
	int len;
	int offset;
	struct Info *next;
};

struct Item {
	int busy; //занятость
	int key;
	struct Info *pinfo; //инфа для ключа

};

struct Table { //просматриваемая таблица - вектор
	int n; //размер талицы 
	int SZ; //размер вектора
	char *fname;
	Item *vec; //таблица - вектор
	FILE *fd;
};

const char *msgs[] = { "0. Quit", "1. Add", "2. Find", "3. Delete", "4. Show"};
const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

const char *errmsgs[] = { "Ok", "Table overflow" };

int dialog(const char *msgs[], int);
int getInt(int*);
int D_Add(Table*),
D_Find(Table*),
D_Delete(Table*),
D_Show(Table*),
D_Load(Table*),
D_Save(Table*);
char *getStr();
int insert(Table*, int, char *);
int(*fptr[])(Table*) = { NULL, D_Add, D_Find, D_Delete, D_Show};
int hash1(Table*, int);
int hash2(Table*, int);
int findkey(Table, int);
int findrel(Table, int, int);
int delkey(Table*);
int delrel(Table*);
int delTable(Table*);
int save(Table*);
int load(Table*, char*);
int insert_found(Table*, Info*, int);


int hash1(Table *ptab, int k)
{
	return abs(k) % ptab->SZ;
}

int hash2(Table *ptab, int k)
{
	return abs(k) % (ptab->SZ / 2) + 1;
}

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

int find(Table ptab, int k)
{
	int j, h, n = 0;
	j = hash1(&ptab, k);
	h = hash2(&ptab, k);
	while ((ptab.vec[j].busy != 0) && (n < ptab.SZ)) {
		if ((ptab.vec[j].busy == 1) && (ptab.vec[j].key == k)) {
			return j;
		}
		j = (j + h) % ptab.SZ;
		n = n + 1;
	}
	return -1;
}

int findkey(Table ptab, int k) {
	Info *tmp;
	Table tabfind = { 0, ptab.SZ, NULL };
	char *info = NULL;
	int j = find(ptab, k);
	if (j >= 0) {
		tabfind.vec = (Item*)calloc(tabfind.SZ, sizeof(Item));
		tabfind.fd = ptab.fd;
		tmp = ptab.vec[j].pinfo;
		while (tmp != NULL) {
			insert_found(&tabfind, tmp, k);
			tmp = tmp->next;
		}
		D_Show(&tabfind);
		delTable(&tabfind);
		return 1;
	}
	return -1;

}

int findrel(Table ptab, int k, int rel) {
	Info *tmp;
	char *info = NULL;
	int j, h, n = 0;
	j = hash1(&ptab, k);
	h = hash2(&ptab, k);
	while ((ptab.vec[j].busy != 0) && (n < ptab.SZ)) {
		if ((ptab.vec[j].busy == 1) && (ptab.vec[j].key == k)) {
			tmp = ptab.vec[j].pinfo;
			while (tmp != NULL) {
				if (tmp->release == rel) {
					printf("\nFound element: \n");
					fseek(ptab.fd, tmp->offset, SEEK_SET);
					info = (char*)calloc(tmp->len, sizeof(char));
					fread(info, sizeof(char), tmp->len, ptab.fd);
					printf("\ninfo: \"%s\" ", info);
					printf("release: %d \n", tmp->release);
					return j;
				}
				tmp = tmp->next;
			}
		}
		j = (j + h) % ptab.SZ;
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
	char *info = NULL;
	Info *tmp = NULL;
	int i = 0;
	for (; i < ptab->SZ; i++) {
		if (ptab->vec[i].busy == 1) {
			printf("\n%d ", i);
			printf("key: %d \n", ptab->vec[i].key);
			tmp = ptab->vec[i].pinfo;
			while (tmp != NULL) {
				fseek(ptab->fd, tmp->offset, SEEK_SET); 
				info = (char*)calloc(tmp->len, sizeof(char));
				fread(info, sizeof(char), tmp->len, ptab->fd);
				printf("info: \"%s\" ", info);
				printf("release: %d \n", tmp->release);
				tmp = tmp->next;
			}
		}
	}
	return 1;
}

int insert(Table *ptab, int k, char *str)
{
	int j, n = 0, h, SZ;
	Info *last;
	j = hash1(ptab, k);
	h = hash2(ptab, k);
	while ((ptab->vec[j].busy == 1) && (n < ptab->SZ) && (ptab->vec[j].key != k)) {
		j = (j + h) % (ptab->SZ);
		n++;
	}
	if (n >= ptab->SZ)
		return 1;
	else {
		last = (Info*)malloc(sizeof(Info));
		last->len = strlen(str) + 1;
		last->next = NULL;
		fseek(ptab->fd, 0, SEEK_END);
		last->offset = ftell(ptab->fd);
		fwrite(str, sizeof(char), last->len, ptab->fd);
		if (ptab->vec[j].busy == 1) {
			last->release = ptab->vec[j].pinfo->release + 1;
			last->next = ptab->vec[j].pinfo;
			ptab->vec[j].pinfo = last;
		}
		else {
			last->release = 1; 
			(*ptab).vec[j].busy = 1;
			(*ptab).vec[j].key = k;
			(*ptab).vec[j].pinfo = last;
			++((*ptab).n);
		}
	}
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
	int k = 0, j = 0, n = 0;
	printf("Enter a key -->");
	getInt(&k);
	j = find(*ptab, k);
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
	ptab->vec[j].busy = 0;
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
		ptab->vec[j].busy = 0;
	printf("The last released element was deleted\n");
	return 1;
}

int delTable(Table *ptab)
{
	Info *tmp = NULL, *ptr = NULL;
	int j = 0;
	for (; j < ptab->SZ; j++) {
		if (ptab->vec[j].busy == 1) {
			tmp = ptab->vec[j].pinfo;
			while (tmp != NULL) {
				ptr = tmp;
				tmp = tmp->next;
				free(ptr);
			}
		}
	}
	printf("\nTable was deleted\n");
	return 1;
}

int main()
{
	Table table = { 0, 0, NULL, NULL, NULL };
	int rc;
	if (D_Load(&table) == 0)
		return 1;
	while (rc = dialog(msgs, NMsgs))
		if (!fptr[rc](&table))
			break;
	D_Save(&table);
	delTable(&table);
	printf("\nbb\n");
	return 0;
}

int load(Table *ptab, char *fname) //загрузка таблицы из файла
{//ptab->fname, ftav - файл таблицы, 
//fdatname, ptab->fd - файл с инфой 
	//int *SZ = NULL, *n = NULL, *key = NULL;
	int SZ, n, key;
	int i, j = 0, size;
	FILE *ftab = NULL; //файл таблицы 
	char *fdatname; //файл данных 
	int len = strlen(fname);
	Info *cur = NULL, *last = NULL;
	//Info info = { 0, 0, NULL }; // убрать релиз в конец структуры 
	Info info = { 0, 0, 0, NULL };
	Item item = { 0, 0, NULL };
	
	//last->info = (char*)malloc(sizeof(char)*(strlen(str) + 1));
	//strcpy(last->info, str);
	fdatname = (char*)malloc(sizeof(char)*(len + 5));
	strcpy(fdatname, fname);
	//fdatname = _strdup(fname);
	fdatname = (char*)realloc(fdatname, len + 5);
	ptab->fname = (char*)malloc(sizeof(char)*(len + 5));
	strcpy(ptab->fname, fname);
	strcat_s(ptab->fname, len + 5, ".tab");
	fdatname = (char*)realloc(fdatname, len + 5);
	strcat_s(fdatname, len + 5, ".dat");
	//файл таблицы
	fopen_s(&ftab, ptab->fname, "rb"); //open .tab read
	if (ftab != 0) {
		//файл данных
		fopen_s(&(ptab->fd), fdatname, "r + b"); //open .dat read+write 
		if (ptab->fd == NULL) {
			free(fdatname);
			return 0;
		}
		fread(&SZ, sizeof(int), 1, ftab); //чтение размера таблицы
		ptab->SZ = SZ;
		ptab->vec = (Item*)calloc(ptab->SZ, sizeof(Item));
		while (j < SZ) {//чтение кол-ва элементов
			fread(&n, sizeof(int), 1, ftab);
			if (n != 0) {
				ptab->n++;
				fread(&item, sizeof(int), 2, ftab); //ключ + busy
				ptab->vec[j] = item;
				for (i = 1; i <= n; i++) {
					fread(&info, sizeof(int), 3, ftab);
					cur = (Info*)calloc(1, sizeof(Info));
					*cur = info;
					if (i != 1)
						last->next = cur;
					else
						ptab->vec[j].pinfo = cur;
					last = cur;
				}
			}
			else
				fseek(ftab, 2*sizeof(int), SEEK_CUR);
			j++;
		}
		fclose(ftab); //close .tab
	}
	else {
		fopen_s(&(ptab->fd), fdatname, "w+b"); //open(create) .dat read + write
		fopen_s(&ftab, ptab->fname, "wb+"); //open(create) .tab read + write
		if (ptab->fd == NULL) {
			free(fdatname);
			return 0;
		}
		printf("Enter table size -->");
		do getInt(&size);
		while (size % 2 == 0 || size < 3);
		ptab->SZ = size;
		ptab->vec = (Item*)calloc(ptab->SZ, sizeof(Item));
		fwrite(&(ptab->SZ), sizeof(int), 1, ftab);
		fwrite(ptab->vec, sizeof(Item), ptab->SZ, ftab);
		fclose(ftab); //close .tab
	}
	free(fdatname);
	return 1;
}

int save(Table *ptab) //выгрузка таблицы в файл
{
	int i, n =0;
	FILE *ftab = NULL;
	Info  *cur = NULL;
	fclose(ptab->fd); //close .dat
	fopen_s(&ftab, ptab->fname, "wb+"); //open(create) .tab write
	if (ftab == NULL)
		return 0;
	fseek(ftab, 0, SEEK_SET);
	fwrite(&(ptab->SZ), sizeof(int), 1, ftab);
	for (i = 0; i <= ptab->SZ; i++) {
		n = 0;
		//сначала записать кол-ва эл-тов
		//потом запись Item
		//потом запись Info
		fseek(ftab, sizeof(int), SEEK_CUR);
		fwrite(&ptab->vec[i].busy, sizeof(int), 1, ftab);
		fwrite(&ptab->vec[i].key, sizeof(int), 1, ftab);
		//мб можно в один Item записать
		if (ptab->vec[i].busy == 1) {
			cur = ptab->vec[i].pinfo;
			while (cur != NULL) {
				fwrite(cur, sizeof(int), 3, ftab);
				cur = cur->next;
				n++;
			}
		}
		fseek(ftab, -3*(n+1)*sizeof(int), SEEK_CUR);
		fwrite(&n, sizeof(int), 1, ftab);
		fseek(ftab, 0, SEEK_END);
	}
	fclose(ftab); //close .tab
	return 1;
}

int D_Load(Table *ptab) {
	int rc = 1;
	char *fname = NULL;
	printf("Enter file name: -->");
	fname = getStr();
	if (fname == NULL)
		return 0;
	rc = load(ptab, fname); //открытие и закрытие табличного файла, открытие файла данных
	if (rc == 0)
		puts("The appropriate data file doesn't exist.");
	free(fname);
	return rc;
}

int D_Save(Table *ptab) 
{
	int rc;
	rc = save(ptab);
	if (rc == 0)
		printf("Create file error.");
	fclose(ptab->fd);
	return 1;
}

int insert_found(Table *newtab, Info *ptr, int k)
{
	int j;
	Info *last;
	last = (Info*)malloc(sizeof(Info));
	last->len = ptr->len;
	last->offset = ptr->offset;
	last->release = ptr->release;
	last->next = NULL;
	j = hash1(newtab, k);
	if (newtab->vec[j].busy == 1) {
		last->next = newtab->vec[j].pinfo;
		newtab->vec[j].pinfo = last;
	}
	else {
		(*newtab).vec[j].busy = 1;
		(*newtab).vec[j].key = k;
		(*newtab).vec[j].pinfo = last;
		++((*newtab).n);
	}
	return 1;
}