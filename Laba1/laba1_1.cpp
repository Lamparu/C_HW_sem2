#include "pch.h"
#include <stdio.h>

int input(int k)
{
	int rc;
	do
	{
		rc = scanf_s("%d", &k);
		if (rc <= 0 || k < 0)
		{
			printf("Error\n");
			scanf_s("%*[^\n]");
		}
	} while (rc <= 0 || k < 0);
	return k;
}

void inputseq(int *n, int arr[], int m)
{
	int rc, i;
	do
	{
		printf("How many elments? ");
		rc = scanf_s("%d", n);
		if (rc <= 0 || *n <= 0 || *n > m)
		{
			printf("Error\n");
			scanf_s("%*[^\n]");
		}
	} while (rc <= 0 || *n <= 0 || *n > m);
	for (i = 0; i < *n; i++)
		arr[i] = input(arr[i]);
	for (i = 0; i < *n; i++)
		printf("a[%d] = %d\n", i, arr[i]);
}

void funk(int *mass, int *mass1, int *mass2, int *n) //mass1 - исходный, mass2 - кол-во цифр
{
	int i, r = 0, k;
	for (i = 0; i < *n; i++)
	{
		int ten = 1, q = 0, b = 0;
		mass1[i] = mass[i];
		while (mass1[i] / ten != 0)
		{
			r = 9 - (mass1[i] % (ten * 10)) / ten;
			b = b + r * ten;
			ten = ten * 10;
		}
		mass1[i] = b;
		while (b != 0)
		{
			b = b / 10;
			q++;
		}
		mass2[i] = q;
	}
}

void sort(int *mass1, int *mass2, int *n)
{
	int fse, fsq, i = 0, j = 0;
	while (i <= *n)
	{
		while (j < *n)
		{
			if (mass1[j] < mass1[i])
			{
				fse = mass1[j];
				fsq = mass2[j];
				mass1[j] = mass1[i];
				mass2[j] = mass2[i];
				mass1[i] = fse;
				mass2[i] = fsq;
			}
			j++;
		}
		i++;
		j = i;
	}
}

int main()
{
	int p, m, i, *n;
	int mass[100], mass1[100], mass2[100];
	inputseq(&m, mass, 100);
	funk(&mass[0], &mass1[0], &mass2[0], *n);
	sort(&mass1[0], &mass2[0], *n);
	printf("Sorted array:\n");
	for (i = 0; i < *n; i++)
		printf("%d - %d\n", mass1[i], mass2[i]);

}