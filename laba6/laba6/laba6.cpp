//неориентированный 
//списки смежности
//BFS - поиск вершин, связанных с заданной

//вид в файле
// x y
// name1 name2
// ...
// name1 name2


#include "pch.h"
#include <iostream>
#include <time.h>
#include <fstream>
#pragma warning(disable:4996)


struct Node {
	int x;
	int y;
	char* name;
	int vis;
	struct Edge *link;
};

struct Edge {
	int n;
	struct Edge *next;
};

struct Graph {
	int n;
	struct Node *elem;
};

const char *msgs[] = { "0. Quit", "1. Load from file", "2. Random graph", "3. Add element", "4. Add edge","5. Find all connected elements", "6. Delete", "7. Show", "8. Timing" };
const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

const char *errmsgs[] = { "Ok", "Name exists", "Coordinates exist", "Element 1 doesn't exist", "Element 2 doesn't exist", "Name doesn't exist", "Edge exists" };

int D_Add_Node(Graph*),
D_Add_Edge(Graph*),
D_Find(Graph*),
D_Delete(Graph*),
D_Show(Graph*),
D_Save(Graph*),
D_Timing(Graph*),
D_Load(Graph*),
D_Random(Graph*);
int(*fptr[])(Graph*) = { NULL, D_Load, D_Random, D_Add_Node, D_Add_Edge,  D_Find, D_Delete, D_Show, D_Timing };
int dialog(const char *msgs[], int);
int getInt(int*);
char *getStr();

int insertNode(Graph*, int, int, char*);
int checkName(Graph*, char*);
int checkCoord(Graph*, int, int);
int insertEdge(Graph*, char*, char*);
void NodeToNode(Graph*, int, int);
int deleteNode(Graph*, char*);
void delfromTable(Graph*, int);
int delEdges(Graph*, int);
int checkEdge(Graph*, int, int);
void fixupEdges(Graph*, int);
int findAll(Graph*, char*, int);
int BFS(Graph*, int, int);
void delGraph(Graph*);
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
	return 1;
}

int dialog(const char *msgs[], int N) 
{
	const char *errmsg = "";
	int i, n = 0;
	do {
		puts(errmsg);
		errmsg = "U r wrong. Repeat, pls!";
		for (i = 0; i < N; ++i)
			puts(msgs[i]);
		puts("Make ur choice: ");
		getInt(&n);
	} while (n < 0 || n >= N);
	return n;
}

int D_Add_Node(Graph* graph)
{
	int x, y, rc;
	char *name = NULL;
	printf("Enter name --> ");
	do name = getStr();
	while (name == NULL);
	printf("Enter x --> ");
	getInt(&x);
	printf("Enter y --> ");
	getInt(&y);
	rc = insertNode(graph, x, y, name);
	printf("%s: [%d, %d]  \"%s\" \n", errmsgs[rc], x, y, name);
	free(name);
	return 1;
}

int insertNode(Graph *graph, int x, int y, char *name)
{
	int rc;
	//проверка на имя - 1
	if (graph->elem)
	{
		rc = checkName(graph, name);
		if (rc > -1)
			return 1;
		rc = checkCoord(graph, x, y);
		if (rc > -1)
			return 2;
	}
	//проверка на координаты - 2
	if (graph->elem)
		graph->elem = (Node*)realloc(graph->elem, (++graph->n) * sizeof(Node));
	else
	{
		graph->elem = (Node*)malloc(sizeof(Node));
		++graph->n;
	}
	graph->elem[graph->n - 1].x = x;
	graph->elem[graph->n - 1].y = y;
	graph->elem[graph->n - 1].vis= 0;
	graph->elem[graph->n - 1].name = _strdup(name);
	graph->elem[graph->n - 1].link = NULL;

	return 0;
}

int checkName(Graph *graph, char *name)
{
	int i;
	for (i = 0; i < graph->n; i++)
		if (strcmp(graph->elem[i].name, name) == 0)
			return i;
	return -1;
}

int checkCoord(Graph *graph, int x, int y)
{
	int i;
	for (i = 0; i < graph->n; i++)
		if (graph->elem[i].x == x && graph->elem[i].y == y)
			return i;
	return -1;
}

int D_Add_Edge(Graph *graph)
{
	char *name1 = NULL, *name2 = NULL;
	int rc;
	printf("Enter name of first node--> ");
	do name1 = getStr();
	while (name1 == NULL);
	printf("Enter name of second node--> ");
	do name2 = getStr();
	while (name2 == NULL);	
	rc = insertEdge(graph, name1, name2);
	printf("%s:  \"%s\" <--> \"%s\" \n", errmsgs[rc], name1, name2);
	free(name1);
	free(name2);
	return 1;
}

int insertEdge(Graph *graph, char* x, char* y)
{
	int i, k, rc;
	i = checkName(graph, x);
	if (i == -1)
		return 3;
	k = checkName(graph, y);
	if (k == -1)
		return 4;
	rc = checkEdge(graph, i, k);
	if (rc == 1)
		return 6;
	if (i != k)
		NodeToNode(graph, i, k);
	NodeToNode(graph, k, i);
	return 0;
}

