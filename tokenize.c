#include "9cc.h"

Token *token;

bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
    return false;
  }
  token = token->next;
  return true;
}

bool consume_tokenkind(TokenKind tk) {
    if (token->kind != tk) {
      return false;
    }
    token = token->next;
    return true;
}

Token *consume_ident(void) {
    if (token->kind != TK_IDENT) {
        return NULL;
    }
    Token *ret = token;
    token = token->next;
    return ret;
}

void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
    error_at(token->str, "\"%s\"ではありません", op);
  }
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM) error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

int is_alpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

int is_alnum(char c) {
  return is_alpha(c) || ('0' <= c && c <= '9');
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (strchr("+-*/()<>;=", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
        cur = new_token(TK_RETURN, cur, p, 6);
        p += 6;
        continue;
    }

    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
        cur = new_token(TK_IF, cur, p, 2);
        p += 2;
        continue;
    }

    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
        cur = new_token(TK_ELSE, cur, p, 4);
        p += 4;
        continue;
    }

    if (is_alpha(*p)) {
      char *q = p;
      while (q && is_alnum(*q)) {
        q++;
      }
      int len = q - p;
      cur = new_token(TK_IDENT, cur, p, len);
      p += len;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}