// AVL - tree
// вывод min -> max
// поиск max

#include "pch.h"
#include <iostream>
#include <time.h>
#include <fstream>
#pragma warning(disable:4996)


struct Node {
	int height;
	char *key;
	char *info;
	struct Node *left, *right, *parent;
};

const char *msgs[] = { "0. Quit", "1. Add", "2. Find", "3. Delete", "4. Show", "5. Timing" };
const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

const char *errmsgs[] = { "Ok", "Double keys" };

Node infElist = { 0, 0, NULL, NULL, NULL };
Node *Elist = &infElist;
Node *root = Elist;

int D_Add(Node**),
D_Find(Node**),
D_Delete(Node**),
D_Show(Node**),
D_Timing(Node **);
int(*fptr[])(Node**) = { NULL, D_Add, D_Find, D_Delete, D_Show , D_Timing };
int dialog(const char *msgs[], int);
int getInt(int*);
char *getStr();
int getInt(int*);
void showTree(Node*, int);
void MaxMin(Node*);
void delTree(Node**);
Node* findMax(Node*);
Node* findNext(Node*);
Node* findMin(Node*);
Node* checkKey(Node*, char*);
Node* delKey(Node*, Node*);
Node* left_rotate(Node*);
Node* right_rotate(Node*);

Node* insertTree(Node*, char*, char*, Node*);
Node* add(char*, char*, Node*);
int height(Node*);
int balance(Node*);
int imax2(int, int);
Node* leftright_rotate(Node*);
Node* rightleft_rotate(Node*);
char* getRandomStr(int);

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

int D_Load(Node **root)
{
	FILE* file;
	Node *rc;
	int k;
	char *info = NULL;
	char *key = NULL;
	fopen_s(&file, "data.txt", "r");
	info = (char*)malloc(sizeof(char));
	key = (char*)malloc(sizeof(char));
	printf("Data from file: \n");
	while (fscanf(file, "%s", key) != -1)
	{
		fscanf(file, "*c");
		fscanf(file, "%s", info);
		fscanf(file, "*c");
		rc = checkKey(*root, key);
		if (rc == Elist)
		{
			*root = insertTree(*root, key, info, (*root)->parent);
			k = 0;
		}
		else
			k = 1;
		//showTree(*root, 1);
		printf("%s: \"%s\" \"%s\" \n", errmsgs[k], key, info);
	}
	printf("\nTree from data file: \n\n");
	showTree(*root, 1);
	fclose(file);
	return 1;
}

Node* add(char* key, char* info, Node *par)
{
	Node *x;
	x = (Node*)malloc(sizeof(Node));
	if (x != NULL)
	{
		x->key = _strdup(key);
		x->info = _strdup(info);
		x->height = 0;
		x->right = Elist;
		x->left = Elist;
		x->parent = par;
	}
	return x;
}

Node* insertTree(Node *root, char* key, char* info, Node *par)
{
	if (root == Elist)
		return add(key, info, par);
	if (strcmp(key, root->key) < 0)
	{
		root->left = insertTree(root->left, key, info, root);
		if (balance(root) == -2)
		{
			if (strcmp (key, root->left->key) < 0)
				root = right_rotate(root);
			else
				root = leftright_rotate(root);
		}
		else
			if (balance(root) == -1 && root->right == Elist)
			{
				if (strcmp(key, root->left->key) < 0)
					root = right_rotate(root);
				else
					root = leftright_rotate(root);
			}
	}
	if (strcmp(key, root->key) > 0)
	{
		root->right = insertTree(root->right, key, info, root);
		if (balance(root) == 2)
		{
			if (strcmp(key, root->right->key) > 0)
				root = left_rotate(root);
			else
				root = rightleft_rotate(root);
		}
		else
			if (balance(root) == 1 && root->left == Elist)
			{
				if (strcmp(key, root->right->key) > 0)
					root = left_rotate(root);
				else
					root = rightleft_rotate(root);
			}
	}

	if (root->left == Elist && root->right == Elist)
		root->height = 0;
	else
		root->height = imax2(height(root->left), height(root->right)) + 1;
	return root;
}

int D_Add(Node **root)
{
	Node *rc;
	int k;
	char *info = NULL, *key = NULL;
	printf("Enter key: -->");
	do key = getStr();
	while (key == NULL);
	//проверка на совпадение с другими ключами
	printf("Enter info:\n");
	do info = getStr();
	while (info == NULL);
	rc = checkKey(*root, key);
	if (rc == Elist)
	{
		*root = insertTree(*root, key, info, (*root)->parent);
		k = 0;
	}
	else
		k = 1;
	printf("%s: \"%s\" \"%s\" \n", errmsgs[k], key, info);
	free(info);

	return 1;
}