int checkEdge(Graph* graph, int i, int k)
{
	Edge *e;
	e = graph->elem[i].link;
	while (e != NULL)
	{
		if (e->n == k)
			return 1;
		e = e->next;
	}
	return 0;
}

void NodeToNode(Graph *graph, int x, int y)
{
	Edge *e;
	e = (Edge*)malloc(sizeof(Edge));
	e->n = y;
	e->next = graph->elem[x].link;
	graph->elem[x].link = e;
}

int D_Show(Graph *graph)
{
	int i;
	Edge *e;
	printf("Nodes: \n");
	for (i = 0; i < graph->n; i++)
		printf("[%d, %d]  \"%s\"\n", graph->elem[i].x, graph->elem[i].y, graph->elem[i].name);
	printf("\n");
	printf("Edges: \n");
	for (i = 0; i < graph->n; i++)
	{
		e = graph->elem[i].link;
		while (e != NULL)
		{
			printf("\"%s\" <--> \"%s\"\n", graph->elem[i].name, graph->elem[e->n].name);

			//printf("[%d, %d] <--> [%d, %d]\n", graph->elem[i].x, graph->elem[i].y, graph->elem[e->n].x, graph->elem[e->n].y);
			e = e->next;
		}
	}
	return 1;

}

int D_Delete(Graph *graph)
{
	int rc;
	char *name = NULL;
	printf("Enter name to delete --> ");
	do name = getStr();
	while (name == NULL);
	rc = deleteNode(graph, name);
	printf("%s:  \"%s\" \n", errmsgs[rc], name);
	free(name);
	return 1;
}

int deleteNode(Graph *graph, char* name)
{
	int rc;
	rc = checkName(graph, name);
	if (rc == -1)
		return 5;
	printf("Element found: [%d, %d] \"%s\"\n", graph->elem[rc].x, graph->elem[rc].y, graph->elem[rc].name);
	delEdges(graph, rc);
	delfromTable(graph, rc); 
	fixupEdges(graph, rc);
	return 0;
}

void fixupEdges(Graph *graph, int i)
{
	int k;
	Edge *e;
	for (k = 0; k < graph->n; k++)
	{
		e = graph->elem[k].link;
		while (e != NULL)
		{
			if (e->n == graph->n)
				e->n = i;
			e = e->next;
		}
	}
}

void delfromTable(Graph *graph, int i)
{
	free(graph->elem[i].link);
	graph->elem[i] = graph->elem[graph->n - 1];
	graph->elem = (Node*)realloc(graph->elem, (--graph->n) * sizeof(Node));
}

int delEdges(Graph *graph, int i)
{
	int k;
	Edge *e, *tmp, *del;
	for (k = 0; k < graph->n; k++)
	{
		e = graph->elem[k].link;
		if (e != NULL)
		{
			if (e->n == i) //если 1ый в списке
			{
				tmp = e;
				graph->elem[k].link = e->next;
				free(tmp);
			}
			else
			{
				while (e->next != NULL)//доходит до последнего?
				{
					tmp = e->next;
					if (tmp->n == i)
					{
						e->next = tmp->next;
						del = tmp;
						e = e->next;
						free(del);
						break;
					}
					e = e->next;
				}
			}
		}
	}
	return 1;
}

int D_Find(Graph *graph)
{
	char *name = NULL;
	int rc;
	printf("Enter name of element --> ");
	do name = getStr();
	while (name == NULL);
	rc = findAll(graph, name, 1);
	printf("%s: \"%s\" \n", errmsgs[rc], name);
	free(name);
	return 1;
}

int findAll(Graph* graph, char* name, int flag) //в ширину
{
	int rc;
	rc = checkName(graph, name);
	if (rc == -1)
		return 5;
	BFS(graph, rc, flag);
	return 0;
}

int BFS(Graph *graph, int x, int flag)
{
	int *q;
	int cur, i;
	int count = 0, head = 0;
	Edge *e;
	q = (int*)malloc(graph->n*sizeof(int));
	for (i = 0; i < graph->n; i++)
		graph->elem[i].vis = 0;
	graph->elem[x].vis = 1;
	q[count++] = x;
	while (head < count)
	{
		cur = q[head++];
		if (flag == 1)
			printf("[%d, %d] \"%s\" \n", graph->elem[cur].x, graph->elem[cur].y, graph->elem[cur].name);
		e = graph->elem[cur].link;
		while (e != NULL)
		{
			if (graph->elem[e->n].vis == 0)
			{
				graph->elem[e->n].vis = 1;
				q[count++] = e->n;
			}
			graph->elem[e->n].vis = 2;
			e = e->next;
		}
	}
	//free(q);
	return 1;
}

