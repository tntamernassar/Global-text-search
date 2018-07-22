#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <fstream>
#include <streambuf>
#include <vector>
#include <filesystem>
#include <Shlobj.h> 

#define MAX_INPUT 2048

typedef struct a{
	
	int score;
	char* file_path;
	 struct a *next;
	 struct a *prev;
}Bolod;

typedef struct LinkedListNode {
	Bolod *bolod;
	struct LinkedListNode *next;
} LinkedListNode;

typedef struct LinkedList {
	int size;
	struct LinkedListNode* head;
} LinkedList;




Bolod* create_bolod(int score, char* path);
char** parse_keywords(char* key_words);
char* read_file(char* file_path);
int is_in(char* wanted, char* text);
void iterate(char* start_path, char ** keys,LinkedList* l);
bool is_dir(char* name);
bool is_editable(char* name);
void add_bolod(int score,char* path,LinkedList* l);
void insert(LinkedList* list, LinkedListNode* toAdd);
LinkedListNode* initLLN(int score, char* path);
LinkedList* initLL();
using namespace std;


void insert(LinkedList* list, LinkedListNode* toAdd) {
	//empty list , 'toAdd' is the first element
	if (list->size == 0) {
		list->head = toAdd;
		list->size++;
		return;
	}
	LinkedListNode* curr = list->head;
	//check if toAdd should be the first element becuase we skip it in the while
	if ((list->head->bolod->score < toAdd->bolod->score)) {
		toAdd->next = list->head;
		list->head = toAdd;
		list->size++;
		return;
	}
	//reach 'toAdd' correct place
	while (curr->next && (curr->next->bolod->score >= toAdd->bolod->score))
		curr = curr->next;
	//not the end of the list
	if (curr->next) {
		toAdd->next = curr->next;
		curr->next = toAdd;
	}
	else {
		//end of the list
		curr->next = toAdd;
	
	}
	list->size++;
}

LinkedListNode* initLLN(int score,char* path) {

	LinkedListNode* l = (LinkedListNode*)malloc(sizeof(LinkedListNode));
	l->bolod = create_bolod(score,path);
	l->next = NULL;
	return l;
}
LinkedList* initLL() {
	LinkedList* l = (LinkedList*)malloc(sizeof(LinkedList));
	l->size = 0;
	return l;
}

int main() {


	char path[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, path) != S_OK)
		printf("error #1");
	
	

	while (1) {
		
		char key_words[MAX_INPUT];
		printf("Please enter keywords , for example : pinapple cats weed :  ");
		fgets(key_words, MAX_INPUT, stdin);
		while (strlen(key_words) == 1) {
			printf("Please enter at least one keyword\n");
			fgets(key_words, MAX_INPUT, stdin);
		}


		char **parsed_key_words = parse_keywords(key_words);	
		LinkedList* l = initLL();
		iterate(path, parsed_key_words, l);
		
		LinkedListNode* head = l->head;
		while (head)
		{
			printf("\nSCORE : %d \nPATH : %s\n",head->bolod->score,head->bolod->file_path);
			printf("\n________________________________________________________________________\n");
			head = head->next;
		}

				
	}
	
}

/*
	@params : 'key_words', string to parce
	@do : parse string 
	@return : parsed string
*/
char** parse_keywords(char* key_words) {

	//result memory allocate
	char **parsed_key_words = (char**)malloc(sizeof(char*) * 10);
	int i = 0;
	int j = 0;
	int c = 0;

	char temp_word[30];

	while (key_words[i] != '\0') {
		
		int _c = key_words[i];
		if (_c == ' ' || _c == '\n') {
			temp_word[c] = '\0';
			parsed_key_words[j] = (char *)malloc(strlen(temp_word));
			strcpy(parsed_key_words[j], temp_word);
		
			j++;
			c = 0;
		}
		else {
			temp_word[c] = _c;
			c++;
		}
		
		i++;
	}

	parsed_key_words[j] = NULL;

	return parsed_key_words;
}

char* read_file(char* file_path) {

	FILE* f = fopen(file_path, "r");

	// Determine file size
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);

	char* where = new char[size];

	rewind(f);
	fread(where, sizeof(char), size, f);
	return where;
	
}




int is_in(char* wanted, char* text) {

	int size = strlen(wanted);
	int times = 0;
	int i = 0;
	while (text[i] != 0) {

		if (text[i] == wanted[0]) {
			int c = 0;
			for (int j = 0; j < size && text[j] != '\0';j++) {
				if (text[i + j] == wanted[j])
					c++;
				else
				{
					i += j - 1;
					break;
				}
			}
			
			if (c == size)
				times++;
		}



		i++;
		
	}

	return times;
}



Bolod* create_bolod(int score, char* path) {
	Bolod* new_bolod = (Bolod*)malloc(sizeof(Bolod));
	new_bolod->file_path = (char *)malloc(strlen(path));
	strcpy(new_bolod->file_path, path);
	new_bolod->score = score;
	new_bolod->next = NULL;
	new_bolod->prev = NULL;
	return new_bolod;
}


void iterate(char* start_path, char ** keys,LinkedList* l) {
	

	std::string pattern(start_path);
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		
		do {
			char* path = (char*)malloc(sizeof(char)*(strlen(start_path) + strlen(data.cFileName) + 1));
			strcpy(path, start_path);
			strcpy(path + strlen(path), "\\");
			strcpy(path + strlen(path), data.cFileName);
			if (is_dir(data.cFileName)) {
		
				iterate(path, keys, l);
			}
			else {

				if (is_editable(data.cFileName))
				{
					char* file_context = read_file(path);
					
					int i = 0;
					int score = 0;
					while (keys[i] != NULL) {
						score += is_in(keys[i], file_context);
						i++;
					}

					if (score) {
						
						
						add_bolod(score, path, l);
					}
				}

			}
			
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
		
	}

	return;
}

bool is_dir(char* name) {

	int i = 0;
	while (name[i] != '\0') {
		if (name[i] == '.')
			return false;
		i++;
	}
	
	return true;
}

bool is_editable(char* name) {

	int len = strlen(name);

	if (len <= 4)
		return false;


	if (name[len - 1] != 't' || name[len - 2] != 'x' || name[len - 3] != 't' || name[len - 4] != '.')
		return false;

	return true;

}

void add_bolod(int score,char* path,LinkedList* l) {
	insert(l,initLLN(score,path));
}

