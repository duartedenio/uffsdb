#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifndef FMACROS
   #include "macros.h"
#endif
#ifndef FTYPES
   #include "types.h"
#endif
#ifndef FMISC
   #include "misc.h"
#endif
#ifndef FDICTIONARY
   #include "dictionary.h"
#endif
#ifndef FSQLCOMMANDS
   #include "sqlcommands.h"
#endif
#ifndef FDATABASE
   #include "database.h"
#endif

#include "interface/y.tab.h"

db_connected connected;

int main(){
    dbInit(NULL);
    printf("uffsdb (16.2).\nType \"help\" for help.\n\n");
    interface();
    return 0;
}
