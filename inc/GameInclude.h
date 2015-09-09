/*
    Funções mais voltadas ao lado da logica do jogo
*/
#ifndef _GAME_INCLUDE_H_
#define _GAME_INCLUDE_H_
#include "GameStructs.h"
#include "CallFunctions.h"
#include "OpenglInclude.h"


void RunGameState(GameState * gs);

// cria e inicializa um game state
GameState * CreateGameState();

Map * CreateMap ();

GameObject * CreateGameObject (int num);

// Destrutores:
void FreeQuad (Quad * ptr, int num);

void FreeMap(Map * ptr);

void FreeButton (Button * ptr, int num);

void FreeText (Text * ptr, int num);

void FreeBackground (Background * ptr);


// Inicializa os recursos do jogo, deveria fazer isso a partir de um arquivo mas por enquanto é tudo contruido manualmente
void BuildResources (Resources * res);

// Encontra e atualiza o texto com nome name para "string"
int UpdateTextStringOV (const char * name, const char * string, Overlay * ov);

int UpdateTextStringBT (const char * name, const char * string, Button * bt);

Texture * GetResourceTexture (const char * name, Resources * res);

Font * GetResourceFont (const char * name, Resources * res);

Overlay * GetResourceOverlay (const char * name, Resources * res);

GameObject * GetResourceObjectCopy (const char * name, Resources * res);

Map * GetResourceMap (const char * mapName, Resources * res);

int GetNextUnlockPos (const char * mapName, Resources * res, int min, int sec);

Background * BuildBackground (Texture * tex);

void BuildMainMenu (GameState * gs);

void BuildMap (GameState * gs, const char * mapName);

void * GetUpdateFunc (int n);

void * GetCollisionFunc (int n);

void * GetAnimationFunc (int n);

void MapCollision (Map * map, GameObject * obj, double time);

ObjectList * ObjectCollision (ObjectList * ptr, GameObject * self, int cont, int numTypes, ...);

void updateGun (GameObject * player, Map * map, double time);

void AddToProfile(const char * mapName);

void LoadUnlockProfile(GameState * gs);

void SaveMap (Map * map);

void LoadMap (GameState * gs);

void FreeGameState(GameState * gs);
#endif
