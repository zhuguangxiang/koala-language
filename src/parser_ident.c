/*
 * Copyright (c) 2018 James, https://github.com/zhuguangxiang
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "parser.h"
#include "log.h"

LOGGER(0)

static void code_current_module_class(ParserState *ps, void *arg)
{
  ParserUnit *u = ps->u;
  IdentExpr *exp = arg;
  Symbol *sym = exp->sym;
  TypeDesc *desc = exp->desc;
  ExprCtx ctx = exp->ctx;
  Expr *right = exp->right;

  /* Id, in module/class, is in variable/field declaration's expr */
  assert(ctx == EXPR_LOAD);
  if (sym->kind == SYM_CONST || sym->kind == SYM_VAR) {
    Log_Debug("Id '%s' is const/var", sym->name);
    if (desc->kind == TYPE_PROTO && Expr_Is_Call(right)) {
      /* Id is const/var, but it's a func reference, call function */
      Log_Debug("call '%s' function(var)", sym->name);
      int argc = Vector_Size(((CallExpr *)right)->args);
      CODE_CALL(u->block, sym->name, argc);
    } else {
      /* load variable */
      Log_Debug("load '%s' variable", sym->name);
      CODE_LOAD_FIELD(u->block, sym->name);
    }
  } else {
    assert(sym->kind == SYM_FUNC);
    Log_Debug("Id '%s' is function", sym->name);
    if (Expr_Is_Call(right)) {
      /* call function */
      Log_Debug("call '%s' function", sym->name);
      int argc = Vector_Size(((CallExpr *)right)->args);
      CODE_CALL(u->block, sym->name, argc);
    } else {
      /* load function */
      Log_Debug("load '%s' function", sym->name);
      CODE_LOAD_FIELD(u->block, sym->name);
    }
  }
}

static void code_current_function_block_closure(ParserState *ps, void *arg)
{
  ParserUnit *u = ps->u;
  IdentExpr *exp = arg;
  Symbol *sym = exp->sym;
  TypeDesc *desc = exp->desc;
  ExprCtx ctx = exp->ctx;
  Expr *right = exp->right;

  /*
   * expr, in function/block/closure scope, is local variable or parameter
   * it's context is load or store
   */
  assert(sym->kind == SYM_VAR);
  VarSymbol *varSym = (VarSymbol *)sym;
  Log_Debug("Id '%s' is variable", varSym->name);
  if (desc->kind == TYPE_PROTO && Expr_Is_Call(right)) {
    /* Id is const/var, but it's a func reference, call function */
    Log_Debug("call '%s' function(var)", sym->name);
    assert(ctx == EXPR_LOAD);
    int argc = Vector_Size(((CallExpr *)right)->args);
    CODE_CALL(u->block, sym->name, argc);
  } else {
    /* load variable */
    assert(ctx == EXPR_LOAD || ctx == EXPR_STORE);
    if (ctx == EXPR_LOAD) {
      Log_Debug("load '%s' variable", varSym->name);
      CODE_LOAD(u->block, varSym->index);
    } else {
      Log_Debug("store '%s variable", varSym->name);
      CODE_STORE(u->block, varSym->index);
    }
  }
}

static void code_up_class(ParserState *ps, void *arg)
{
  ParserUnit *u = ps->u;
  IdentExpr *exp = arg;
  Symbol *sym = exp->sym;
  TypeDesc *desc = exp->desc;
  ExprCtx ctx = exp->ctx;
  Expr *right = exp->right;
  ParserUnit *uu = exp->scope;

  /*
   * up scope MUST be module
   * Id, in class, is in field declaration's expr
   */
  Log_Debug("Id '%s' is in module", sym->name);
  assert(uu->scope == SCOPE_MODULE);
  assert(ctx == EXPR_LOAD);
  if (sym->kind == SYM_CONST || sym->kind == SYM_VAR) {
    Log_Debug("Id '%s' is const/var", sym->name);
    if (desc->kind == TYPE_PROTO && Expr_Is_Call(right)) {
      /* Id is const/var, but it's a func reference, call function */
      Log_Debug("call '%s' function(var)", sym->name);
      int argc = Vector_Size(((CallExpr *)right)->args);
      //FIXME:
      //CODE_PKG_CALL(u->block, sym->name, argc);
    } else {
      /* load variable */
      Log_Debug("load '%s' variable", sym->name);
      //FIXME:
      //CODE_PKG_GETFIELD(u->block, sym->name);
    }
  } else {
    assert(sym->kind == SYM_FUNC);
    Log_Debug("Id '%s' is function", sym->name);
    if (Expr_Is_Call(right)) {
      /* call function */
      Log_Debug("call '%s' function", sym->name);
      int argc = Vector_Size(((CallExpr *)right)->args);
      //FIXME:
      //CODE_PKG_CALL(u->block, sym->name, argc);
    } else {
      /* load function */
      Log_Debug("load '%s' function", sym->name);
      //FIXME:
      //CODE_PKG_GETFIELD(u->block, sym->name);
    }
  }
}

static void code_up_function(ParserState *ps, void *arg)
{
  ParserUnit *u = ps->u;
  IdentExpr *exp = arg;
  Symbol *sym = exp->sym;
  TypeDesc *desc = exp->desc;
  ExprCtx ctx = exp->ctx;
  Expr *right = exp->right;
}

