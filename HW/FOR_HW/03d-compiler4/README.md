# Compiler FOR

# [code](https://github.com/AIONLin/sp109b/blob/main/HW/FOR_HW/03d-compiler4/compiler.c)
## FOR 的部分 參考while跟老師的compiler.c、IF  
>define :
```
// #define emit printf

// for (assign E;E) STMT    
// 為了for新增 
int isTempIr = 0;
char tempIr[100000], *tempIrp = tempIr;  // mark掉int tempIrIdx = 0; 
//emit(...)的點點點代表變動參數 
// __VA_ARGS__就是那些變動參數
// if (isTempIr) { --> 如果 isTempIr為1 則:
//                                          sprintf到tempIrp(不會印出來會存起來) -->sprintf(tempIrp, __VA_ARGS__); 
//else 就做printf(__VA_ARGS__);    不為1 才印出來        
// \方便換行觀看
// isTempIr設成1 取完設成0
#define emit(...) ({ \
  if (isTempIr) { \
    sprintf(tempIrp, __VA_ARGS__); \
    tempIrp += strlen(tempIrp);\
  } \
  else { \
    printf(__VA_ARGS__);\
  }\
})   
```
>void FOR ( ) :
```
// FOR =  for (ASSIGN EXP; EXP) STMT
void FOR() {
  int forBegin = nextLabel();
  //int forIng = nextLabel();
  int forEnd = nextLabel();
  emit("(L%d)\n", forBegin);
  skip("for");     // 忽略 for
  skip("(");       // 忽略 (
  ASSIGN();        // 相較 IF 多了這行
  int e2 = E();    // e被 if用走 改用 e2
  emit("if not T%d goto L%d\n", e2, forEnd);  
  skip(";");       // 忽略  ;
  isTempIr = 1;
  int e3 = E();    // 先暫存他 到 emit("%s\n", e3str) 時才讓他印出來
  isTempIr = 0;
  char e3str[10000];
  strcpy(e3str, tempIr) ; // 複製tempIr給e1str
  skip(")"); // 忽略  )      
  STMT();
  emit("%s\n", e3str);  // 放他自由
  emit("goto L%d\n", forBegin);
  emit("(L%d)", forEnd);
}
```

>STMT 新增的部分 : 
```
// STMT = WHILE | BLOCK | ASSIGN | IF |FOR
void STMT() {
  if (isNext("while"))
    return WHILE();
  else if (isNext("if"))
    IF();
  else if (isNext("else")) 
    IF();  
  else if (isNext("for"))  // 這裡要記得加入
    FOR();     
  else if (isNext("{"))
    BLOCK();
  else
    ASSIGN();
}
```

## 語法  

```
PROG = STMTS
BLOCK = { STMTS }
STMTS = STMT*
STMT = WHILE | BLOCK | ASSIGN | IF | FOR
WHILE = while (E) STMT
ASSIGN = id '=' E;
E = F (op E)*
F = (E) | Number | Id
If (EXP) STMT (else STMT)?
FOR =  for (ASSIGN EXP; EXP) STMT
```

## 執行結果

```
PS D:\LIN\CO\109b\sp109b\HW\FOR_HW\03d-compiler4> ./compiler test/for.c
for( i=0 ; i<9 ; i++)
{
    a = a + 2 ;
}

========== lex ==============
token=for
token=(
token=i
token==
token=0
token=;
token=i
token=<
token=9
token=;
token=i
token=++
token=)
token={
token=a
token==
token=a
token=+
token=2
token=;
token=}
========== dump ==============
0:for
1:(
2:i
3:=
4:0
5:;
6:i
7:<
8:9
9:;
10:i
11:++
12:)
13:{
14:a
15:=
16:a
17:+
18:2
19:;
20:}
============ parse =============
(L0)
t0 = 0
i = t0
t1 = i
t2 = 9
t3 = t1 < t2
if not T3 goto L1
t5 = a
t6 = 2
t7 = t5 + t6
a = t7
i = i + 1
t4 = i

goto L0
(L1)
``` 