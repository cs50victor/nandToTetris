/*
Files in C
Victor Atasie
CS 325
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>


//--------------------------
//  Symbol Table Linked List
struct Node{
    char* symbol;
    char* memLocation;
    struct Node* next;
};

struct LinkedList{
    struct Node* head;
};

void printList(struct LinkedList* list);
void insertAtEnd(struct LinkedList* list,char* position, char* word);
void deleteList(struct LinkedList* list);
char* searchNode(struct LinkedList* list, char* word);

//--------------------------
void firstPass(char* fileName, struct LinkedList* list);
//---------------------------
char* removeSpacesAndComment(char*);
int stringToInt(char*);
char* intToBinary(int);
char* breakBefore(char[], char);
char* breakAfter(char[], char);
char* jumpOrDest(char []);
char aBit(char []);
char* compTrans(char []);
char* destTrans(char []);
char* jumpTrans(char []);
char* predefinedSymbolChecker(char []);
char* labelsChecker(struct LinkedList* list,char []);

//everything finally goes through here
void fileCopyingandPasting(struct LinkedList* list);



int main()
{

    struct LinkedList symbolTable;
    symbolTable.head = NULL;
    fileCopyingandPasting(&symbolTable);
    deleteList(&symbolTable);
    return 0;
}


void firstPass(char* fileName, struct LinkedList* list){
    FILE* inputFile = NULL;
    const int bufferSize = 256;

    inputFile = fopen(fileName, "r");

    if(inputFile == NULL){
        puts("Error opening input file");
    }
    else{

        //Create a buffer to read into
        char buffer[bufferSize];
        int i = 0;

        // note-to-self : don't free memory while a process/ loop is running (slow+errors)
        char *line = (char*)malloc(sizeof(char) * bufferSize);

        int cursor = -1;

        //Use fgets to read from the file, null when end of file
        while(fgets(buffer, bufferSize, inputFile) != NULL){
            buffer[strcspn(buffer, "\r\n")] = 0;

            line = removeSpacesAndComment(buffer);

            if(line[i] == '@' ||
                    line[i] == 'M' ||
                    line[i] == 'A'||
                    line[i] == 'D'||
                    line[i] == '0'){
                cursor++;
            }
            if(line[i] == '('){
                char* newLine = breakBefore(line, ')')+1;
                char* stringCursor=(char*)malloc(10*sizeof(char));
                char stringCursor2[20];

                sprintf(stringCursor2, "%d", cursor+1);
                strcpy(stringCursor, stringCursor2);
                insertAtEnd(list, stringCursor, newLine);
                free(stringCursor);
            }

        }
        //Close the file
        fclose(inputFile);
    }
}

void fileCopyingandPasting(struct LinkedList* list){
    //Files are declared using file pointers
    FILE* inputFile = NULL;
    FILE* outputFile = NULL;
    //File names are C-strings
    //const fixes whining about variable length array
    const int bufferSize = 256;
    //256 is an arbitrary value
    char inputFileName[bufferSize];

    //Get file name from keyboard
    printf("Please enter an input filename: ");
    fgets(inputFileName, bufferSize, stdin);
     //removes the \n
    inputFileName[strcspn(inputFileName, "\n")] = 0;

    firstPass(inputFileName, list);
    printList(list);

    //Open files with fopen
     //read only
    inputFile = fopen(inputFileName, "r");
    //write only
    outputFile = fopen("out.txt", "w");

    //Note that you could avoid the extra NULL lines by doing
    //FILE* inputFile = fopen(inputFileName, "r");
    //But I like the NULL declaration.
    //If the file pointers are null, the file didn't open
    if(inputFile == NULL){
        puts("Error opening input file");
    }
    //Don't proceed unless you know the files are open
    else if (outputFile != NULL){

        //Create a buffer to read into
        char buffer[bufferSize];
        int i = 0;

        // note-to-self : don't free memory while a process/ loop is running (slow+errors)
        char *line = (char*)malloc(sizeof(char) * bufferSize);
        char *cBinary = (char*)malloc(17*sizeof(char));
        char* binary = NULL;
        char* jumpText= NULL;
        char* jumpBinary= NULL;
        char* compText= NULL;
        char* compBinary= NULL;
        char* destText2= NULL;
        char* compText2= NULL;
        char* destBinary2= NULL;
        char* compBinary2= NULL;
        char* defaultSymbolCheck=NULL;
        char* labelCheck=NULL;
        int currentVariablePosition = 16;

        //Use fgets to read from the file, null when end of file
        while(fgets(buffer, bufferSize, inputFile) != NULL){
            buffer[strcspn(buffer, "\r\n")] = 0;

            line = removeSpacesAndComment(buffer);

            if(line[i] == '/'){
                puts("Comment");
            }
            else if (line[i] == '@'){
                char* newLine = line + 1;
                int num = -1;
                char* checksComplete = "";

                if(isdigit(newLine[0])==0){
                    if(newLine[0] >= 'A' && newLine[0] <= 'Z'){
                        defaultSymbolCheck = predefinedSymbolChecker(newLine);
                        labelCheck = labelsChecker(list,defaultSymbolCheck);
                        checksComplete = labelCheck;
                    }
                    else if(newLine[0] >= 'a' && newLine[0] <= 'z'){
                        char* variablePosition=(char*)malloc(10*sizeof(char));
                        char stringVariablePosition[10];


                        sprintf(stringVariablePosition, "%d", currentVariablePosition);
                        strcpy(variablePosition, stringVariablePosition);
                        insertAtEnd(list, variablePosition, newLine);

                        checksComplete= searchNode(list, newLine);
                        assert(strcmp(checksComplete,"none") != 0);
                        currentVariablePosition++;
                        free(variablePosition);
                    }

                    assert(strcmp(checksComplete,"") != 0);
                    num = stringToInt(checksComplete);
                }
                else{
                    num = stringToInt(newLine);
                }

                assert(num != -1);
                binary = intToBinary(num);
                printf("%s\n", binary);
                //send the line to the file
                fputs(binary,outputFile);
                //add a new line
                fputs("\n",outputFile);
            }
            else if(line[i] == 'A' ||
                    line[i] == 'M' ||
                    line[i] == 'D' ||
                    line[i] == '0' ||
                    line[i] == '1' ||
                    line[i] == '-' ||
                    line[i] == '!'
                    ){
                cBinary[16] = '\0';
                cBinary[15] = '1';
                cBinary[14] = '1';
                cBinary[13] = '1';

                char compare[5];
                //compare copies the returnes Jump/Dest Value
                strcpy(compare, jumpOrDest(line));

                if (strcmp(compare,"Jump\0") == 0){

                    //char* jumpText = breakAfter(line, ';');
                    jumpText = breakAfter(line, ';');
                    //char* jumpBinary = jumpTrans(jumpText);
                    jumpBinary = jumpTrans(jumpText);

                    //copy to cBinary
                    int counterJ = 0;
                    for (int position = 2; position>-1; position--){
                        cBinary[position] = jumpBinary[counterJ];
                        counterJ++;
                    }

                    //char* compText = breakBefore(line, ';');
                    compText = breakBefore(line, ';');
                    //char* compBinary = compTrans(compText);
                    compBinary = compTrans(compText);

                    int counterC = 0;
                    for (int position = 11; position>5; position--){
                        cBinary[position] = compBinary[counterC];
                        counterC++;
                    }

                    cBinary[12] = aBit(compText);
                    cBinary[5] = '0';
                    cBinary[4] = '0';
                    cBinary[3] = '0';
                    // -------default set for all Jump ------------

                    // reverse binary String
                    strrev(cBinary);
                    puts(cBinary);
                    fputs(cBinary,outputFile); //send the line to the file
                    fputs("\n",outputFile); //add a new line

                }
                else if (strcmp(compare,"Dest\0") == 0){

                    //char* destText2 = breakBefore(line, '=');
                    destText2 = breakBefore(line, '=');
                    //char* compText2 = breakAfter(line, '=');
                    compText2 = breakAfter(line, '=');

                    //char* destBinary2 = destTrans(destText2);
                    destBinary2 = destTrans(destText2);
                    //char* compBinary2 = compTrans(compText2);
                    compBinary2 = compTrans(compText2);



                    //copy to cBinary
                    int counterC2 = 0;
                    for (int position = 11; position>5; position--){
                        cBinary[position] = compBinary2[counterC2];
                        counterC2++;
                    }

                    int counterD2 = 0;
                    for (int position = 5; position>2; position--){
                        cBinary[position] = destBinary2[counterD2];
                        counterD2++;
                    }

                    cBinary[12] = aBit(compText2);
                    cBinary[2] = '0';
                    cBinary[1] = '0';
                    cBinary[0] = '0';

                    //reverse, output binary and write to file
                    strrev(cBinary);
                    puts(cBinary);
                    fputs(cBinary,outputFile); //send the line to the file
                    fputs("\n",outputFile); //add a new line

                    //free(destText2);
                    //free(compText2);
                    //free(destBinary2);
                    //free(compBinary2);

                }
                else{
                    puts("Error with C instruction comparison");
                    puts(compare);
                }
            }
            else{
                puts("ERROR");
            }
        }
        //Close the file
        fclose(inputFile);
        fclose(outputFile);

        //always free mallocs LAST
        free(line);
        free(cBinary);
        free(defaultSymbolCheck);
        free(binary);
        free(jumpText);
        free(jumpBinary);
        free(compText);
        free(compBinary);
        free(destText2);
        free(compText2);
        free(destBinary2);
        free(compBinary2);
    }
}

char* predefinedSymbolChecker(char string[]){
    char* decimal = (char*)malloc(7*sizeof(char));

    if (strcmp(string, "SCREEN\0") == 0){
        strcpy(decimal, "16384");
    }
    else if (strcmp(string, "KBD\0") == 0){
        strcpy(decimal, "24576");
    }
    else if (strcmp(string, "SP\0") == 0 || strcmp(string, "R0\0") == 0){
        strcpy(decimal, "0");
    }
    else if (strcmp(string, "LCL\0") == 0 || strcmp(string, "R1\0") == 0){
        strcpy(decimal, "1");
    }
    else if (strcmp(string, "ARG\0") == 0 || strcmp(string, "R2\0") == 0){
        strcpy(decimal, "2");
    }
    else if (strcmp(string, "THIS\0") == 0 || strcmp(string, "R3\0") == 0){
        strcpy(decimal, "3");
    }
    else if (strcmp(string, "THAT\0") == 0 || strcmp(string, "R4\0") == 0){
        strcpy(decimal, "4");
    }
    else if (strcmp(string, "R5\0") == 0){
        strcpy(decimal, "5");
    }
    else if (strcmp(string, "R6\0") == 0){
        strcpy(decimal, "6");
    }
    else if (strcmp(string, "R7\0") == 0){
        strcpy(decimal, "7");
    }
    else if (strcmp(string, "R8\0") == 0){
        strcpy(decimal, "8");
    }
    else if (strcmp(string, "R9\0") == 0){
        strcpy(decimal, "9");
    }
    else if (strcmp(string, "R10\0") == 0){
        strcpy(decimal, "10");
    }
    else if (strcmp(string, "R11\0") == 0){
        strcpy(decimal, "11");
    }
    else if (strcmp(string, "R12\0") == 0){
        strcpy(decimal, "12");
    }
    else if (strcmp(string, "R13\0") == 0){
        strcpy(decimal, "13");
    }
    else if (strcmp(string, "R14\0") == 0){
        strcpy(decimal, "14");
    }
    else if (strcmp(string, "R15\0") == 0){
        strcpy(decimal, "15");
    }
    else{
        strcpy(decimal, string);
    }

    assert(decimal != NULL);
    return decimal;
}

char* labelsChecker(struct LinkedList* list,char string[]){
    char* labelTranslation = "none";
    if(strcmp(searchNode(list,string), "none") == 0){
        return string;
    }
    else{
        labelTranslation = searchNode(list,string);
        return labelTranslation;
    }
    assert(strcmp(labelTranslation,"none") == 0);
}
//determine the 'a' bit -> 0 or 1
char aBit(char string[]){
    char bit = '\0';
    int found = 0;

    if(strlen(string) > 0){
        for (unsigned int i =0; i< strlen(string); i++){
            if(string[i] == 'A'){
                bit = '0';
                found = 1;
            }
            else if (string[i] == 'M'){
                bit = '1';
                found = 1;
            }
        }
        if (found == 0){
            bit = '0';
        }
    }
    return bit;
}

// translate breakBefore/After to binary
char* compTrans(char string[]){
    char *binary = (char*)malloc(3*sizeof(char));

    if (strcmp(string, "0\0") == 0){
        strcpy(binary, "101010");
    }
    else if (strcmp(string, "1\0") == 0){
        strcpy(binary, "111111");
    }
    else if (strcmp(string, "-1\0") == 0){
        strcpy(binary, "111010");
    }
    else if (strcmp(string, "D\0") == 0){
        strcpy(binary, "001100");
    }
    else if (strcmp(string, "A\0") == 0 || strcmp(string, "M\0") == 0){
        strcpy(binary, "110000");
    }
    else if (strcmp(string, "!D\0") == 0){
        strcpy(binary, "001101");
    }
    else if (strcmp(string, "!A\0") == 0 || strcmp(string, "!M\0") == 0){
        strcpy(binary, "110001");
    }
    else if (strcmp(string, "-D\0") == 0){
        strcpy(binary, "001111");
    }
    else if (strcmp(string, "-A\0") == 0 || strcmp(string, "-M\0") == 0){
        strcpy(binary, "110011");
    }
    else if (strcmp(string, "D+1\0") == 0){
        strcpy(binary, "011111");
    }
    else if (strcmp(string, "A+1\0") == 0 || strcmp(string, "M+1\0") == 0){
        strcpy(binary, "110111");
    }
    else if (strcmp(string, "D-1\0") == 0){
        strcpy(binary, "001110");
    }
    else if (strcmp(string, "A-1\0") == 0 || strcmp(string, "M-1\0") == 0){
        strcpy(binary, "110010");
    }
    else if (strcmp(string, "D+A\0") == 0 || strcmp(string, "D+M\0") == 0){
        strcpy(binary, "000010");
    }
    else if (strcmp(string, "D-A\0") == 0 || strcmp(string, "D-M\0") == 0){
        strcpy(binary, "010011");
    }
    else if (strcmp(string, "A-D\0") == 0 || strcmp(string, "M-D\0") == 0){
        strcpy(binary, "000111");
    }
    else if (strcmp(string, "D&A\0") == 0 || strcmp(string, "D&M\0") == 0){
        strcpy(binary, "000000");
    }
    else if (strcmp(string, "D|A\0") == 0 || strcmp(string, "D|M\0") == 0){
        strcpy(binary, "010101");
    }
    else{
        puts("Error on line 270");
    }



    /*
    int lastInt = strlen(binary)+1;
    binary[lastInt] = '\0';
    */
    return binary;

}
char* destTrans(char string[]){
    char *binary = (char*)malloc(3*sizeof(char));

    if (strcmp(string, "M\0") == 0){
        strcpy(binary, "001");
    }
    else if (strcmp(string, "D\0") == 0){
        strcpy(binary, "010");
    }
    else if (strcmp(string, "MD\0") == 0){
        strcpy(binary, "011");
    }
    else if (strcmp(string, "A\0") == 0){
        strcpy(binary, "100");
    }
    else if (strcmp(string, "AM\0") == 0){
        strcpy(binary, "101");
    }
    else if (strcmp(string, "AD\0") == 0){
        strcpy(binary, "110");
    }
    else if (strcmp(string, "AMD\0") == 0){
        strcpy(binary, "111");
    }
    else{
        puts("Error on line 307");
    }

    return binary;

}
char* jumpTrans(char string[]){
    char *binary = (char*)malloc(3*sizeof(char));

    if (strcmp(string, "JGT\0") == 0){
        strcpy(binary, "001");
    }
    else if (strcmp(string, "JEG\0") == 0){
        strcpy(binary, "010");
    }
    else if (strcmp(string, "JGE\0") == 0){
        strcpy(binary, "011");
    }
    else if (strcmp(string, "JLT\0") == 0){
        strcpy(binary, "100");
    }
    else if (strcmp(string, "JNE\0") == 0){
        strcpy(binary, "101");
    }
    else if (strcmp(string, "JLE\0") == 0){
        strcpy(binary, "110");
    }
    else if (strcmp(string, "JMP\0") == 0){
        strcpy(binary, "111");
    }
    else{
        puts("Error on line 338");
    }

    return binary;
}

