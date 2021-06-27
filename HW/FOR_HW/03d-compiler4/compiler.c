#include <assert.h>
#include "compiler.h"

int E();
void STMT();
void IF();
void BLOCK();

// int isTempIr = 0;
// char tempIr[100000], *tempIrp = tempIr;
// int tempIrIdx = 0;

int tempIdx = 0, labelIdx = 0;     

#define nextTemp() (tempIdx++)
#define nextLabel() (labelIdx++)
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




int isNext(char *set) {
  char eset[SMAX], etoken[SMAX];
  sprintf(eset, " %s ", set);
  sprintf(etoken, " %s ", tokens[tokenIdx]);
  return (tokenIdx < tokenTop && strstr(eset, etoken) != NULL);
}

int isEnd() {
  return tokenIdx >= tokenTop;
}

char *next() {
  // printf("token[%d]=%s\n", tokenIdx, tokens[tokenIdx]);
  return tokens[tokenIdx++];
}

char *skip(char *set) {
  if (isNext(set)) {
    return next();
  } else {
    printf("skip(%s) got %s fail!\n", set, next());
    assert(0);
  }
}

// F = (E) | Number | Id
int F() {
  int f;
  if (isNext("(")) { // '(' E ')'
    next(); // (
    f = E();
    next(); // )
  } else { // Number | Id
    f = nextTemp();
    char *item = next();
    if (isdigit(*item)) {
      emit("t%d = %s\n", f, item);
    } else {
      if (isNext("++")) {
        next();
        emit("%s = %s + 1\n", item, item);
      }
      emit("t%d = %s\n", f, item);
    }
  }
  return f;
}

// E = F (op E)*
int E() {
  int i1 = F();
  while (isNext("+ - * / & | ! < > = <= >= == !=")) {
    char *op = next();
    int i2 = E();
    int i = nextTemp();
    emit("t%d = t%d %s t%d\n", i, i1, op, i2);
    i1 = i;
  }
  return i1;
}

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

// ASSIGN = id '=' E;
void ASSIGN() {
  char *id = next();
  skip("=");
  int e = E();
  skip(";");
  emit("%s = t%d\n", id, e);
}

// WHILE = while (E) STMT
void WHILE() {
  int whileBegin = nextLabel();
  int whileEnd = nextLabel();
  emit("(L%d)\n", whileBegin);
  skip("while");
  skip("(");
  int e = E();
  emit("if not T%d goto L%d\n", e, whileEnd);
  skip(")");
  STMT();
  emit("goto L%d\n", whileBegin);
  emit("(L%d)\n", whileEnd);
}

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

// STMTS = STMT*
void STMTS() {
  while (!isEnd() && !isNext("}")) {
    STMT();
  }
}

// BLOCK = { STMTS }
void BLOCK() {
  skip("{");
  STMTS();
  skip("}");
}

// PROG = STMTS
void PROG() {
  STMTS();
}

void parse() {
  printf("============ parse =============\n");
  tokenIdx = 0;
  PROG();
}