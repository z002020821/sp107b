#include <assert.h>
#include "compiler.h"

int E();
void STMT();
void IF();
void BLOCK();
//HackCPU
void HackCUP_F(int , char*);
void HackCPU_E(int , int, char*, int);
void HackCUPU_ASSIGN(char*, int);
void HackCPU_IFNOT(int, int);
void HackCUP_GOTO(int);

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
    f = nextTemp();
    char *item = next();
    emit("#t%d = %s\n", f, item);
    HackCUP_F(f, item);
  }
  return f;
}

// E = F (op E)*
int E() {
  int i1 = F();
  while (isNext("+ - * / & | ! < > =")) {
    char *op = next();
    int i2 = E();
    int i = nextTemp();
    emit("#t%d = t%d %s t%d\n", i, i1, op, i2);
    i1 = i;
    HackCPU_E(i, i1, op, i2);
  }
  return i1;
}

// ASSIGN = id '=' E;
void ASSIGN() {
  char *id = next();
  skip("=");
  int e = E();
  skip(";");
  emit("#%s = t%d\n", id, e);
  HackCUPU_ASSIGN(id, e);
}

// while (E) STMT
void WHILE() {
  int whileBegin = nextLabel();
  int whileEnd = nextLabel();
  emit("#(L%d)\n", whileBegin);
  skip("while");
  skip("(");
  int e = E();
  emit("#ifnot T%d goto L%d\n", e, whileEnd);
  HackCPU_IFNOT(e, whileEnd);
  skip(")");
  STMT();
  emit("#goto L%d\n", whileBegin);
  emit("#(L%d)\n", whileEnd);
  HackCUP_GOTO(whileEnd);
}

void STMT() {
  if (isNext("while"))
    return WHILE();
  else if (isNext("if"))
     IF();
  else if (isNext("{"))
    BLOCK();
  else
    ASSIGN();
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

//IF = if '(' E ')' STMT (else STMT)?
void IF() {
  int elseLabel = nextLabel();
  int endifLabel = nextLabel();
  skip("if");
  skip("(");
  int e = E();
  skip(")");
  emit("#ifnot t%d goto L%d\n", e, elseLabel);
  HackCPU_IFNOT(e, elseLabel);
  STMT();
  emit("#goto L%d\n", endifLabel);
  HackCUP_GOTO(endifLabel);
  if(isNext("else")) {
    emit("#(L%d)\n", elseLabel);
    skip("else");
    STMT();
  }
  emit("#(L%d)\n", endifLabel);
}

void parse() {
  printf("============ parse =============\n");
  tokenIdx = 0;
  PROG();
}

void HackCUP_F(int f, char *item) {
  if (isAlpha(item[0])) {
    emit("\t@%s\n", item);
    emit("\tD = M\n");
  }else {
    emit("\t@%s\n", item);
    emit("\tD = A\n");
  }
  emit("\t@t%d\n", f);
  emit("\tM = D\n");
}

void HackCPU_E(int i, int i1, char *op, int i2) {
  emit("\t@t%d\n", i1);
  emit("\tD = M\n");
  emit("\t@t%d\n", i2);
  emit("\tD = D %s M\n", op);
  emit("\t@t%d\n", i);
  emit("\tM = D\n");
}

void HackCUPU_ASSIGN(char *id, int e) {
  emit("\t@t%d\n", e);
  emit("\tD = M\n");
  emit("\t@%s\n", id);
  emit("\tM = D\n");
}

void HackCPU_IFNOT(int e, int jump) {
  emit("\t@t%d\n", e);
  emit("\tD = M\n");
  emit("\t@L%d\n" , jump);
  emit("\tD ; JEQ\n");
}

void HackCUP_GOTO(int end) {
  emit("\t@L%d\n", end);
  emit("\t0 ; JMP\n");
}