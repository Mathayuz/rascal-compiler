#ifndef RASCAL_MEPA_H
#define RASCAL_MEPA_H

#include "rascal_ast.h"
#include "symbol_table.h"
#include <stdio.h>

// Armazena o contexto da geração de código
typedef struct CodeGenContext {
    FILE *mepafile;
    int label_count;
    int current_level;
} CodeGenContext;

// MEPA Code Generation Functions
void generate_code(Program *root, const char *filename);

void gen_program(Program* p, CodeGenContext* ctx);
void gen_block(Block* b, CodeGenContext* ctx);

void gen_var_decl(VarDeclaration* vd, CodeGenContext* ctx);
void gen_subrot_decl(SubRotDeclaration* sd, CodeGenContext* ctx);

void gen_command_list(Command* c, CodeGenContext* ctx);
void gen_command(Command* c, CodeGenContext* ctx);
void gen_assign(Command* c, CodeGenContext* ctx);
void gen_proc_call(Command* c, CodeGenContext* ctx);
void gen_if(Command* c, CodeGenContext* ctx);
void gen_while(Command* c, CodeGenContext* ctx);
void gen_read(Command* c, CodeGenContext* ctx);
void gen_write(Command* c, CodeGenContext* ctx);

void gen_expression(Expression* e, CodeGenContext* ctx);
void gen_binary_expr(Expression* e, CodeGenContext* ctx);
void gen_unary_expr(Expression* e, CodeGenContext* ctx);
void gen_literal_int(Expression* e, CodeGenContext* ctx);
void gen_literal_bool(Expression* e, CodeGenContext* ctx);
void gen_var_usage(Expression* e, CodeGenContext* ctx);
void gen_func_call(Expression* e, CodeGenContext* ctx);

// Auxiliary Write Functions
int get_new_label(CodeGenContext* ctx);
void emit_label(CodeGenContext* ctx, int label);
void emit_no_arg(CodeGenContext* ctx, const char* op);
void emit_arg_int(CodeGenContext* ctx, const char* op, int arg);
void emit_arg_2_int(CodeGenContext* ctx, const char* op, int arg1, int arg2);
void emit_arg_label(CodeGenContext* ctx, const char* op, int label);

#endif