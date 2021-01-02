# NandToTetris / Hack Virtual Machine

> The program takes a hack vm file as input and generates an output in hack assembly format, which can then be translated into machine code by the assembler.

## Example
> Virtual machine Translator Folder -> "vm2"

### SimpleAdd.vm [ Turns this file ]

<code>
// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/07/StackArithmetic/SimpleAdd/SimpleAdd.vm

// Pushes and adds two constants.
push constant 7
push constant 8
add
</code>

### SimpleAdd.asm  [ into this file ]

`
// push constant 7
@7
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 8
@8
D=A
@SP
A=M
M=D
@SP
M=M+1
// add
@SP
M=M-1
A=M
D=M
@SP
M=M-1
A=M
M=M+D
@SP
M=M+1
`

<br/>

> Assembler Translator Directory -> "2passAssembler/completeAssembler2"

### Example.asm  [ Turns this assembly file ]

>// Given two numbers stored in the registers R0 and R1,
// compute the maximum between them and store it in the R2 register.
<br/>
@R0
<br/>
D=M             
@R1
<br/>
D=D-M  
@OUTPUT_FIRST
<br/>
D;JGT      
@R1
<br/>
D=M       
@OUTPUT_D
<br/>
0;JMP  
(OUTPUT_FIRST)
<br/>
@R0
<br/>
D=M  
(OUTPUT_D)
<br/>
@R2
<br/>
M=D     
(INFINITE_LOOP)
<br/>
@INFINITE_LOOP
<br/>
0;JMP

### Example.hack  [ into this hack machine code ]

>
>0000000000000000
<br/>
1111110000010000
<br/>
0000000000000001
<br/>
1111010011010000
<br/>
0000000000001010
<br/>
1110001100000001
<br/>
0000000000000001
<br/>
1111110000010000
<br/>
0000000000001100
<br/>
1110101010000111
<br/>
0000000000000000
<br/>
1111110000010000
<br/>
0000000000000010
<br/>
1110001100001000
<br/>
0000000000001110
<br/>
1110101010000111
