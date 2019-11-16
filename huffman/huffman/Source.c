#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 
#include<stdio.h>
#include<string.h>
#define MAX_SIZE 256
#define MAX_CODE_SIZE 30

typedef struct Node {
	 int num;
	unsigned char c;
	struct Node* left;
	struct Node* right;
}Node;

typedef struct tree_node {
	Node* edge;
	struct tree_node *prev;
	struct tree_node *next;
}tree_node;


Node* new_node() {
	Node* res = malloc(sizeof(Node));
	res->c = 0;
	res->num = 0;
	res->left = NULL;
	res->right = NULL;
	return res;
}
tree_node* new_tree_node(Node* node) {
	tree_node* res = malloc(sizeof(tree_node));
	res->edge = node;
	res->next = NULL;
	res->prev = NULL;
	return res;
}
void tree_dtor(Node* tree) {
	if (tree != NULL) {
		tree_dtor(tree->left);
		tree_dtor(tree->right);
		free(tree);
	}
}

void delete_node(tree_node** n, tree_node*** text, tree_node** end) {
	tree_node* tmp = *n;
	if ((*n)->prev != NULL) {
		(*n)->prev->next = (*n)->next;
		free((*n)->next);
	}
	if ((*n)->next != NULL) {
		(*n)->next->prev = (*n)->prev;
		free((*n)->prev);
	}
	if ((*n) == (**text)) 	(**text) = (**text)->next;
	if ((*n) == (*end))	(*end) = (*end)->prev;
	free(tmp);
}

void tree_encode(Node* tree, unsigned char* curr_code, int curr_length, unsigned char** codes, int* max_lenght) {
	if (tree == NULL)return;
	if (!tree->left) {
		if ((*max_lenght) < curr_length)(*max_lenght) = curr_length;
		for (int i = 0; i <= (int)strlen(curr_code); i++) {
			codes[tree->c][i] = curr_code[i];
		}
	}
	else {
		curr_code[curr_length] = '0';
		curr_code[curr_length + 1] = '\0';
		tree_encode(tree->left, curr_code, curr_length + 1, codes, max_lenght);
		curr_code[curr_length] = '1';
		curr_code[curr_length + 1] = '\0';
		tree_encode(tree->right, curr_code, curr_length + 1, codes, max_lenght);
	}
}
void swap(int* min2, int* min1, tree_node** n1, tree_node** n2) {
	int c = (*min2);
	(*min2) = (*min1);
	(*min1) = c;
	tree_node* buf = (*n1);
	(*n1) = (*n2);
	(*n2) = buf;
}

tree_node* read_symbols(FILE* in, int* count, int* symb_count, int* flag) {
	tree_node* begin = NULL;
	tree_node* end = begin;
	unsigned char str[MAX_SIZE + 1];
	int size = 0, tmp_size = 0, tmp_c = 0;
	int freq[MAX_SIZE] = { 0 };
	while (!feof(in)) {
		size = fread(str, sizeof(unsigned char), MAX_SIZE, in);
		(*symb_count) += size;
		for (int i = 0; i < size; i++) {
			freq[str[i]]++;
		}
	}
	if ((*symb_count) % 256 == 0) {
		tmp_size = freq[0];
		for (int i = 0; i < MAX_SIZE; i++) {
			if (freq[i] == tmp_size)tmp_c++;
			else break;
		}
	}
	for (int i = 0; i < MAX_SIZE; i++) {
		if (freq[i] != 0) {
			(*count)++;
			Node* node = new_node();
			node->c = i;
			node->num = freq[i];
			tree_node* t_node = new_tree_node(node);
			if (end != NULL) 	end->next = t_node;
			else begin = t_node;
			t_node->prev = end;
			end = t_node;
		}
	}
	if ((*count) == tmp_c)(*flag) = -2;
	fseek(in, 3, SEEK_SET);
	return begin;
}
void build_tree(tree_node** text, int non_empty) {
	int min1 = INT_MAX, min2 = INT_MAX;
	tree_node* n1 = NULL, *n2 = NULL, *end = NULL, *cur_node = NULL;
	if (non_empty == 1) {
		Node* n = new_node();
		n->c = (*text)->edge->c;
		n->num = (*text)->edge->num + 1;
		(*text)->next = new_tree_node(n);
		non_empty++;
	}
	while (non_empty > 1) {
		non_empty--;
		cur_node = *text;
		min1 = INT_MAX, min2 = INT_MAX;
		n1 = NULL, n2 = NULL;
		while (cur_node != NULL) {
			if (cur_node->next == NULL)end = cur_node;
			if (cur_node->edge->num < min2) {
				min2 = cur_node->edge->num;
				n2 = cur_node;
			}
			if (min2 < min1)swap(&min2, &min1, &n1, &n2);
			cur_node = cur_node->next;

		}
		tree_node *n_node = new_tree_node(new_node());
		n_node->edge->num = n1->edge->num + n2->edge->num;
		n_node->edge->left = n1->edge;
		n_node->edge->right = n2->edge;
		delete_node(&n1, &text, &end);
		delete_node(&n2, &text, &end);
		if (end != NULL) {
			end->next = n_node;
			n_node->prev = end;
		}
		else *text = n_node;
	}
}
 void char_to_b(unsigned char a, unsigned char* buff) {
	int tmp = (int)a;
	int num = 128;
	for (int j = 0; j < 8; j++) {
		if (tmp >= num)
		{
			buff[j] = '1';
			tmp -= num;
		}
		else buff[j] = '0';
		num /= 2;
	}
	buff[8] = '\0';
	return;
}
void tree_to_str(Node* tree, char* str, int* i) {
	if (!tree)return;
	char curr_code[9];
	if (!(tree->left) || !(tree->right)) {
		str[(*i)] = '0';
		(*i)++;
		 char_to_b(tree->c, curr_code);
		for (int j = 0; j < 8; j++) {
			str[(*i)] = curr_code[j];
			(*i)++;
		}
	}
	else {
		str[(*i)] = '1';
		(*i)++;
		if (tree->left)tree_to_str(tree->left, str, i);
		if (tree->right)tree_to_str(tree->right, str, i);
	}

}

