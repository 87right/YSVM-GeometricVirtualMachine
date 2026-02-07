# 再帰処理
```
START
CONST 16 CALL CONST 1 CONST 0
CONST 1
CONST 1
ENTER
CONST 28 JMP
CONST 1 CONST 0 LPUSH CONST 10 IMORE JMPIF RET
CONST 16 CALL CONST 1 CONST 0
CONST 0 LPUSH dOPTOP CONST 1 IADD CONST 1
ENTER
RET
STOP
```
->
```
START

current top of operand stack: 1

current top of operand stack: 2

current top of operand stack: 3

current top of operand stack: 4

current top of operand stack: 5

current top of operand stack: 6

current top of operand stack: 7

current top of operand stack: 8

current top of operand stack: 9

STOP
(^^)b iikanzi: 0
```
# 演算処理
```
START
"--- YSVM Math Test ---\n"

"10 + 2 = " CONST 10 CONST 2 IADD ICOUT "\n"
"10 - 2 = " CONST 10 CONST 2 ISUB ICOUT "\n"
"10 * 2 = " CONST 10 CONST 2 IMUL ICOUT "\n"
"10 / 2 = " CONST 10 CONST 2 IDIV ICOUT "\n"
"10 % 3 = " CONST 10 CONST 3 IMOD ICOUT "\n"

"\n--- Float Test ---\n"
"1.23 + 4.56 = " CONST 1.23 CONST 4.56 FADD FCOUT "\n"
"10.0 - 0.5 = " CONST 10.0 CONST 0.5 FSUB FCOUT "\n"
"2.5 * 4.0 = " CONST 2.5 CONST 4.0 FMUL FCOUT "\n"
"10.0 / 3.0 = " CONST 10.0 CONST 3.0 FDIV FCOUT "\n"

"\n--- Mix Test ---\n"
"10 / 3.3 = " CONST 10 ITF CONST 3.3 FDIV FCOUT "\n"
"3.3 / 10 = " CONST 3.3 CONST 10 ITF FDIV FCOUT "\n"



STOP
```
->
```
START
--- YSVM Math Test ---
10 + 2 = 12
10 - 2 = 8
10 * 2 = 20
10 / 2 = 5
10 % 3 = 1

--- Float Test ---
1.23 + 4.56 = 5.79
10.0 - 0.5 = 9.5
2.5 * 4.0 = 10
10.0 / 3.0 = 3.33333

--- Mix Test ---
10 / 3.3 = 3.0303
3.3 / 10 = 0.33

STOP
(^^)b iikanzi: 0
```
# 高速逆平方根
```
START
CONST 2 ITF DUP
CONST 6910469410797702400 SWAP
CONST 1 IRSHIFT
ISUB DUP FCOUT " // initial guess\n"
DDUP DUP FMUL FMUL CONST 0.5 FMUL CONST 1.5 SWAP FSUB FMUL DUP FCOUT " // first iteration\n"
DDUP DUP FMUL FMUL CONST 0.5 FMUL CONST 1.5 SWAP FSUB FMUL DUP FCOUT " // second iteration\n\n"
"input -> " OVER FCOUT "\n"
"rsqrt -> " DUP FCOUT "\n"
"sqrt  -> " FMUL FCOUT "\n"
STOP
```
->
```
START
0.716225 // initial guess
0.70693 // first iteration
0.707107 // second iteration

input -> 2
rsqrt -> 0.707107
sqrt  -> 1.41421

STOP
(^^)b iikanzi: 0
```
# fibonacci
```
START
"1\n1\n"
CONST 1 CONST 1
DUP ROT IADD DUP ICOUT "\n"
DUP ROT IADD DUP ICOUT "\n"
DUP ROT IADD DUP ICOUT "\n"
STOP
```
->
```
START
1
1
2
3
5

STOP
(^^)b iikanzi: 0
```