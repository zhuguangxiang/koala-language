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

#ifndef _KOALA_TUPLEOBJECT_H_
#define _KOALA_TUPLEOBJECT_H_

#include "object.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tupleobject {
  OBJECT_HEAD
  int avail;
  int size;
  Object *items[0];
} TupleObject;

extern Klass Tuple_Klass;
void Init_Tuple_Klass(void);
void Fini_Tuple_Klass(void);
Object *Tuple_New(int size);
void Tuple_Free(Object *ob);
Object *Tuple_ToString(Object *ob);
Object *Tuple_Get(Object *ob, int index);
static inline int Tuple_Size(Object *ob)
{
  if (ob == NULL)
    return 0;
  OB_ASSERT_KLASS(ob, Tuple_Klass);
  TupleObject *tuple = (TupleObject *)ob;
  return tuple->size;
}
int __Tuple_Set(TupleObject *tuple, int index, Object *val);
static inline int Tuple_Set(Object *ob, int index, Object *val)
{
  OB_ASSERT_KLASS(ob, Tuple_Klass);
  TupleObject *tuple = (TupleObject *)ob;
  return __Tuple_Set(tuple, index, val);
}
static inline int Tuple_Append(Object *ob, Object *val)
{
  OB_ASSERT_KLASS(ob, Tuple_Klass);
  TupleObject *tuple = (TupleObject *)ob;
  return __Tuple_Set(tuple, tuple->avail++, val);
}

#ifdef __cplusplus
}
#endif
#endif /* _KOALA_TUPLEOBJECT_H_ */
