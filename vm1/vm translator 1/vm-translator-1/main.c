/*
 * Victor Atasie
VIRTUAL MACHINE TRANSLATOR
    * processfile()
    * Input filename: example.vm
    * automated -> Output filename: example.asm
    * cpu emulator software, project 7,
test file (same name and directory as output.asm)
*/

#include <stdio.h>   // io
#include <stdlib.h>  // malloc
#include <string.h>  // for strlen, strcpy, strcmp
#include <stdbool.h> // allows bool y = false
#include <assert.h>  // assert test
#include <ctype.h>   // isdigit,islower,isupper,ispunct

void convertVmFile();
char* removeSpacesAndComment(char*);
char* breakBefore(char[], char);
char* breakAfter(char[], char);
char* numberFinder(char []);
char* lineTypeDissector(char [],char,char [],char,char []);

int main()
{
    convertVmFile();
    return 0;
}


void convertVmFile(){
    //Files are declared using file pointers
    FILE* inputFile = NULL;
    FILE* outputFile = NULL;

    const int bufferSize = 256;
    //256 is an arbitrary value
    char inputFileName[bufferSize];
    //Get file name from keyboard
    printf("Please enter an input filename: ");
    fgets(inputFileName, bufferSize, stdin);
     //removes the \n
    inputFileName[strcspn(inputFileName, "\n")] = 0;
    //Open files with fopen, read only
    inputFile = fopen(inputFileName, "r");

    //autoGenerate the outputFileName
    int inputLen = strlen(inputFileName);
    inputFileName[inputLen-3]='\0';
    char outputFileName[inputLen];
    strcpy(outputFileName, inputFileName);
    strcat(outputFileName, ".txt");
    printf("Output is in this file -> %s\n", outputFileName);

    //Open files with fopen, write only
    outputFile = fopen(outputFileName, "w");

    //If the file pointer are none, the file didn't open
    if(inputFile == NULL){
        puts("Error opening input file, restart program");
    }
    //makes sure code doesn't keep running if there no input file
    assert(inputFile != NULL);
    char buffer[bufferSize];
    //to keep track of each letter in a line while reading the file
    int i = 0;
    char *line = (char*)malloc(sizeof(char) * bufferSize);
    char* first = NULL;
    char firstType = '-';
    char* second = NULL;
    char secondType = '-';
    char* number = NULL;
    char* out = NULL;
    //Use fgets to read from the file, null when end of file
    while(fgets(buffer, bufferSize, inputFile) != NULL){
        buffer[strcspn(buffer, "\r\n")] = 0;

        line = removeSpacesAndComment(buffer);

        if(strlen(line)>0){

             first = breakBefore(line, ' ');
             second = breakAfter(line, ' ');
             number = numberFinder(line);

            //send the line to the file & add a new line
            fputs(first,outputFile);
            fputs("\n",outputFile);
            if(strlen(second)>0){
                fputs(second,outputFile);
                fputs("\n",outputFile);
            }
            if(strlen(number)>0){
                fputs(number,outputFile);
                fputs("\n",outputFile);
            }

            if(first[0]=='p')
            {
                firstType='M';
            }
            else if (first[0]=='f' ||first[0]=='c'
                     ||first[0]=='r')
            {
                firstType='F';
            }
            else if (first[0]=='i' ||
                     (first[0]=='l' && first[1]=='a' )||
                     (first[0]=='g' && first[1]=='o' ))
            {
                firstType='P';
            }
            else{
                firstType='O';
            }




            assert(firstType!='-' && secondType!='-');
            out = lineTypeDissector(first,firstType,
                second,secondType,number);
        }
    }
    //done reading inputfile and writing to outputfile
    fclose(inputFile);
    fclose(outputFile);
    //mallocs
    free(line);
    free(first);
    free(second);
    free(number);
    free(out);

};

char* removeSpacesAndComment(char *input){

    int len = strlen(input) + 1;


    int begin = 0;
    char compare[2];
    compare[1] = '\0';
    char comment[] = "/\0";
    char *spaceless = (char*)malloc(sizeof(char) * len);

    if (input == NULL){
        return NULL;
    }

    // loops scans text horizontally till the end(len)
    for(int i = 0; i < len; i++){
            compare[0] = input[i];
            //stops for loop if it encounters a "/"
            if (strcmp(compare,comment) == 0){
                i=len;
            }
            if(input[i]==' ' && begin==0){
                i++;
            }
            // else it just adds everyting to 'spaceless'
            else{
                spaceless[begin] = input[i];
                begin++;
            }
    }

    int lastInt = begin ;
    spaceless[lastInt] = '\0';
    return spaceless;
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
char* numberFinder(char string[]){
    int begin = 0;
    int length = strlen (string);
    char* digits = (char*)malloc(sizeof(char) * length);
    for (int i=0;i<length; i++){
        if (isdigit(string[i]))
        {
            digits[begin] = string[i];
            begin++;
        }
    }
    digits[begin]='\0';
    return digits;
}

char* lineTypeDissector(char first[],char firstType,
    char second[],char secondType,char number[]){

}






























