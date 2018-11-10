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

#include "log.h"
#include "mem.h"
#include "codeobject.h"

static CodeObject *code_new(CodeKind kind, TypeDesc *proto)
{
  CodeObject *code = mm_alloc(sizeof(CodeObject));
  Init_Object_Head(code, &Code_Klass);
  code->kind = kind;
  code->proto = proto;
  return code;
}

Object *KLCode_New(uint8 *codes, int size, TypeDesc *proto)
{
  CodeObject *code = code_new(CODE_KLCODE, proto);
  code->kl.codes = codes;
  code->kl.size = size;
  return (Object *)code;
}

Object *CLCode_New(cfunc cf, TypeDesc *proto)
{
  CodeObject *code = code_new(CODE_CLCODE, proto);
  code->cl = cf;
  return (Object *)code;
}

void CodeObject_Free(Object *ob)
{
  OB_ASSERT_KLASS(ob, Code_Klass);
  CodeObject *code = (CodeObject *)ob;

  TypeDesc_Free(code->proto);
  if (IS_KLCODE(code)) {
    //FIXME
  }

  mm_free(ob);
}

int KLCode_Add_LocVar(Object *ob, char *name, TypeDesc *desc, int pos)
{
  OB_ASSERT_KLASS(ob, Code_Klass);
  CodeObject *code = (CodeObject *)ob;
  assert(IS_KLCODE(code));

  MemberDef *member = Member_Var_New(name, desc, 0);
  member->offset = pos;
  Vector_Append(&code->kl.locvec, member);
  return 0;
}

static Object *__code_disassemble(Object *ob, Object *args)
{
  OB_ASSERT_KLASS(ob, Code_Klass);
  assert(!args);
  CodeObject *cob = (CodeObject *)ob;
  //FIXME
  return NULL;
}

static FuncDef code_funcs[] = {
  {"DisAssemble", NULL, NULL, __code_disassemble},
  {NULL}
};

void Init_Code_Klass(void)
{
  Klass_Add_CFunctions(&Code_Klass, code_funcs);
}

Klass Code_Klass = {
  OBJECT_HEAD_INIT(&Klass_Klass)
  .name = "Code",
  .basesize = sizeof(CodeObject),
};