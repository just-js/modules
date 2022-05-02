#ifndef JUST_SQLITE_H
#define JUST_SQLITE_H

#include "just.h"
#include <sqlite3.h>
//#include <sqlite3ext.h> /* Do not use <sqlite3.h>! */

namespace just {
// https://zetcode.com/db/sqlitec/
// https://github.com/sql-js/sql.js/blob/master/src/exported_functions.json
namespace sqlite_vfs {

void Create(const FunctionCallbackInfo<Value> &args);
void LoadExtension(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_sqlite_vfs() {
		return (void*)just::sqlite_vfs::Init;
	}

int sqlite3_sqlite_vfs_init(
  sqlite3 *db, 
  char **pzErrMsg, 
  const sqlite3_api_routines *pApi
){
  int rc = SQLITE_OK;
	fprintf(stderr, "extension loaded\n");
  //SQLITE_EXTENSION_INIT2(pApi);
  /* Insert here calls to
  **     sqlite3_create_function_v2(),
  **     sqlite3_create_collation_v2(),
  **     sqlite3_create_module_v2(), and/or
  **     sqlite3_vfs_register()
  ** to register the new features that your extension adds.
  */
  return rc;
}

}

#endif
