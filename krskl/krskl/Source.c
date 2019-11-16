#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC 
#include<stdlib.h>
#include<crtdbg.h> 
#include <stdio.h>
#include <malloc.h>
#include <limits.h>


typedef struct TEdge
{
	int v1, v2;
	int length;
} Edge;

int cmp(const void *a, const void *b) {
	Edge *c = (Edge*)a, *d = (Edge*)b;
	return c->length - d->length;
}


void kruskal(Edge* edges, int top_number, int r_number) {
	int* groups = calloc(top_number + 1, sizeof(int));
	int group_num = 0;
	Edge* result = malloc(sizeof(Edge)*(top_number - 1));
	int result_num = 0;

	for (int i = 0; i < r_number; i++) {
		int peak1 = groups[edges[i].v1];
		int peak2 = groups[edges[i].v2];
		if (peak1 != 0 && peak2 != 0 && peak1 == peak2) {
			continue;
		}

		if (peak1 == 0 && peak2 == 0) {
			group_num++;
			groups[edges[i].v1] = group_num;
			groups[edges[i].v2] = group_num;
		}
		if (peak1 != peak2) {
			if (peak1 == 0) {
				groups[edges[i].v1] = peak2;
			}
			else if (peak2 == 0) {
				groups[edges[i].v2] = peak1;
			}
			else {
				int _min = min(peak1, peak2);
				int _max = max(peak1, peak2);
				for (int j = 0; j <= top_number; j++) {
					if (groups[j] == _max) {
						groups[j] = _min;
					}
				}
			}
		}
		result[result_num] = edges[i];
		result_num++;
	}

	int counter = 0;
	for (int i = 0; i < top_number; i++) {
		if (groups[i] == 1) {
			counter++;
		}

	}
	if (counter != top_number - 1 || (result_num < top_number - 1)) {
		printf("no spanning tree");

	}
	else {
		
		for (int i = 0; i < top_number - 1; i++) {
			printf("%d %d \n", min(result[i].v1, result[i].v2), max(result[i].v1, result[i].v2));
		}
	}
	free(groups);
	free(result);
	return;
}

int errors(int top_number, int r_number) {
	if (top_number < 0 || top_number> 5000) {
		printf("bad number of vertices");
		return 0;
	}
	if (r_number<0 || r_number>(top_number*(top_number + 1) / 2)) {
		printf("bad number of edges");
		return 0;
	}
	return 1;
}

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	FILE *in;
	in = fopen("in.txt", "r");
	int top_number, r_number;
	
	if (fscanf(in, "%d", &top_number) == EOF) {
		printf("bad number of lines");
		return 0;
	}
	if (fscanf(in, "%d", &r_number) == EOF) {
		printf("bad number of lines");
		return 0;
	}
	if (errors(top_number, r_number) == 1) {
		Edge* edges = malloc(sizeof(Edge)*r_number);
		if (r_number < (top_number-1)) {
			printf("no spanning tree");
		}
		for (int i = 0; i < r_number; i++) {
			int start, fin;
			long val;
			if (fscanf(in, "%d %d %ld", &start, &fin, &val) == EOF) {
				printf("bad number of lines");
				return 0;
			}
			if (start < 1 || start>top_number || fin < 1 || fin>top_number) {
				printf("bad vertex");
				return 0;
			}
			if (val<0 || val>INT_MAX) {
				printf("bad length");
				return 0;
			}
			edges[i].v1 = start;
			edges[i].v2 = fin;
			edges[i].length = val;
			if (start == fin) {
				r_number--;
				i--;
			}
		}
		qsort(edges, r_number, sizeof(Edge), cmp);
		kruskal(edges, top_number, r_number);
		free(edges);

	}
	_CrtDumpMemoryLeaks();
	return 0;
}
