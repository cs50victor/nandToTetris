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
#include <math.h>    // for power and floor in stingToint function
#include <stdbool.h> // allows bool y = false
#include <assert.h>  // assert test
#include <ctype.h>   // isdigit,islower,isupper,ispunct

void convertVmFile();
char* removeSpacesAndComment(char*);
char* breakBefore(char[], char);
char* breakAfter(char[], char);
char* numberFinder(char []);
char* lineTypeDissector(char [],char,char [],char,char [], char [], int);
char* pushOrpop(char [], char [], char [], char [], char[]);
char* alu(char [], char [], int);
int stringToInt(char*);

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
    char *line = (char*)malloc(sizeof(char) * bufferSize);
    char* firstText = NULL;
    char firstType = '-';
    char* secondText = NULL;
    char secondType = '-';
    char* number = NULL;
    char* out = NULL;
    int labelCounter = 0;
    //Use fgets to read from the file, null when end of file
    while(fgets(buffer, bufferSize, inputFile) != NULL){
        buffer[strcspn(buffer, "\r\n")] = 0;

        line = removeSpacesAndComment(buffer);
        //makes sure line isn't empty
        if(strlen(line)>0){

            firstText = breakBefore(line, ' ');
            secondText = breakAfter(line, ' ');
            number = numberFinder(line);

            if(firstText[0]=='p')
            {
                firstType='M';
            }
            else if (firstText[0]=='f' ||firstText[0]=='c'
                     ||firstText[0]=='r')
            {
                firstType='F';
            }
            else if ((firstText[0]=='i' && firstText[1]=='f') ||
                     (firstText[0]=='l' && firstText[1]=='a' )||
                     (firstText[0]=='g' && firstText[1]=='o' ))
            {
                firstType='B';
            }
            else{
                firstType='O';
            }

            if (islower(secondText[0])){
                //memory segment
                secondType='s';
            }
            else if (isupper(secondText[0]) && isupper(secondText[1])){
                //label
                secondType='L';
            }
            else if (isupper(secondText[0]) && islower(secondText[1])){
                //name of a function
                secondType='N';
            }

            assert(firstType!='-');

            out = lineTypeDissector(firstText,firstType,
                secondText,secondType,number,inputFileName, labelCounter);
            //send the outputAssembly to the file
            fputs(out,outputFile);
        }
    }
    //done reading inputfile and writing to outputfile
    fclose(inputFile);
    fputs("(END)\n@END\n0;JMP",outputFile);
    fclose(outputFile);
    //mallocs
    free(line);
    free(firstText);
    free(secondText);
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

char* lineTypeDissector(char firstText[],char firstType,
    char secondText[],char secondType,char number[], char inputFileName[], int labelCounter){

    char popOrpopAssembly[500];
    char aluAssembly[500];
    char* assembly=NULL;

     //first 2 && secondType=='-'
    if(firstType=='O'){
        //(Arithmetic /Logical commands)
        assembly=alu(firstText,aluAssembly, labelCounter);
    }
    else if(firstType=='F' && secondType=='-'){
        //return (Function command)
        assembly="IMPLEMENTED IN VM2";
    }
    else if(firstType=='F' && secondType=='N'){
        //function or call (Function command)
        assembly="IMPLEMENTED IN VM2";
    }
    else if(firstType=='M' && secondType=='s' && number){
        //push or pop segment (Memory Access commands)
        assembly=pushOrpop(firstText,secondText,number,popOrpopAssembly, inputFileName);
    }
    else if(firstType=='B' && secondType=='L'){
        //label, goto, if-goto LABEL(Branching commands)
        assembly="IMPLEMENTED IN VM2";
    }
    else{
        puts("Error -> LINE 272");
        puts(firstText);
        puts(&firstType);
        puts(&secondType);
        assembly=firstText;
    }

    assert(assembly != NULL);
    return assembly;
}