// return if string is a jump or destination instruction
char* jumpOrDest(char string[]){
    char *cType = (char*)malloc(sizeof(char) * 5);

    if(strlen(string) > 0){
        for (unsigned int i =0; i< strlen(string); i++){
            if(string[i] == ';'){
                strcpy(cType, "Jump");
            }
            else if (string[i] == '='){
                strcpy(cType, "Dest");
            }
        }
    }
    else{
        strcpy(cType, "l137");
    }

    int lastInt = strlen(cType)+1;
    cType[lastInt] = '\0';
    return cType;
}

char* breakBefore(char string[], char symbol){
    int wordCount = strlen(string);

    int inc = 0;
    char *splitWord = (char*)malloc(wordCount*sizeof(char));

    if(wordCount > 0){
        for(int i = 0; i < wordCount; i++){
            if(string[i] == symbol){
                i = wordCount;
            }
            splitWord[inc] = string[i];
            inc++;
        }
    }

    int lastInt = inc;
    splitWord[lastInt] = '\0';
    return splitWord;
}

char* breakAfter(char string[], char symbol){

    int wordCount = strlen(string);
    int inc = 0;
    int start = 0;
    char *newWord = (char*)malloc(wordCount*sizeof(char));

    if(wordCount > 0){
        for(int i = 0; i < wordCount; i++){
            char a = string[i];
            if (start == 1 ){
                newWord[inc] = a;
                inc++;
            }
            if(a == symbol){
                start++;
            }
        }
    }

    int lastInt = inc;
    newWord[lastInt] = '\0';
    return newWord;
}