int D_Show(Node **root)
{
	int choice;
	Node *ptr;
	printf("== Enter 1 to show a tree == \n== Enter 2 to show data from most to least ==\n");
	do
	{
		getInt(&choice);
		if ((choice != 1) && (choice != 2))
			printf("Wrong input. Try again. \n");
	} while ((choice != 1) && (choice != 2));
	if (choice == 1)
		showTree(*root, 1);
	if (choice == 2)
	{
		ptr = *root;
		MaxMin(ptr);
	}
	return 1;
}

int D_Find(Node **root)
{
	int k, choice;
	Node *rc;
	char *key = NULL;
	printf("== Enter 1 to find by a key == \n== Enter 2 to find MAX\n");
	do
	{
		getInt(&choice);
		if ((choice != 1) && (choice != 2))
			printf("Wrong input. Try again. \n");
	} while ((choice != 1) && (choice != 2));
	if (choice == 1)
	{
		printf("Enter a key-->");
		do key = getStr();
		while (key == NULL);
		rc = checkKey(*root, key);
		if (rc == Elist)
			printf("Nothing found\n");
		else
			printf("Found element -- key:  \"%s\"  info: \"%s\" \n", rc->key, rc->info);
	}
	if (choice == 2)
	{
		rc = findMax(*root);
		if (rc == NULL)
			printf("Tree is empty\n");
		else
			printf("MAX -- key: \"%s\"  info: \"%s\" \n", rc->key, rc->info);
	}
	return 1;
}

int D_Delete(Node **root)
{
	Node  *x, *y, *p, *par;
	char *key = NULL;
	printf("Enter a key to delete-->");
	do key = getStr();
	while (key == NULL);
	x = checkKey(*root, key);
	if (x == Elist)
		printf("Nothing found");
	else
	{
		printf("Element --  key: \"%s\"  info: \"%s\" \n", x->key, x->info);
		//поиск реально удаляемого - y
		if (x->right == Elist && x->left == Elist) //если лист
			y = x;
		else
			y = findNext(x);
		//находим поддерево удаляемого - p
		if (y->left != Elist)
			p = y->left;
		else
			p = y->right;
		par = y->parent;
		//переопределение род узла для для поддерева p
		if (p != Elist)
			p->parent = par;
		//если у - корень дерева
		if (par == Elist)
			*root = p;
		//переопределение дочернего для par
		else
		{
			if (par->left == y)
				par->left = p;
			else
				par->right = p;
		}
		//копирование данных
		if (y != x)
		{
			x->key = y->key;
			x->info = y->info;
		}
		// p - дочерний реально удаленного 
		// y - удаленный 
		// par - родитель реально удаленного
		while (par != NULL)
		{
			if (par->left == Elist && par->right == Elist)
				par->height = 0;
			else
				par->height = imax2(height(par->left), height(par->right)) + 1;
			
			if (abs(balance(par)) > 1)
			{//выполнить одно из четырёх вращений и, если после вращений 
				//баланс вершины стал равным нулю, то подъём продолжается, 
				//иначе останавливается
				if (strcmp(p->key, par->key) < 0)
				{
					if (balance(par) == -2)
					{
						if (strcmp(p->key, par->left->key) < 0)
							*root = right_rotate(*root);
						else
							*root = leftright_rotate(*root);
					}
				}
				if (strcmp(p->key, par->key) > 0)
				{
					if (balance(par) == 2)
					{
						if (strcmp(p->key, par->right->key) > 0)
							*root = left_rotate(*root);
						else
							*root = rightleft_rotate(*root);
					}
				}
				if (par->left == Elist && par->right == Elist)
					par->height = 0;
				else
					par->height = imax2(height(par->left), height(par->right)) + 1;
				if (balance(par) == 0)
				{
					p = par;
					par = par->parent;
				}
				else
					return 1;
			
			}
			else
				if (abs(balance(par)) == 1)
					return 1;
				else
				{
					p = par;
					par = par->parent;
				}
		}
	}
	return 1;
}

void delTree(Node** root)
{
	if (*root == Elist)
		return;
	delTree(&(*root)->left);
	delTree(&(*root)->right);
	free(*root);
}


int main()
{
	int rc;
	D_Load(&root);
	while (rc = dialog(msgs, NMsgs))
		if (!fptr[rc](&root))
			break;
	delTree(&root);
	printf("\nbb\n");
	return 0;
}