int D_Random(Graph* graph)
{

	if (graph->elem)
		delGraph(graph);
	*graph = { 0, NULL };
	char *name = NULL;
	int i, k, sz, x, y;
	printf("Enter amount of elements to random -->\n");
	getInt(&sz);
	for (i = 0; i < sz; i++)
	{
		name = getRandomStr(5);
		x = rand()*rand()%100;
		y = rand()*rand()%100;
		insertNode(graph, x, y, name);
	}
	for (i = 0; i < sz; i++)
	{
		for (k = 0; k < sz; k++)
		{
			if (rand() % 5 == 1)
				insertEdge(graph, graph->elem[i].name, graph->elem[k].name);
		}
	}
	printf("Random graph created\n");

	return 1;
}

int main()
{
	Graph graph = {0, NULL};
	int rc;
	while (rc = dialog(msgs, NMsgs))
		if (!fptr[rc](&graph))
			break;
	printf("\nbb\n");
	D_Save(&graph);
	delGraph(&graph);
	return 0;
}

void delGraph(Graph *graph)
{
	int i;
	Edge *e, *ptr;
	for (i = 0; i < graph->n; i++)
	{
		e = graph->elem[i].link;
		while (e != NULL)
		{
			ptr = e;
			e = e->next;
			free(ptr);
		}
	}
	free(graph->elem);
	//free(graph);
}

int D_Load(Graph* graph)
{
	FILE *file;
	int x, y, rc;
	char *name = NULL, *name1 = NULL, *name2 = NULL;
	name = (char*)malloc(sizeof(char));
	name1 = (char*)malloc(sizeof(char));
	name2 = (char*)malloc(sizeof(char));
	if (fopen_s(&file, "data.txt", "r") == -1)
	{
		printf("File doesn't exist\n");
		return 1;
	}
	
	if (fscanf(file, "%d", &x) == -1)
	{
		printf("File is empty\n");
		fclose(file);
		return 1;
	}
	fseek(file, 0, SEEK_SET);
	//printf("Data from file: \n");
	while (fscanf(file, "%d",&x) > 0)
	{
		fscanf(file, "*c");
		fscanf(file, "%d", &y);
		fscanf(file, "*c");
		fscanf(file, "%s", name);
		rc = insertNode(graph, x, y, name);
		//printf("%s: [%d, %d]  \"%s\" \n", errmsgs[rc], x, y, name);
	}
	//fscanf(file, "%c");
	while (fscanf(file, "%s", name1) != -1)
	{
		fscanf(file, "*c");
		fscanf(file, "%s", name2);
		fscanf(file, "*c");
		rc = insertEdge(graph, name1, name2);
		//printf("%s:  \"%s\" <--> \"%s\" \n", errmsgs[rc], name1, name2);
	}
	printf("\nGraph from data file: \n\n");
	D_Show(graph);
	fclose(file);
	return 1;
}

int D_Save(Graph *graph)
{
	FILE *file;
	int i;
	Edge *e;
	fopen_s(&file, "data.txt", "w");
	fseek(file, 0, SEEK_SET);
	for (i = 0; i < graph->n; i++)
	{
		fprintf(file, "%d", graph->elem[i].x);
		fputc(' ', file);
		fprintf(file, "%d", graph->elem[i].y);
		fprintf(file, "\n");
		fprintf(file, "%s", graph->elem[i].name);
		fprintf(file, "\n");
	}
	for (i = 0; i < graph->n; i++)
	{
		e = graph->elem[i].link;
		while (e != NULL)
		{
			fprintf(file, "%s", graph->elem[i].name);
			fputc(' ', file);
			fprintf(file, "%s", graph->elem[e->n].name);
			fprintf(file, "\n");
			e = e->next;
		}
	}
	fclose(file);
	return 1;
}

int D_Timing(Graph*)
{
	std::ofstream Timing("Timing.txt", std::ios_base::trunc);
	Graph graph = { 0, NULL };
	char *name = NULL, *nameq[10000];
	int n = 10, cnt = 500, x, y, i, k, m, rc;
	int str = 3;
	clock_t first, last;
	srand(time(NULL));
	Timing << "ItemsCount" << " " << "FindTime" << std::endl;
	while (n--)
	{
		for (i = 0; i < 10000; i++)
			nameq[i] = getRandomStr(str);
		for (i = 0; i < cnt; )
		{
			x = rand();//*rand();
			y = rand();//*rand();
			name = getRandomStr(str);
			rc = insertNode(&graph, x, y, name);
			if (rc == 0)
				++i;
		}
		m = 0;
		for (i = 0; i < graph.n; i++)
		{
			for (k = 0; k < graph.n; k++)
			{
				if (rand() % 4 == 1)
					insertEdge(&graph, graph.elem[i].name, graph.elem[k].name);
			}
		}

		first = clock();
		for (i = 0; i < 10000; ++i)
		{
			//rc = rand() % graph.n;
			//name = graph.elem[rc].name;
			//name = getRandomStr(str);
			if (findAll(&graph, nameq[i], 0) != 5)
				++m;
		}
		last = clock();
		printf("%d items were found\n", m);
		printf("test #%d, number of nodes = %d, timr = %d\n", 10 - n, (10 - n)*cnt, last - first);
		Timing << (10 - n)*cnt << " " << last - first << std::endl;
	}
	Timing.close();
	delGraph(&graph);
	return 1;
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