#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "pl_reset.h"

int main()
{
    if (reset_pl())
    {
        return 1;
    }
    
    return 0;
}
