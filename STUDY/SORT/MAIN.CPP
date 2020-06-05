#include <stdio.h>
#include <stdlib.h>
#include "bubble.h"
#include "quick.h"
#include "select.h"
#include "rand.h"

int main() {
	int valores[10];
	int i;

	for (i = 0; i < sizeof(valores) / sizeof(int); i++) {
		valores[i] = lrand() % 100;
	}

	printf("Antes\n");
	for (i = 0; i < 10; i++) {
		printf("Valor %i\n", valores[i]);
	}

	//quick_sort(valores, 10);
	//bubble_sort(valores, 10);
  selection_sort(valores, 10);

	printf("\nDepois\n");
	for (i = 0; i < 10; i++) {
		printf("Valor %i\n", valores[i]);
	}

	return 0;
}