int height(Node *ptr)
{
	return (ptr != NULL) ? ptr->height : -1;
}

int balance(Node *ptr)
{
	return height(ptr->right) - height(ptr->left);
}

void MaxMin(Node *ptr)
{
	if (ptr == Elist)
		return;
	MaxMin(ptr->right);
	printf("key:  \"%s\"  info: \"%s\" \n", ptr->key, ptr->info);
	MaxMin(ptr->left);
}

Node* findMax(Node *root)
{
	Node *ptr;
	ptr = root;
	if (root == Elist)
		return NULL;
	while (ptr->right != Elist)
		ptr = ptr->right;
	return ptr;
}

Node* findMin(Node *root)
{
	Node *ptr;
	ptr = root;
	if (root == Elist)
		return NULL;
	while (ptr->left != Elist)
		ptr = ptr->left;
	return ptr;
}

Node* findNext(Node *x)
{
	Node *ptr;
	if (x->right != Elist)
		ptr = findMin(x->right);
	else
	{
		ptr = x->parent;
		while (ptr != Elist && x == ptr->right)
		{
			x = ptr;
			ptr = x->parent;
		}

	}
	return ptr;
}

void showTree(Node *root, int level)
{
	int i;
	Node *ptr = root;
	if (root)
	{
		showTree(ptr->right, level + 1);
		for (i = 0; i < level + 1; i++)
			printf("      ");
		if (ptr == Elist)
			printf("  \n");
		else
			printf(" %s(%d)\n", ptr->key, ptr->height);
		//printf("%d(%d)\n", ptr->key, ptr->balance);
		showTree(ptr->left, level + 1);
	}
}

Node* checkKey(Node *root, char* k)
{
	Node *ptr;
	ptr = root;
	while (ptr != Elist)
	{
		if (strcmp(ptr->key, k) == 0)
			return ptr;
		if (strcmp(ptr->key, k) > 0)
			ptr = ptr->left;
		else
			ptr = ptr->right;
	}
	return Elist;
}

int imax2(int x, int y)
{
	if (x > y)
		return x;
	else
		return y;
}

Node* left_rotate(Node *root)
{
	Node *right;
	right = root->right;
	root->right = right->left;
	right->left = root;
	if (root->left == Elist && root->right == Elist)
		root->height = 0;
	else
		root->height = imax2(height(root->left), height(root->right)) + 1;
	right->height = imax2(height(right->right), root->height) + 1;
	return right;
}

Node* right_rotate(Node *root)
{
	Node *left;
	left = root->left;
	root->left = left->right;
	left->right = root;
	if (root->left == Elist && root->right == Elist)
		root->height = 0;
	else
		root->height = imax2(height(root->left), height(root->right)) + 1;
	left->height = imax2(height(left->left), root->height) + 1;
	return left;
}

Node* leftright_rotate(Node *root)
{
	root->left = left_rotate(root->left);
	return right_rotate(root);
}

Node* rightleft_rotate(Node *root)
{
	root->right = right_rotate(root->right);
	return left_rotate(root);
}

char* getRandomStr(int len)
{
	char *str = (char*)malloc(len + 1);
	int x, i = 0;
	char a;
	for (i = 0; i < len; i++)
	{
		x = 65 + rand() % 25;
		a = x;
		str[i] = a;
	}
	str[len] = '\0';
	return str;
}

int D_Timing(Node **)
{
	std::ofstream Timing("Timing.txt", std::ios_base::trunc);
	Node *root = Elist, *rc;
	char *info = NULL, *k = NULL, *key[10000];
	int n = 9, cnt = 1000000, i, m;
	clock_t first, last;
	srand(time(NULL));
	Timing << "ItemsCount" << " " << "FindTime" << std::endl;
	while (n--)
	{
		for (i = 0; i < 10000; ++i)
			key[i] = getRandomStr(5);
		for (i = 0; i < cnt; )
		{
			k = getRandomStr(5);
			rc = checkKey(root, k);
			if (rc == Elist)
			{
				root = insertTree(root, k, info, root->parent);
				++i;
			}
		}
		m = 0;
		first = clock();
		for (i = 0; i < 10000; ++i)
			if (checkKey(root, key[i]) != Elist)
				++m;
		last = clock();
		printf("%d items were found\n", m);
		printf("test #%d, number of nodes = %d, timr = %d\n", 9 - n, (9 - n)*cnt, last - first);
		Timing << (9 - n)*cnt << " " << last - first << std::endl;
	}
	Timing.close();
	delTree(&root);
	return 1;
}