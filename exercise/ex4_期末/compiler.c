#include <assert.h>
#include "compiler.h"

int E();
void STMT();
void IF();
void BLOCK();

int tempIdx = 0, labelIdx = 0;

#define nextTemp() (tempIdx++)
#define nextLabel() (labelIdx++)
#define emit printf

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
    f = nextTemp();   //f = tempIdx(t)
    char *item = next();  
    emit("t%d = %s\n", f, item);
  }
  return f;
}

// E = F (op E)*
int E() {
  int i1 = F();
  while (isNext("+ - * / & | ! < > =")) {   //當下一個字元為運算符號時
    char *op = next();   //op = 下一個字元(運算符號)
    int i2 = E();  
    int i = nextTemp();    //i = tempIdx(t)
    emit("t%d = t%d %s t%d\n", i, i1, op, i2);
    i1 = i;
  }
  return i1;
}

// ASSIGN = id '=' E;
void ASSIGN() {
  char *id = next();  //id = 下一個字元
  skip("=");
  int e = E();
  skip(";");
  emit("%s = t%d\n", id, e);
}

// while (E) STMT
void WHILE() {
  int whileBegin = nextLabel();  //設定while開始的回復點  nextLabel是為增加回復點
  int whileEnd = nextLabel();    //設定while結束的回復點
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

//判斷現在為哪種類型
void STMT() {
  if (isNext("while"))  //如果tokenIdx = while，就執行WHILE()
    return WHILE();
  // else if (isNext("if"))
  //   IF();
  else if (isNext("{"))  //如果tokenIdx = {，就執行BLOCK()
    BLOCK();
  else
    ASSIGN();           //以上2種都不是，就執行ASSIGN()
}

void STMTS() {
  while (!isEnd() && !isNext("}")) {
    STMT();
  }
}

// { STMT* }
void BLOCK() {
  skip("{");
  STMTS();
  skip("}");
}

void PROG() {
  STMTS();
}

void parse() {
  printf("============ parse =============\n");
  tokenIdx = 0;
  PROG();
}