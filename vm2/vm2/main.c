/*
 * Victor Atasie
VIRTUAL MACHINE TRANSLATOR
    * convertVmFile()
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
char* lineTypeDissector(char [],char,char [],char,char [], char []);
char* pushOrpop(char [], char [], char [], char [], char[]);
char* alu(char [], char []);
char* ifGotoLabel(char [],char [], char []);
char* functionOrCall(char [], char [], char [], char []);
char* returnCommand(char [], char []);
int stringToInt(char*);

int ltCounter = -1;
int eqCounter = -1;
int gtCounter = -1;
int callCounter = -1;

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
    strcat(outputFileName, ".asm");


    //Open files with fopen, write only
    outputFile = fopen(outputFileName, "w");

    //If the file pointer are none, the file didn't open
    if(inputFile == NULL){
        puts("Error opening input file, restart program");
    }
    //makes sure code doesn't keep running if there no input file
    assert(inputFile != NULL);
    printf("Output is in this file -> %s\n", outputFileName);
    char buffer[bufferSize];
    char *line = (char*)malloc(sizeof(char) * bufferSize);
    char* firstText = NULL;
    char firstType = '-';
    char* secondText = NULL;
    char secondType = '-';
    char* number = NULL;
    char* out = NULL;

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
            else{
                //secondtype is empty
                secondType='E';
            }

            assert(firstType!='-' && secondType!='-');

            out = lineTypeDissector(firstText,firstType,
                secondText,secondType,number,inputFileName);
            //send the outputAssembly to the file
            fputs(out,outputFile);
            //free(out);
        }
    }
    //done reading inputfile and writing to outputfile
    fclose(inputFile);
    fputs("(END)\n@END\n0;JMP\n//VICTOR ATASIE",outputFile);
    fclose(outputFile);
    //mallocs
    free(line);
    free(firstText);
    free(secondText);
    free(number);


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
    char secondText[],char secondType,char thirdText[], char inputFileName[]){

    //crazy amount of memory , because I was getting buffer overflow errors

    char* getAssembly= malloc(sizeof(char) * 3024);
    char* assembly=NULL;


     //first 2 && secondType=='-', 'E' for empty
    if(firstType=='O' && secondType=='E'){
        //char aluAssembly[400];
        //(Arithmetic /Logical commands)
        assembly=alu(firstText,getAssembly);
    }
    else if(firstType=='F' && secondType=='E'){
        //char returnAssembly[100];
        //return (Function command)
        assembly=returnCommand(firstText, getAssembly);
    }
    else if(firstType=='F' && secondType=='N'){
        //char functionOrCallAssembly[100];
        //function or call (Function command)
        assembly=functionOrCall(firstText, secondText, thirdText, getAssembly);
    }
    else if(firstType=='M' && secondType=='s' && thirdText){
        //char popOrpopAssembly[100];
        //push or pop segment (Memory Access commands)
        assembly=pushOrpop(firstText,secondText,thirdText,getAssembly, inputFileName);
    }
    else if(firstType=='B' && secondType=='L'){
        //char branchingAssembly[60];
        //label, goto, if-goto LABEL(Branching commands)
        assembly=ifGotoLabel(firstText,secondText,getAssembly);
    }
    else{
        puts("Error -> LINE 274");
        puts(firstText);
        puts(&firstType);
        puts(&secondType);
    }
    assert(assembly != NULL);
    return assembly;
}
char* pushOrpop(char firstText[], char secondText[], char number[],char popOrpopAssembly[], char inputFileName[]){
    char versatile[30];
    char buffer[15];
    int staticPointerNumber;

    if(firstText[1]=='u'){
        //push memoryType number
        if(memcmp(secondText, "local",4) == 0 ||
                memcmp(secondText, "argument",7) == 0 ||
                memcmp(secondText, "this",3) == 0 ||
                memcmp(secondText, "that",3) == 0){

            char memType[6];
            if(memcmp(secondText, "local",4)==0){
                strcpy(memType, "LCL");
            }
            else if (memcmp(secondText, "argument",4)==0){
                strcpy(memType, "ARG");
            }
            else if (memcmp(secondText, "that",4) == 0){
                strcpy(memType, "THAT");
            }
            else{strcpy(memType, "THIS");}

            // Logic -> addr = segmentPointer + number, *SP = *addr, SP++
            printf("%s %s %s\n",firstText,memType,number);
            strcpy(popOrpopAssembly, "@");
            strcat(popOrpopAssembly, number);
            strcat(popOrpopAssembly, "\nD=A\n@");
            strcat(popOrpopAssembly, memType);
            strcat(popOrpopAssembly, "\nA=M\nD=D+A\nA=D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");

        }
        else if(memcmp(secondText, "constant",7) == 0){
            printf("%s %s %s\n",firstText,secondText,number);
            // Logic -> put @number where SP is and increment its position (@SP,M=M+1)\n ")
            strcpy(popOrpopAssembly, "@");
            strcat(popOrpopAssembly, number);
            strcat(popOrpopAssembly, "\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
        }
        else if(memcmp(secondText, "static",5) == 0 ||
                memcmp(secondText, "temp",3) == 0 ||
                memcmp(secondText, "pointer",6) == 0){

            if(memcmp(secondText, "static",5)==0){
                printf("%s %s %s\n",firstText,secondText,number);
                //Logic -> addr = inputFileName.number, SP--, *addr = *SP \n
                strcpy(versatile, inputFileName);
                strcat(versatile, ".");
                strcat(versatile, number);
            }
            else if (memcmp(secondText, "temp",3)==0){
                printf("%s %s %s\n",firstText,secondText,number);
                //Logic -> addr = 5 + i, *SP = *addr, SP++
                staticPointerNumber = stringToInt(number) + 5;
                sprintf(buffer, "%d", staticPointerNumber);
                assert(buffer != NULL);
                strcpy(versatile, "R");
                strcat(versatile, buffer);
            }
            else{
                printf("%s %s %s\n",firstText,secondText,number);
                //pointer
                // Logic -> addr = 3 + i, *SP = *addr, SP++
                staticPointerNumber = stringToInt(number) + 3;
                sprintf(buffer, "%d", staticPointerNumber);
                assert(buffer != NULL);
                strcpy(versatile, "R");
                strcat(versatile, buffer);
            }

            strcpy(popOrpopAssembly, "@");
            strcat(popOrpopAssembly, versatile);
            strcat(popOrpopAssembly, "\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
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

            if(memcmp(secondText, "local",4)==0){
                strcpy(memType, "LCL");
            }
            else if (memcmp(secondText, "argument",3)==0){
                strcpy(memType, "ARG");
            }
            else if (memcmp(secondText, "that",4) == 0){
                strcpy(memType, "THAT");
            }
            else{strcpy(memType, "THIS");}
            printf("%s %s %s\n",firstText,memType,number);

            // Logic ->  SP--,addr =  segmentPointer + number, *addr = *SP

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


            /*
             * dOESNT switch values of top of stack with @memtype
            strcpy(popOrpopAssembly, "@SP\nAM=M-1\nD=M\n@");
            strcat(popOrpopAssembly, number);
            strcat(popOrpopAssembly, "\nD=A\n@");
            strcat(popOrpopAssembly, memType);
            strcat(popOrpopAssembly, "\nA=M\nD=D+A\nA=D\nD=M\n@SP\nA=M\nM=D\n");
            */

        }
        else if(memcmp(secondText, "static",5) == 0 ||
                memcmp(secondText, "temp",3) == 0 ||
                memcmp(secondText, "pointer",6) == 0){


            if(memcmp(secondText, "static",5)==0){
                printf("%s %s %s\n",firstText,secondText,number);

                //Logic -> addr = inputFileName.number, SP--, *addr = *SP \n
                strcpy(versatile, inputFileName);
                strcat(versatile, ".");
                strcat(versatile, number);

            }
            else if (memcmp(secondText, "temp",3)==0){

                printf("%s %s %s\n",firstText,secondText,number);
                //Logic -> addr = 5 + i, SP--, *addr = *SP
                staticPointerNumber = stringToInt(number) + 5;
                sprintf(buffer, "%d", staticPointerNumber);
                assert(buffer != NULL);
                strcpy(versatile, "R");
                strcat(versatile, buffer);
                puts(buffer);
            }
            else{
                //pointer
                printf("%s %s %s\n",firstText,secondText,number);

                // Logic -> addr = 3 + i, SP--, *addr = *SP
                staticPointerNumber = stringToInt(number) + 3;
                sprintf(buffer, "%d", staticPointerNumber);
                assert(buffer != NULL);
                strcpy(versatile, "R");
                strcat(versatile, buffer);


            }
            // Logic ->  SP--, addr = 3 + i, *addr = *SP
            strcpy(popOrpopAssembly, "@SP\nAM=M-1\nD=M\n@");
            strcat(popOrpopAssembly, versatile);
            strcat(popOrpopAssembly, "\nM=D\n");
        }
        else{
            strcpy(popOrpopAssembly, "line 414-> Push memory segment comparison error");
        }
    }

    assert(popOrpopAssembly != NULL);
    return popOrpopAssembly;
}
// aka arithmetic-logic unit
char* alu(char command[], char aluAssembly[]){
    char versatile[10];
    char intCounterString[10];
    char labelStartString[20];
    char labelEndString[20];

    if(memcmp(command, "neg",3) == 0 ||
            memcmp(command, "not",3) == 0){

        if(strcmp(command, "neg\0") == 0){puts(command);strcpy(versatile, "M=-M");}
        else{puts(command);strcpy(versatile, "M=!M");}

        strcpy(aluAssembly, "@SP\nA=M-1\n");
        strcat(aluAssembly, versatile);
        strcat(aluAssembly, "\n");
    }
    else if(memcmp(command, "add",3) == 0 ||
            memcmp(command, "and",3) == 0 ||
            memcmp(command, "or",2) == 0 ||
            memcmp(command, "sub",3) == 0){

        if(strcmp(command, "add\0") == 0){puts(command);strcpy(versatile, "D=D+M");}
        else if(strcmp(command, "and\0") == 0){puts(command);strcpy(versatile, "D=D&M");}
        else if(strcmp(command, "or\0") == 0){puts(command);strcpy(versatile, "D=D|M");}
        else{puts(command);strcpy(versatile, "D=D-M");}

        strcpy(aluAssembly, "@SP\nA=M-1\nD=M\n@R13\nM=D\n@SP\nM=M-1\n@SP\nA=M-1\nD=M\n@R13\n");
        strcat(aluAssembly, versatile);
        strcat(aluAssembly, "\n@SP\nA=M-1\nM=D\n");
    }
    else if(memcmp(command, "eq",2) == 0 ||
            memcmp(command, "gt",2) == 0 ||
            memcmp(command, "lt",2) == 0){


        if(strcmp(command, "eq\0") == 0){
            puts(command);
            eqCounter=eqCounter+1;
            strcpy(versatile, "D;JEQ");
            sprintf(intCounterString, "%d", eqCounter);

            strcpy(labelStartString, "RANDOM");
            strcat(labelStartString, ".EQ.START.");
            strcat(labelStartString, intCounterString);

            strcpy(labelEndString, "RANDOM");
            strcat(labelEndString, ".EQ.END.");
            strcat(labelEndString, intCounterString);

        }
        else if(strcmp(command, "gt\0") == 0){
            puts(command);
            gtCounter=gtCounter+1;
            strcpy(versatile, "D;JGT");
            sprintf(intCounterString, "%d", gtCounter);

            strcpy(labelStartString, "RANDOM");
            strcat(labelStartString, ".GT.START.");
            strcat(labelStartString, intCounterString);

            strcpy(labelEndString, "RANDOM");
            strcat(labelEndString, ".GT.END.");
            strcat(labelEndString, intCounterString);
        }
        else{
            puts(command);
            ltCounter=ltCounter+1;
            strcpy(versatile, "D;JLT");
            sprintf(intCounterString, "%d", ltCounter);

            strcpy(labelStartString, "RANDOM");
            strcat(labelStartString, ".LT.START.");
            strcat(labelStartString, intCounterString);

            strcpy(labelEndString, "RANDOM");
            strcat(labelEndString, ".LT.END.");
            strcat(labelEndString, intCounterString);
        }

        strcpy(aluAssembly, "@SP\nA=M-1\nD=M\n@R13\nM=D\n@SP\nM=M-1\n"
                            "@SP\nA=M-1\nD=M\n@R13\nD=D-M\n@");
        strcat(aluAssembly, labelStartString);
        strcat(aluAssembly, "\n");
        strcat(aluAssembly, versatile);

        strcat(aluAssembly, "\n@SP\nA=M-1\nM=0\n@");
        strcat(aluAssembly, labelEndString);
        strcat(aluAssembly, "\n");
        strcat(aluAssembly, "0;JMP\n(");
        strcat(aluAssembly, labelStartString);
        strcat(aluAssembly, ")");
        strcat(aluAssembly, "\n@SP\nA=M-1\nM=-1\n(");
        strcat(aluAssembly, labelEndString);
        strcat(aluAssembly, ")\n");
    }

    assert(aluAssembly != NULL);
    return aluAssembly;
}
//for branching commands -> label, goto label, if-goto
char* ifGotoLabel(char firstText[],char secondText[],char branchingAssembly[]){
    if (memcmp(firstText, "label",5) == 0){
        strcpy(branchingAssembly, "(");
        strcat(branchingAssembly, secondText);
        strcat(branchingAssembly, ")");
        strcat(branchingAssembly, "\n");
    }
    else if (memcmp(firstText, "goto",4) == 0){
        strcpy(branchingAssembly, "@");
        strcat(branchingAssembly, secondText);
        strcat(branchingAssembly, "\n0;JMP\n");
    }
    else if (memcmp(firstText, "if-goto",7) == 0){
        strcpy(branchingAssembly, "@SP\nM=M-1\nA=M\nD=M\n@");
        strcat(branchingAssembly, secondText);
        strcat(branchingAssembly, "\nD;JNE\n");
    }
    else{
        puts("Line 575");
    }
    assert(branchingAssembly != NULL);
    return branchingAssembly;
}
char* functionOrCall(char firstText[],char secondText[],char thirdText[], char functionOrCallAssembly[]){

    int functionLoop;
    char callCounterString[10];
    char versatile[10];

    if (memcmp(firstText, "function",8) == 0){

        strcpy(functionOrCallAssembly, "(");
        strcat(functionOrCallAssembly, secondText);
        strcat(functionOrCallAssembly, ")");
        strcat(functionOrCallAssembly, "\n");
        functionLoop = stringToInt(thirdText);
        printf("%d\n",functionLoop);
        for(int i = 0; i < functionLoop; i=i+1){
            strcat(functionOrCallAssembly, "@SP\nA=M\nM=0\n@SP\nM=M+1\n");
        }
    }
    else if (memcmp(firstText, "call",4) == 0){
        callCounter = callCounter +1 ;
        sprintf(callCounterString, "%d", callCounter);
        strcpy(versatile, "CALL");
        strcat(versatile, callCounterString);


        strcpy(functionOrCallAssembly, "@");
        strcat(functionOrCallAssembly, versatile);
        // replaced using multiple returns
        strcat(functionOrCallAssembly, "\nD=A\n@SP\nA=M\nM=D\n@SP\n"
                                       "M=M+1\n@LCL\nD=M\n@SP\nA=M\n"
                                       "M=D\n@SP\nM=M+1\n@ARG\nD=M\n"
                                       "@SP\nA=M\nM=D\n@SP\nM=M+1\n@"
                                       "THIS\nD=M\n@SP\nA=M\nM=D\n@SP"
                                       "\nM=M+1\n@THAT\nD=M\n@SP\nA=M"
                                       "\nM=D\n@SP\nM=M+1\n@SP\nD=M\n"
                                       "@");
        strcat(functionOrCallAssembly, thirdText);
        strcat(functionOrCallAssembly, "\nD=D-A\n@5\nD=D-A\n@ARG\nM=D\n"
                                       "@SP\nD=M\n@LCL\nM=D\n");
        strcat(functionOrCallAssembly, "@");
        strcat(functionOrCallAssembly, secondText);
        strcat(functionOrCallAssembly, "\n0;JMP\n");
        strcat(functionOrCallAssembly, "(");
        strcat(functionOrCallAssembly, versatile);
        strcat(functionOrCallAssembly, ")");
        strcat(functionOrCallAssembly, "\n");
    }
    else{
        puts("Error -> Line 575");
    }
    assert(functionOrCallAssembly != NULL);
    return functionOrCallAssembly;
};
char* returnCommand(char firstText[], char returnAssembly[]){

    if (memcmp(firstText, "return",6) == 0){
        strcpy(returnAssembly, "@1\nD=M\n@13\nM=D\n@13\nD=M-1\nD=D-1\n"
                               "D=D-1\nD=D-1\nA=D-1\nD=M\n@14\n"
                               "M=D\n@0\nM=M-1\nA=M\nD=M\n@2\nA=M\nM=D\n"
                               "@2\nD=M+1\n@0\nM=D\n@13\nA=M-1\nD=M\n"
                               "@4\nM=D\n@13\nD=M\nA=D-1\nA=A-1\nD=M\n@3\n"
                               "M=D\n@13\nD=M\nA=D-1\nA=A-1\nA=A-1\n"
                               "D=M\n\n@2\nM=D\n@13\nD=M\nA=D-1\nA=A-1\n"
                               "A=A-1\nA=A-1\nD=M\n@1\nM=D\n@14\nA=M\n0;JMP\n");

    }
    else{
        puts("Error -> Line 575");
    }

    assert(returnAssembly != NULL);
    return returnAssembly;
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

/*
 * VM2 done in debug mode due to previous buffer overflow
 *  100% working
 * alu
 * pop
 * push
 *
 *
 * passed -> programFlow tests + Simple Function Test
 * call function hasn't been worked on thoroughly
*/