static void code_up_block(ParserState *ps, void *arg)
{
  ParserUnit *u = ps->u;
  IdentExpr *exp = arg;
  Symbol *sym = exp->sym;
  TypeDesc *desc = exp->desc;
  ExprCtx ctx = exp->ctx;
  Expr *right = exp->right;
}

static void code_up_closure(ParserState *ps, void *arg)
{
  ParserUnit *u = ps->u;
  IdentExpr *exp = arg;
  Symbol *sym = exp->sym;
  TypeDesc *desc = exp->desc;
  ExprCtx ctx = exp->ctx;
  Expr *right = exp->right;
}

/* identifier is found in current scope */
static CodeGenerator current_codes[] = {
  {SCOPE_MODULE,   code_current_module_class},
  {SCOPE_CLASS,    code_current_module_class},
  {SCOPE_FUNCTION, code_current_function_block_closure},
  {SCOPE_BLOCK,    code_current_function_block_closure},
  {SCOPE_CLOSURE,  code_current_function_block_closure},
};

/*
 * identifier is found in up scope
 * It's impossible that ID is in SCOPE_MODULE, is it right?
 */
static CodeGenerator up_codes[] = {
  {SCOPE_MODULE,   NULL},
  {SCOPE_CLASS,    code_up_class},
  {SCOPE_FUNCTION, code_up_function},
  {SCOPE_BLOCK,    code_up_block},
  {SCOPE_CLOSURE,  code_up_closure},
};

#define code_generate(codes_array, _scope, ps, arg) \
({ \
  CodeGenerator *gen = codes_array; \
  for (int i = 0; i < nr_elts(codes_array); i++) { \
    if (_scope == gen->scope) { \
      if (gen->code != NULL) \
        gen->code(ps, arg); \
      break; \
    } \
    gen++; \
  } \
})

/*
  a.b.c.attribute
  a.b.c()
  a.b.c[10]
  a.b.c[1:10]
  leftmost identifier is variable or imported external package name
 */
void Parse_Ident_Expr(ParserState *ps, Expr *exp)
{
  assert(exp->kind == ID_KIND);
  ParserUnit *u = ps->u;
  IdentExpr *idExp = (IdentExpr *)exp;
  char *name = idExp->name;
  Symbol *sym;
  int depth;

  assert(idExp->sym == NULL);
  assert(idExp->desc == NULL);

  /* find ident from current scope */
  sym = STable_Get(u->stbl, name);
  if (sym != NULL) {
    depth = ps->depth;
    Log_Debug("find symbol '%s' in local scope-%d(%s)",
              name, depth, scope_name(u));
    sym->used++;
    idExp->sym = sym;
    idExp->desc = sym->desc;
    TYPE_INCREF(idExp->desc);
    idExp->where = CURRENT_SCOPE;
    idExp->scope = u;
    return;
  }

  /* find ident from up scope */
  ParserUnit *uu;
  depth = ps->depth;
  list_for_each_entry(uu, &ps->ustack, link) {
    depth -= 1;
    sym = STable_Get(uu->stbl, name);
    if (sym != NULL) {
      Log_Debug("find symbol '%s' in up scope-%d(%s)",
                name, depth, scope_name(uu));
      sym->used++;
      idExp->sym = sym;
      idExp->desc = sym->desc;
      TYPE_INCREF(idExp->desc);
      idExp->where = UP_SCOPE;
      idExp->scope = uu;
      return;
    }
  }

  /* find ident from external scope (imported) */
  sym = STable_Get(ps->extstbl, name);
  if (sym != NULL) {
    Log_Debug("find symbol '%s' as imported name", name);
    sym->used++;
    idExp->sym = sym;
    idExp->desc = sym->desc;
    TYPE_INCREF(idExp->desc);
    idExp->where = CURRENT_SCOPE;
    idExp->scope = u;
    return;
  }

  /* find ident from external scope (imported dot) */
  sym = STable_Get(ps->extdots, name);
  if (sym != NULL) {
    Log_Debug("find symbol '%s' in imported symbols", name);
    sym->used++;
    RefSymbol *refSym = (RefSymbol *)sym;
    refSym->sym->used++;
    idExp->sym = refSym->sym;
    idExp->desc = refSym->sym->desc;
    TYPE_INCREF(idExp->desc);
    idExp->where = CURRENT_SCOPE;
    idExp->scope = u;
    return;
  }

  Syntax_Error(ps, &exp->pos, "cannot find symbol '%s'", name);
}

void Code_Ident_Expr(ParserState *ps, Expr *exp)
{
  assert(exp->kind == ID_KIND);
  assert(exp->desc != NULL && exp->sym != NULL);
  ParserUnit *u = ps->u;
  IdentExpr *idExp = (IdentExpr *)exp;
  if (idExp->where == CURRENT_SCOPE) {
    /* current scope */
    code_generate(current_codes, u->scope, ps, idExp);
  } else if (idExp->where == UP_SCOPE) {
    /* up scope */
    code_generate(up_codes, u->scope, ps, idExp);
  } else {
    assert(0);
  }
}
