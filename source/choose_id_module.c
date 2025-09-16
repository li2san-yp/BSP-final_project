#include "core.h"

void ChooseIDModule()
{
    if (GetKeyAct(enumKey1) == enumKeyPress)
        id = 1; 
    else if (GetKeyAct(enumKey2) == enumKeyPress)
        id = 2;
    else if (GetKeyAct(enumKey3) == enumKeyPress)
        id = 3;
}