void print_to_file(unsigned char* str, int len, FILE* out) {
	int st_2[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
	unsigned int tmp = 0, code = 0;
	unsigned char s_code;
	while (len != 0) {
		for (int i = 0; i < 8; i++) {
			code += ((str[tmp + i]) - 48)*st_2[7 - i];
		}
		s_code = code;
		fprintf(out, "%c", s_code);
		code = 0;
		tmp += 8;
		len -= 8;
	}
}

void text_encode(FILE* in, FILE* out, tree_node* tree, int max_l, int len, unsigned char** codes, int char_counter) {
	int size = 0, j = 0, b = 0, zero_counter = 0, curr_index = 0, tmp_a = 1;
	short flag = 0;
	unsigned char* small_tree = malloc((char_counter+1)*(char_counter+1)*8);
	tree_to_str(tree->edge, small_tree, &curr_index);
	if (curr_index % 8 != 0) {
		while ((curr_index % 8) != 0) {
			small_tree[curr_index] = '0';
			curr_index++;
		}
	}
	small_tree[curr_index] = '\0';
	fprintf(out, "%d\x1", char_counter);
	print_to_file(small_tree, curr_index, out);
	if (char_counter == 1) {
		if (tree->edge->left->c == '\0')fprintf(out, "%d", 0);
		else {
			fprintf(out, "%d", len);
			free(small_tree);
			return;
		}
	}
	unsigned char* buff = malloc((max_l+1)*MAX_SIZE);
	unsigned char str[MAX_SIZE + 1],tmp_str[9];
	int l= 0, a=0;
	while (!feof(in)) {
		size = fread(str, sizeof(unsigned char), MAX_SIZE-a, in);
		if (size < MAX_SIZE - a) {
			size -= a;
			flag = 1;
		}
		for (int i = 0; i < a; i++) {
			buff[i] = tmp_str[i];
		}
		b = a;
		for (int i = 0; i < size; i++) {
			while (codes[str[i]][j] != '\0') {
				buff[b] = codes[str[i]][j];
				b++;
				j++;
			}
			j = 0;
		}
		a = b % 8;
		if (flag == 0) {
			if (a != 0) {
				tmp_a = a;
				while (tmp_a != 0) {
					tmp_str[l] = buff[b - tmp_a];
					l++;
					tmp_a--;
				}
				l = 0;
				buff[b - a] = '\0';
			}
				print_to_file(buff, b-a, out);
			
		}
	}

	if (b % 8 != 0) {
		while ((b % 8) != 0) {
			buff[b] = '0';
			zero_counter++;
			b++;
		}
	}
	buff[b] = '\0';
	print_to_file(buff, b, out);
	fprintf(out, "%d", zero_counter);

	free(buff);
	free(small_tree);
	return;
}
void encode_file(FILE* in, FILE* output) {
	fseek(in, 3, 0);
	int count = 0, max_code_l = 0, symb_count = 0, flag = 0;
	tree_node* text = read_symbols(in, &count, &symb_count, &flag);
	if (flag == -2) {
		fprintf(output, "%d ", -2);
		int t_count = 0;
		while (t_count != symb_count) {
			unsigned char txt = fgetc(in);
			fprintf(output, "%c", txt);
			t_count++;
		}
		tree_dtor((text->edge));
		free(text);
		return;
	}
	if (count == 1 && symb_count > 10) {
		int curr_index = 0;
		build_tree(&text, count);
		unsigned char* small_tree = malloc(count*(count + 8)*8);
		small_tree[curr_index] = '\0';
		tree_to_str(text->edge, small_tree, &curr_index);
		if (curr_index % 8 != 0) {
			while ((curr_index % 8) != 0) {
				small_tree[curr_index] = '0';
				curr_index++;
			}
		}
		fprintf(output, "%d\x1", count);
		print_to_file(small_tree, curr_index, output);
		fprintf(output, "%d", symb_count);
		tree_dtor((text->edge));
		free(small_tree);
		free(text);
		return;
	}
	unsigned char** codes = malloc(sizeof(unsigned char*)*MAX_SIZE);
	for (int i = 0; i < MAX_SIZE; i++) {
	codes[i] = (unsigned char*)malloc(MAX_CODE_SIZE);
		for (int j = 0; j < MAX_CODE_SIZE; j++)codes[i][j] = '\0';
	}
	build_tree(&text, count);
	unsigned char* curr_code = malloc(count + 1);
	tree_encode(text->edge, curr_code, 0, codes, &max_code_l);
	fseek(in, 3, SEEK_SET);
	text_encode(in, output, text, max_code_l, symb_count, codes, count);
	free(curr_code);
	for (int i = 0; i < MAX_SIZE; i++)free(codes[i]);
	free(codes);
	tree_dtor((text->edge));
	free(text);
}

void text_decode(unsigned char* buff, int len) {
	buff[len] = '\0';
	unsigned char str[MAX_SIZE+1];
	for (int i = 0; i < len; i++)str[i] = buff[i];
	str[len] = '\0';
	int tmp = 0, code = 0, num = 0, l = 0;
	for (int i = 0; i < len; i++) {
		num = 128;
		tmp = str[i];
		for (int j = 0; j < 8; j++) {
			if (tmp >= num)
			{
				buff[l] = '1';
				tmp -= num;
			}
			else buff[l] = '0';
			num /= 2;
			l++;
		}
	}
	buff[len * 8] = '\0';
}

Node* dec_tree(char* in, int* c, int* i) {
	int st_2[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
	unsigned char tmp;
	int code = 0;
	tmp = in[(*i)];
	Node* tree = new_node();
	if (tmp == '0') {
		(*c)--;
		(*i)++;
		for (int j = 0; j < 8; j++) {
			code += ((in[(*i) + j]) - 48)*st_2[7 - j];
		}
		(*i) += 8;
		tree->c = (unsigned char)code;
	}
	else {
		tree->c = in[(*i)];
		(*i)++;
	}
	if (tmp == '1') {
		if ((*c) != 0) tree->left = dec_tree(in, c, i);
		if ((*c) != 0)	tree->right = dec_tree(in, c, i);
	}
	return tree;
}
void decode_tree_to_str(int count, FILE* in, unsigned char* str) {
	if (count == 1)count++;
	int num, i = 0, tmp, tmp_c = 0, l = 0, n = 0;
	short flag = 0;
	unsigned char c;
	while (count>0) {
		num = 128;
		c = fgetc(in);
		tmp = (int)c;
		for (int j = 0; j < 8; j++) {
			if (tmp >= num)
			{
				str[i] = '1';
				tmp -= num;
				if (flag == 1) {
					tmp_c++;
					if (tmp_c == 9) {
						tmp_c = 0;
						flag = 0;
					}
				}
			}
			else {
				str[i] = '0';
				if (tmp_c == 0) {
					count--;
					flag = 1;
				}
				tmp_c++;
				if (tmp_c == 9) {
					tmp_c = 0;
					flag = 0;
				}
			}
			num /= 2;
			i++;
		}
		if (count == 0 && l == 0) {
			l = 1;
			count++;
		}
		n++;
	}
	str[i] = '\0';
	return;
}

int decode_text(FILE* out, unsigned char* text, Node* tree, char* not_useless) {
	int count = 0;
	int len = strlen(text), i = 0, j=0, l = 0;
	Node* curr_tree = tree;
	unsigned char tmp;
	short flag = 1;
	while (1) {
		if (i >= len) {
			while (i > j) {
				not_useless[l] = text[j + l];
				l++;
				i--;
			}
			not_useless[l] = '\0';
			flag = 0;
			return l;
		}
		tmp = text[i];
		if (tmp == '0') {
				if (curr_tree->left->left) {
					curr_tree = curr_tree->left;
					i++;
				}
			
			else {
				i++;
				fprintf(out, "%c", curr_tree->left->c);
				j = i;
				curr_tree = tree;
				count++;
			}
		}
		else {
			if (curr_tree->right) {
				i++;
				if (curr_tree->right->left)curr_tree = curr_tree->right;
				else {
					fprintf(out, "%c", curr_tree->right->c);
					j = i;
					curr_tree = tree;
					count++;
				}
			}
			else if (count == 0) {
				fprintf(out, "%c", curr_tree->left->c);
				j = i;
				i++;
			}
		}

	}
	return l;
}

void decode(FILE* in, FILE* out) {
	fseek(in, 3, 0);
	int count, tmp_count, zero_text_count, byte_count = 1;
	int max_len, len, curr_len, curr_index = 0, l = 0;
	short flag = 0;
	char tmp, size1[3], size3[2];
	while (flag == 0) {
		tmp = fgetc(in);
		if (tmp == '\x1') flag = 1;
		else {
			size1[l] = tmp;
			l++;
		}
	}
	count = atoi(size1);
	tmp_count = count;
	flag = 0;
	unsigned char* tree_str = malloc((count + 1)*(count + 1) * 9);
	decode_tree_to_str(count, in, tree_str);
	Node* node = dec_tree(tree_str, &tmp_count, &curr_index);
	if (count == 1) {
		fscanf(in, "%d", &len);
		if (len != 0) {
			if (len > 256) {
				unsigned char full_Str[MAX_SIZE + 1];
				for (int i = 0; i <= MAX_SIZE; i++) {
					full_Str[i] = node->left->c;
				}
				full_Str[MAX_SIZE] = '\0';
				while (len > 0) {
					fprintf(out, "%s", full_Str);
					len -= MAX_SIZE;
				}
			}
			else {
				unsigned char* full_Str = malloc(len + 1);
				for (int i = 0; i < len; i++) {
					full_Str[i] = node->left->c;
				}
				full_Str[len] = '\0';
				fprintf(out, "%s", full_Str);
				free(full_Str);
			}
			tree_dtor(node);
			free(tree_str);
			return;
		}
	}
	flag = 0;
	curr_len = ftell(in);
	fseek(in, -1, 2);
		tmp = fgetc(in);
		size3[0] = tmp;
		zero_text_count = atoi(size3);
	len = ftell(in);
	max_len = len - curr_len-1;
	fseek(in, curr_len, 0);
	l = 0;
	unsigned char* text=malloc(MAX_SIZE * 8+1);
	unsigned char not_useless[MAX_SIZE*8+MAX_CODE_SIZE];
	while (max_len > MAX_SIZE-l) {
		byte_count = fread(text, sizeof(unsigned char), MAX_SIZE, in);
		text_decode(text, byte_count);
		if (l > 0) {
			strcat(not_useless, text);
			l = decode_text(out, not_useless, node, not_useless);
		}
		else l = decode_text(out, text, node, not_useless);
		max_len -= byte_count;
		flag = 1;
	}
	if (flag == 1) {
		curr_len = ftell(in);
		fseek(in, -1, 2);
		len = ftell(in);
		max_len = len - curr_len;
		fseek(in, curr_len, 0);
	}
	byte_count = fread(text, sizeof(unsigned char), max_len, in);
	text_decode(text, byte_count);
			if (zero_text_count > 0)text[strlen(text) - zero_text_count] = '\0';
			if (l > 0) {
				strcat(not_useless, text);
				l = decode_text(out, not_useless, node, not_useless);
			}
			else l = decode_text(out, text, node, not_useless);
	free(text);
	free(tree_str);
	tree_dtor(node);
	return;
}
void print_full(FILE* in, FILE* out) {
	fseek(in, 6, 0);
	unsigned char buff='1';
	unsigned char prev_buff = '0';
	while(!feof(in)){
		buff=fgetc(in);
		if (prev_buff != buff) {
			prev_buff = buff;
			fprintf(out, "%c", buff);
		}
	} 
	return;
}

int main() {
	//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	FILE* in = fopen("in.txt", "rb");
	FILE* out = fopen("out.txt", "wb");
	unsigned char task = fgetc(in);
	int flag;
	fseek(in, 0, 2);
	if (task == 'c') {
		if (ftell(in) == 3) {
			fprintf(out, "%d", -1);
			return 0;
		}
		else encode_file(in, out);
	}
	else {
		fseek(in, 3, 0);
		fscanf(in, "%d", &flag);
		if (flag == -1)return 0;
		if (flag == -2)print_full(in, out);
		else decode(in, out);
	}
	
	fclose(in);
	fclose(out);
	return 0;
}