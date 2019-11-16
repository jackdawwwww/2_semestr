#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC 
#include<stdlib.h>
#include<crtdbg.h> 
#include<stdio.h>
struct node 
{
	int key;
	unsigned char height;
	struct node* left;
	struct node* right;
};
typedef struct node* pnode;

pnode balance(pnode tree) {
	return tree;
}
pnode create_node(pnode tree, int top) {
	return tree;
}
pnode insert(pnode tree, int top) 
{
	if (!tree) return create_node(tree,top);
	if (top < tree->key)
		tree->left = insert(tree->left, top);
	else
		tree->right = insert(tree->right, top);
	return balance(tree);
}

pnode read_tops(FILE* in, int N) {
	pnode tree = NULL;
	int top;
	for(int i=0;i<N; i++){
		fscanf(in, "%d", &top);
		tree = insert(tree, top);
	}
	return tree;
}

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	FILE* in = fopen("in.txt", "r");
	int N;
	fscanf(in, "%d", &N);
	if (N == 0) {
		printf("0");
		return 0;
	}
	pnode tree = read_tops(in, N);
	printf("%d",tree->height);
	fclose(in);
	_CrtDumpMemoryLeaks();
	return 0;
}