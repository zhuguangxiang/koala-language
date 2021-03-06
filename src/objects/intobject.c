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

#include "intobject.h"
#include "stringobject.h"
#include "hashfunc.h"
#include "mem.h"
#include "cache.h"

static Cache Int_Cache;

Object *Integer_New(int64 value)
{
  IntObject *iob = Cache_Take(&Int_Cache);
  Init_Object_Head(iob, &Int_Klass);
  iob->value = value;
  return (Object *)iob;
}

void Integer_Free(Object *ob)
{
  OB_ASSERT_KLASS(ob, Int_Klass);
  Cache_Restore(&Int_Cache, ob);
}

int64 Integer_ToCInt(Object *ob)
{
  OB_ASSERT_KLASS(ob, Int_Klass);
  IntObject *iob = (IntObject *)ob;
  return iob->value;
}

static int integer_equal(Object *v1, Object *v2)
{
  OB_ASSERT_KLASS(v1, Int_Klass);
  OB_ASSERT_KLASS(v2, Int_Klass);
  IntObject *iob1 = (IntObject *)v1;
  IntObject *iob2 = (IntObject *)v2;
  return iob1->value == iob2->value;
}

static uint32 integer_hash(Object *v)
{
  OB_ASSERT_KLASS(v, Int_Klass);
  IntObject *iob = (IntObject *)v;
  return hash_uint32((uint32)iob->value, 32);
}

static Object *integer_tostring(Object *v)
{
  OB_ASSERT_KLASS(v, Int_Klass);
  IntObject *iob = (IntObject *)v;
  char buf[128];
  snprintf(buf, 127, "%lld", iob->value);
  return String_New(buf);
}

static Object *integer_numop_add(Object *v1, Object *v2)
{
  OB_ASSERT_KLASS(v1, Int_Klass);
  OB_ASSERT_KLASS(v2, Int_Klass);
  IntObject *iob1 = (IntObject *)v1;
  IntObject *iob2 = (IntObject *)v2;
  return Integer_New(iob1->value + iob2->value);
}

static NumberOperations integer_numops = {
  .add = integer_numop_add,
};

Klass Int_Klass = {
  OBJECT_HEAD_INIT(&Klass_Klass)
  .name = "Integer",
  .ob_free  = Integer_Free,
  .ob_hash  = integer_hash,
  .ob_equal = integer_equal,
  .ob_str   = integer_tostring,
  .num_ops  = &integer_numops,
};

void Init_Integer_Klass(void)
{
  Init_Cache(&Int_Cache, "IntObject", sizeof(IntObject));
  Init_Klass(&Int_Klass, NULL);
}

void Fini_Integer_Klass(void)
{
  Fini_Cache(&Int_Cache, NULL, NULL);
  Fini_Klass(&Int_Klass);
}
