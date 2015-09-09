// DEFINE ALL THE STRUCTS

#ifndef _GAME_STRUCTS_H_
#define _GAME_STRUCTS_H_

#include <GL/glew.h>
#include <GL/glfw.h>
#include <stdlib.h>



// largura e altura da janela padrao
// poderia fazer isso ser dinamico mas para facilitar vou deixar fixo
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


typedef struct
{
    float x; float y;
    float width; float height;
} Coord;

typedef struct
{
   GLuint Buffer;
   char * name;
   char * file;
   GLFWimage * Data;
} Texture;

typedef struct
{
    int isSupported;
    Coord pos;
    float xoffset;
    float yoffset;
    float xadvance;
} CharInfo;

typedef struct
{
    char * name;
    int isSupported[256];
    float lineHeight;
    float base;
    float scaleW;
    float scaleH;
    Texture * fontTexture;
    GLuint listBase;
} Font;

typedef struct
{
    char * name;
    char * string;
    Coord pos;
    float scale;
    float R, G, B;
    Font *font;
    int hidden;
} Text;

typedef struct
{
    Coord pos;
    Coord texCoord;
    Texture * tex;
    float color[16];
    int isColored;
    int isTextured;
} Quad;

// define um botão
typedef struct
{
    char * name;
    Coord pos;  // relativo a posição do overlay
    void (*onClick) (int *, void *);
    int Parameters[3];
    int hidden;
    Quad * quadList;
    int numQuads;
    Text * textList;
    int numTexts;
} Button;

typedef struct
{
    char * name;
    Coord pos;
    float z;
    int hidden;
    Button * buttonList;
    int numButtons;
    Quad * quadList;
    int numQuads;
    Text * textList;
    int numTexts;
    void (*onButtonPressed) (int, void*);
} Overlay;

typedef struct
{
    Quad * quadList;
    int numQuads;
} Background;

typedef struct
{
    char * name;
    int type;
    Coord pos; // onde esse objeto está?
    Quad * quadList; // como eu desenho esse objeto?
    int numQuads;
    int hidden;
    float timeCounter;
    void (*animationFunction) (float, float, void *);
    float direction[2];
    void (*updateObject)(void *, double);
    void * (*collisionFunction) (void *);
    int life;
    float speed;
} GameObject;

typedef struct str0
{
    GameObject * obj;
    struct str0 * next;
    struct str0 * prev;
} ObjectList;

typedef struct
{
    char * name;
    char * string;
    Background * back;
    ObjectList * objectListFirst;
    ObjectList * objectListLast;
    char ** colisionMatrix;
    int maxX, maxY;
    char  ** monsterTypeList;
    int numMonsterTypes;
    Coord startPos;
    Coord dropPos;
    int numMonsters;
    float counter;
    float time;
    int life;
    int ammo;
    int lock;
    GameObject * gun;
    GameObject * gunList;
    int end;
    int numGuns;
    int hidden;
} Map;

typedef struct
{
    int type; // 1 para armas, 2 para mapas
    char * mapName; // nome do mapa que libera
    char * string; // uma string pra ser exibida (nome da arma/mapa)
    int min, sec; // tempo necessario
    char * unlockName; // nome do que é liberado
}  Unlock;

typedef struct
{
    Texture * textureList;
    int numTextures;
    Font * fontList;
    int numFonts;
    Overlay * overlayList;
    int numOverlays;
    GameObject * objectList;
    int numObjects;
    Map * mapList;
    int numMaps;
    Unlock * unlockList;
    int numUnlocks;
} Resources;

typedef struct
{
    void (*renderFunction)();
    void (*inputFunction)();
    Overlay ** overlayList;
    int numOverlays;
    Map * map;
    Resources * res;
    int keybuffer[10];
    int end;
} GameState;

// Eu sei que variaveis globais são consideradas pessimas praticas de programação,
// mas eu tenho que passar isso pra quase todas as funções de update/render
GameState * curGS;


#endif
