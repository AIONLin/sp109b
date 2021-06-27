# Compiler if

## IF 的部分 參考while跟老師的compiler.c
```
// if (EXP) STMT (else STMT)?   // 參考while跟老師的compiler.c
void IF() {
  int ifBegin = nextLabel();
  int ifIng = nextLabel();
  int ifEnd = nextLabel();
  emit("(L%d)\n", ifBegin);
  skip("if");     // 忽略 if
  skip("(");      // 忽略  (
  int e = E();
  emit("if not T%d goto L%d\n", e, ifIng);  
  skip(")");      // 忽略  )
  STMT();
  emit("goto L%d\n", ifEnd);  // 若判斷式成立
  emit("(L%d)\n", ifIng);
  if (isNext("else")) {     // 加入了判斷 else 
    skip("else");           // 忽略 else
    STMT();
    emit("(L%d)\n", ifEnd);
  }
}
```
## 語法

```
PROG = STMTS
BLOCK = { STMTS }
STMTS = STMT*
STMT = WHILE | BLOCK | ASSIGN
WHILE = while (E) STMT
ASSIGN = id '=' E;
E = F (op E)*
F = (E) | Number | Id
```

## 執行結果

```
PS D:\LIN\CO\109b\sp109b\HW\IF_HW> make clean
rm -f *.o *.exe
PS D:\LIN\CO\109b\sp109b\HW\IF_HW> make
gcc -std=c99 -O0 lexer.c compiler.c main.c -o compiler
PS D:\LIN\CO\109b\sp109b\HW\IF_HW> ./compiler test/addelseif.c
if (a>3)     
{
  t=1;       
} 
else if(a==3)
{
  t=2;
}
else
{
  t=3;
}


========== lex ==============
token=if
token=(
token=a
token=>
token=3
token=)
token={
token=t
token==
token=1
token=;
token=}
token=else
token=if
token=(
token=a
token===
token=3
token=)
token={
token=t
token==
token=2
token=;
token=}
token=else
token={
token=t
token==
token=3
token=;
token=}
========== dump ==============
0:if
1:(
2:a
3:>
4:3
5:)
6:{
7:t
8:=
9:1
10:;
11:}
12:else
13:if
14:(
15:a
16:==
17:3
18:)
19:{
20:t
21:=
22:2
23:;
24:}
25:else
26:{
27:t
28:=
29:3
30:;
31:}
============ parse =============
(L0)
t0 = a
t1 = 3
t2 = t0 > t1
if not T2 goto L1
t3 = 1
t = t3
goto L2
(L1)
(L3)
t4 = a
t5 = 3
t6 = t4 == t5
if not T6 goto L4
t7 = 2
t = t7
goto L5
(L4)
t8 = 3
t = t8
(L5)
(L2)
``` 