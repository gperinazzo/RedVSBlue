#include "GameInclude.h"
#include "OpenglInclude.h"
#include <stdio.h>

int main( void )
{
    InitAll();
    curGS = CreateGameState();
    BuildResources (curGS->res);
    LoadUnlockProfile(curGS);
    BuildMainMenu(curGS);
    RunGameState(curGS);
    FreeGameState(curGS);
    return 0;
}