char* removeSpacesAndComment(char *input){

    int len = strlen(input) + 1;

    int stop = 0;
    int begin = 0;
    char compare[2];
    compare[1] = '\0';
    char comment[] = "/\0";
    char *spaceless = (char*)malloc(sizeof(char) * len);

    if (input == NULL){
        return NULL;
    }

    for(int i = 0; i < len; i++){
            compare[0] = input[i];
            if (strcmp(compare,comment) == 0 && begin == 0){
                spaceless[begin] = input[i];
                begin++;
            }
            if (strcmp(compare,comment) == 0){
                stop = 1;
                i=len;
            }
            if (stop == 0 && input[i] != ' '){
                spaceless[begin] = input[i];
                begin++;
            }
    }

    int lastInt = begin ;
    spaceless[lastInt] = '\0';
    return spaceless;
}

int charToDigit(char digit){
    return digit - '0';
}
int stringToInt(char* string){
    //length is the length of the string
    unsigned int len = strlen(string) - 1;

    int sum = 0;
    int power = 0;

    for (int i= len; i>-1; i--){
        sum += (charToDigit(string[i]) * floor(pow(10,power++)));
    }
    return sum;
}
char* intToBinary(int num){
    char *result = (char*)malloc(16*sizeof(char));

    for (int position = 15; position>-1; position--){
        //
        if(num != 0){
            if(num % 2 == 1){
                result[position] = '1';
                num = num/2;
            }
            else{
                result[position] = '0';
                num = num/2;
            }
        }
        else{
            result[position] = '0';
        }

    }

    result[16] = '\0';
    return result;
}

