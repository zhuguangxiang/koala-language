
#ifndef _KOALA_MODULEOBJECT_H_
#define _KOALA_MODULEOBJECT_H_

#include "object.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleobject {
  OBJECT_HEAD
  char *name;
  HashTable *stable;
  ItemTable *itable;
  int avail_index;
  int size;
  TValue locals[0];
} ModuleObject;

/* Exported symbols */
extern Klass Module_Klass;
void Init_Module_Klass(void);
Object *Module_New(char *name, int nr_locals);
int Module_Add_Var(Object *ob, char *name, char *desc, uint8 access);
int Module_Add_Func(Object *ob, char *name, char *rdesc[], int rsz,
                    char *pdesc[], int psz, uint8 access, Object *method);
int Module_Add_Class(Object *ob, Klass *klazz, uint8 access);
int Module_Add_Intf(Object *ob, Klass *klazz, uint8 access);
int Module_Get_VarValue(Object *ob, char *name, TValue *val);
int Module_Get_FuncValue(Object *ob, char *name, Object **func);

Object *Load_Module(char *path);
void Module_Display(Object *ob);

#ifdef __cplusplus
}
#endif
#endif /* _KOALA_MODULEOBJECT_H_ */
