#define MAX_LINE_LENGTH 2000
#define MAX_INPUT_ARGS 100

void parseInput(char* line);

char* firstCommand[MAX_INPUT_ARGS];
char* firstCommandInputToken;
char* firstCommandOutputToken;
char* firstCommandErrorToken;

char* secondCommand[MAX_INPUT_ARGS];
char* secondCommandInputToken;
char* secondCommandOutputToken;
char* secondCommandErrorToken;

int backgrounded;
int piped;