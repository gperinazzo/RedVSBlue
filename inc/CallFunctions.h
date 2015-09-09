// Funções que serão armazenadas nas estruturas para serem chamadas quando necessarias
// é aqui que quase tudo acontece realmente
// Funções de input, render, eventos on click, etc.

#ifndef _CALL_FUNCTIONS_H_
#define _CALL_FUNCTIONS_H_
#include "GameStructs.h"

void InputMainMenu();
void OnClickMainMenuButton(int * p, void * null);
void OnClickDemoButton(int * p, void * null);
void OnClickInGameMenu (int * p, void * null);
void OnClickMapSelector(int * p, void * null);

void UpdateMap();
void RenderMap();

void UpdatePlayer(void * ptr, double time);
void UpdateSimpleEnemy (void * ptr, double time);

void UpdatePistol (void * ptr, double time);
void UpdateDualPistol (void * ptr, double time);
void UpdateShotgun (void * ptr, double time);
void UpdateRifle (void * ptr, double time);
void UpdateBullet(void * ptr, double time);

void * CollisionPlayer (void * ptr);
void * CollisionBullet (void * ptr);



#endif
