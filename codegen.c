#include "9cc.h"

static int get_label_count(void) {
  static int i = 1;
  return i++;
}

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("代入の左辺値が変数ではありません");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  if (node->kind == ND_RETURN) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }

  if (node->kind == ND_IF) {
    int label_count = get_label_count();
    gen(node->pred);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    if (node->rhs) {
      // else があるケース
      printf("  je .Lelse%d\n", label_count);
      gen(node->lhs);
      printf("  jmp .Lend%d\n", label_count);
      printf(".Lelse%d:\n", label_count);
      gen(node->rhs);
    } else {
      // else がないケース
      printf("  je .Lend%d\n", label_count);
      gen(node->lhs);
    }
    printf(".Lend%d:\n", label_count);
    return;
  }

  if (node->kind == ND_WHILE) {
    int label_count = get_label_count();
    printf(".Lbegin%d:\n", label_count);
    gen(node->pred);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", label_count);
    gen(node->lhs);
    printf("  jmp .Lbegin%d\n", label_count);
    printf(".Lend%d:\n", label_count);
    return;
  }

  if (node->kind == ND_FOR) {
    int label_count = get_label_count();
    if (node->init) {
      gen(node->init);
    }
    printf(".Lbegin%d:\n", label_count);
    if (node->pred) {
      gen(node->pred);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", label_count);
    }
    gen(node->lhs);
    if (node->update) {
      gen(node->update);
    }
    printf("  jmp .Lbegin%d\n", label_count);
    printf(".Lend%d:\n", label_count);
    return;
  }

  switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_LVAR:
      gen_lval(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
  }
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("   add rax, rdi\n");
      break;
    case ND_SUB:
      printf("   sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("   imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("   cqo\n");
      printf("   idiv rdi\n");
      break;
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }

  printf("  push rax\n");
}