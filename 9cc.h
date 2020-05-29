#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// main.c
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

// tokenize.c
typedef enum {
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

extern Token *token;
Token *tokenize(char *p);
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();

// parse.c
typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_ASSIGN,
  ND_LVAR, // local variable
  ND_EQ,
  ND_NE,
  ND_LT,
  ND_LE,
  ND_NUM,
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
  int offset; // ローカル変数のベースポインタからのオフセット (kind が ND_LVAR の場合のみ使う)
};

typedef struct LVar LVar;
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};
extern LVar *locals;
extern Node *code[100];
void program();


// codegen.c
void gen(Node *node);