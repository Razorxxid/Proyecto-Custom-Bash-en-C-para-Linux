#ifndef BUILTIN_H
#define BUILTIN_H

#include <stdbool.h>
#include "command.h"

typedef enum {
    BUILTIN_EXIT,
    BUILTIN_CHDIR,
    BUILTIN_UNKNOWN = -1
} cmd_id;

cmd_id builtin_index (const pipeline pipe);
/* Este caso partcular es util para cortar el ciclo principal */
bool builtin_is_exit (const pipeline pipe);


bool builtin_is_cd(const pipeline pipe);

void builtin_run (const pipeline pipe);

#endif