char* pushOrpop(char firstText[], char secondText[], char number[],char popOrpopAssembly[], char inputFileName[]){


    if(firstText[1]=='u'){
        //push memoryType number
        if(memcmp(secondText, "local",4) == 0 ||
                memcmp(secondText, "argument",7) == 0 ||
                memcmp(secondText, "this",3) == 0 ||
                memcmp(secondText, "that",3) == 0){

            char memType[6];
            if(strcmp(secondText, "local\0")){
                strcpy(memType, "LCL");
            }
            else if (strcmp(secondText, "argument\0")){
                strcpy(memType, "ARG");
            }
            else if (strcmp(secondText, "that\0") == 0){
                strcpy(memType, "THAT");
            }
            else{strcpy(memType, "THIS");}

            strcpy(popOrpopAssembly, "@");strcat(popOrpopAssembly, number);
            strcat(popOrpopAssembly, "\n");strcat(popOrpopAssembly, "D=A\n");
            strcat(popOrpopAssembly, "@");strcat(popOrpopAssembly, memType);
            strcat(popOrpopAssembly, "\nA=M\nD=D+A\nA=D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
        }
        else if(memcmp(secondText, "constant",7) == 0){
            strcpy(popOrpopAssembly, "@");
            strcat(popOrpopAssembly, number);strcat(popOrpopAssembly, "\n");
            strcat(popOrpopAssembly, "D=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
        }
        else if(memcmp(secondText, "static",5) == 0 ||
                memcmp(secondText, "temp",3) == 0 ||
                memcmp(secondText, "pointer",6) == 0){


            char fileNamePlusIndex[50];
            int RAM;
            char RAMSTRING[20];
            if(strcmp(secondText, "static\0")){
                strcpy(fileNamePlusIndex, inputFileName);
                strcat(fileNamePlusIndex, ".");
                strcat(fileNamePlusIndex, number);
            }
            else if (strcmp(secondText, "temp\0")){
                RAM=stringToInt(number) + 5;
                sprintf(RAMSTRING, "%d", RAM);
                strcpy(fileNamePlusIndex, "R");
                strcpy(fileNamePlusIndex, RAMSTRING);
            }
            else{
                //pointer
                RAM= stringToInt(number) + 3;
                strcpy(fileNamePlusIndex, "R");
                sprintf(RAMSTRING, "%d", RAM);
                strcpy(fileNamePlusIndex, RAMSTRING);

            }
            assert(fileNamePlusIndex != NULL);

            strcpy(popOrpopAssembly, "@");
            strcat(popOrpopAssembly, fileNamePlusIndex);strcat(popOrpopAssembly, "\n");
            strcat(popOrpopAssembly, "D=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
        }

        else{
            strcpy(popOrpopAssembly, "line 342-> Push memory segment comparison error");
        }
    }
    else{
        //pop segment i
        if(memcmp(secondText, "local",4) == 0 ||
                memcmp(secondText, "argument",7) == 0 ||
                memcmp(secondText, "this",3) == 0 ||
                memcmp(secondText, "that",3) == 0){

            char memType[6];
            if(strcmp(secondText, "local\0")){
                strcpy(memType, "LCL");
            }
            else if (strcmp(secondText, "argument\0")){
                strcpy(memType, "ARG");
            }
            else if (strcmp(secondText, "that\0") == 0){
                strcpy(memType, "THAT");
            }
            else{strcpy(memType, "THIS");}

            strcpy(popOrpopAssembly, "@");
            strcat(popOrpopAssembly, number);
            strcat(popOrpopAssembly, "\nD=A\n@");
            strcat(popOrpopAssembly, memType);
            strcat(popOrpopAssembly, "\nA=M\nD=D+A\n@");
            strcat(popOrpopAssembly, memType);
            strcat(popOrpopAssembly, "\nM=D\n@SP\nAM=M-1\nD=M\n@");
            strcat(popOrpopAssembly, memType);
            strcat(popOrpopAssembly, "\nA=M\nM=D\n@");
            strcat(popOrpopAssembly, number);
            strcat(popOrpopAssembly, "\nD=A\n@");
            strcat(popOrpopAssembly, memType);
            strcat(popOrpopAssembly, "\nA=M\nD=A-D\n@");
            strcat(popOrpopAssembly, memType);
            strcat(popOrpopAssembly, "\nM=D\n");

        }
        else if(memcmp(secondText, "static",5) == 0 ||
                memcmp(secondText, "temp",3) == 0 ||
                memcmp(secondText, "pointer",6) == 0){

            char fileNamePlusIndex[50];
            int RAM;
            char RAMSTRING[20];
            if(strcmp(secondText, "static\0")){
                strcpy(fileNamePlusIndex, inputFileName);
                strcat(fileNamePlusIndex, ".");
                strcat(fileNamePlusIndex, number);
            }
            else if (strcmp(secondText, "temp\0")){
                RAM=stringToInt(number) + 5;
                sprintf(RAMSTRING, "%d", RAM);
                strcpy(fileNamePlusIndex, "R");
                strcpy(fileNamePlusIndex, RAMSTRING);
            }
            else{
                //pointer
                RAM= stringToInt(number) + 3;
                strcpy(fileNamePlusIndex, "R");
                sprintf(RAMSTRING, "%d", RAM);
                strcpy(fileNamePlusIndex, RAMSTRING);

            }
            assert(fileNamePlusIndex != NULL);

            strcpy(popOrpopAssembly, "@SP\nAM=M-1\nD=M\n@");
            strcat(popOrpopAssembly, fileNamePlusIndex);strcat(popOrpopAssembly, "\n");
            strcat(popOrpopAssembly, "M=D\n");
        }
        else{
            strcpy(popOrpopAssembly, "line 414-> Push memory segment comparison error");
        }
    }

    assert(popOrpopAssembly != NULL);
    return popOrpopAssembly;
}
// aka arithmetic-logic unit
char* alu(char command[], char aluAssembly[], int labelCounter){
    char versatile[10];
    if(memcmp(command, "neg",3) == 0 ||
            memcmp(command, "not",3) == 0){

        if(strcmp(command, "neg\0") == 0){strcpy(versatile, "M=-M");}
        else{strcpy(versatile, "M=!M");}

        strcpy(aluAssembly, "@SP\nAM=M-1\n");
        strcat(aluAssembly, versatile);
        strcat(aluAssembly, "\n@SP\nM=M+1\n");
    }
    else if(memcmp(command, "add",3) == 0 ||
            memcmp(command, "and",3) == 0 ||
            memcmp(command, "or",2) == 0 ||
            memcmp(command, "sub",3) == 0){

        if(strcmp(command, "add\0") == 0){strcpy(versatile, "M=D+M");}
        else if(strcmp(command, "and\0") == 0){strcpy(versatile, "M=D&M");}
        else if(strcmp(command, "or\0") == 0){strcpy(versatile, "M=D|M");}
        else{strcpy(versatile, "M=M-D");}

        strcpy(aluAssembly, "@SP\nAM=M-1\nD=M\n@SP\nAM=M-1\n");
        strcat(aluAssembly, versatile);
        strcat(aluAssembly, "\n@SP\nM=M+1\n");
    }
    else if(memcmp(command, "eq",2) == 0 ||
            memcmp(command, "gt",2) == 0 ||
            memcmp(command, "lt",2) == 0){

        if(strcmp(command, "eq\0") == 0){strcpy(versatile, "D;JEQ");}
        else if(strcmp(command, "gt\0") == 0){strcpy(versatile, "D;JGT");}
        else{strcpy(versatile, "D;JLT");}

        char labelCounterString[20];
        sprintf(labelCounterString, "%d", labelCounter);

        strcpy(aluAssembly, "@SP\nAM=M-1\nD=M\n@SP\nAM=M-1\nD=M-D\n@");
        strcat(aluAssembly, "COMP.");
        strcat(aluAssembly, labelCounterString);
        strcat(aluAssembly, ".TRUE\n");
        strcat(aluAssembly, versatile);
        strcat(aluAssembly, "\n@");
        strcat(aluAssembly, "COMP.");
        strcat(aluAssembly, labelCounterString);
        strcat(aluAssembly, ".FALSE");
        strcat(aluAssembly, "\n0;JMP\n(");
        strcat(aluAssembly, "COMP.");
        strcat(aluAssembly, labelCounterString);
        strcat(aluAssembly, ".TRUE)");
        strcat(aluAssembly, "\n@SP\nA=M\nM=-1\n@SP\nM=M+1\n@");
        strcat(aluAssembly, "COMP.");
        strcat(aluAssembly, labelCounterString);
        strcat(aluAssembly, ".END\n0;JMP\n(");
        strcat(aluAssembly, "COMP.");
        strcat(aluAssembly, labelCounterString);
        strcat(aluAssembly, ".FALSE)");
        strcat(aluAssembly, "\n@SP\nA=M\nM=0\n@SP\nM=M+1\n(");
        strcat(aluAssembly, "COMP.");
        strcat(aluAssembly, labelCounterString);
        strcat(aluAssembly, ".END)\n");
        labelCounter++;
    }

    assert(aluAssembly != NULL);
    return aluAssembly;
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

