//symbol linked list
//-------------------------------------
void insertAtEnd(struct LinkedList* list, char* currentPosition, char* word)
{

    if(list->head == NULL)
    {
        list->head = malloc(sizeof(struct Node*));
        list->head->memLocation = currentPosition;
        list->head->symbol = word;
        list->head->next = NULL;

    }
    else
    {
        if( strcmp(searchNode(list,word), "none") == 0){

            struct Node* position = list->head;

            while(position->next != NULL){
                position = position->next;
            }

            struct Node* new_node = malloc(sizeof(struct Node));
            new_node->memLocation = currentPosition;
            new_node->symbol = word;
            new_node->next = NULL;
            position->next = new_node;
        }
    }

}

char* searchNode(struct LinkedList* list,char* word){
    char* found = "none";
    struct Node* temp = list->head;

    while(temp != NULL){
         //symbol found return location
         if(strcmp(temp->symbol, word) == 0){
             found = temp->memLocation;
         }
         temp = temp->next;
    }
    //assert(temp == NULL);
    //symbol not found="0"
    return found;
}

void deleteList(struct LinkedList* list)
{
    struct Node* temp = list->head;
    while(temp != NULL){
        list->head = temp->next;
        free(temp);
        temp = list->head;
    }
}

void printList(struct LinkedList* list)
{
    struct Node* temp = list->head;

    while( temp != NULL )
    {
        printf("Label -> %s, Symbol -> %s\n", temp->memLocation, temp->symbol);
        temp = temp -> next;
    }
}

