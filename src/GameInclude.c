#include "GameInclude.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
// auxiliar para inicializar e copiar uma string
char * _String (const char * string)
{
    int len = strlen(string);
    char * ptr = malloc (sizeof (char) * ( len + 1));
    strcpy (ptr, string);
    return ptr;
}


void RunGameState(GameState * gs)
{
    while (gs->end == 0 && glfwGetWindowParam( GLFW_OPENED ))
    {
        if (gs->inputFunction != NULL) gs->inputFunction (gs);
        if (gs->renderFunction != NULL) gs->renderFunction (gs);
    }
}

GameObject * CreateGameObject (int num)
{
    int i;
    GameObject * ptr = malloc (sizeof (GameObject) * num);
    for (i = 0; i < num; i++)
    {
        ptr[i].name = NULL;
        ptr[i].pos.x = 0; // onde esse objeto está?
        ptr[i].pos.y = 0;
        ptr[i].direction[0] = 0;
        ptr[i].direction[1] = 0;
        ptr[i].quadList = NULL; // como eu desenho esse objeto?
        ptr[i].numQuads = 0;
        ptr[i].hidden = 1;
        ptr[i].timeCounter = 0.0f;
        ptr[i].animationFunction = NULL;
        ptr[i].updateObject = NULL;
    }
    return ptr;
}

Map * CreateMap ()
{
    Map * ptr = malloc (sizeof (Map));
    ptr->back = NULL;
    ptr->objectListFirst = NULL;
    ptr->objectListLast = NULL;
    ptr->numGuns = 0;
    ptr->gunList = NULL;
    ptr->gun = NULL;
    ptr->maxX = 0;
    ptr->maxY = 0;
    ptr->monsterTypeList = NULL;
    ptr->numMonsterTypes = 0;
    return ptr;
}

GameState * CreateGameState ()
{
    GameState * gs = malloc (sizeof (GameState));
    gs->renderFunction = NULL;
    gs->inputFunction = NULL;
    gs->overlayList = NULL;
    gs->map = NULL;
    gs->end = 0;
    gs->res = malloc (sizeof(Resources));
    gs->res->textureList  = NULL;
    gs->res->fontList     = NULL;
    gs->res->overlayList  = NULL;
    gs->res->unlockList   = NULL;
    gs->res->mapList      = NULL;
    gs->res->objectList   = NULL;
    gs->res->numTextures  = 0;
    gs->res->numFonts     = 0;
    gs->res->numOverlays  = 0;
    gs->res->numObjects   = 0;
    gs->res->numMaps      = 0;
    gs->res->numUnlocks   = 0;
    return gs;
}



void BuildResources (Resources * res)
{
    char  buffer1[100], buffer2[200];
    int i;
    FILE * mFile;
    mFile = fopen("Data/resources.res", "r");
    if (mFile == NULL) exit(1);
    while (fscanf (mFile, "%s %s", buffer1, buffer2) != EOF)
    {
        if (strcmp(buffer1, "texture") == 0)
        {
            FILE * aFile = fopen(buffer2, "r");
            if (aFile == NULL) continue;
            if (res->numTextures == 0) res->textureList = NULL;
            while (fscanf (aFile, "%s %s", buffer1, buffer2) != EOF)
            {
                res->numTextures++;
                res->textureList = realloc (res->textureList, sizeof(Texture) * res->numTextures);
                res->textureList[res->numTextures - 1].name = _String(buffer1);
                res->textureList[res->numTextures - 1].file = _String(buffer2);
                res->textureList[res->numTextures - 1].Data = NULL;
                res->textureList[res->numTextures - 1].Buffer = 0;
            }
            fclose(aFile);
        }

        else if (strcmp(buffer1, "font") == 0)
        {
            FILE * aFile = fopen(buffer2, "r");
            if (aFile == NULL) continue;
            if (res->numFonts == 0) res->fontList = NULL;
            char  buffer3[200];
            while (fscanf (aFile, "%s %s %s", buffer1, buffer2, buffer3) != EOF)
            {
                res->numFonts++;
                res->fontList = realloc (res->fontList, sizeof(Font) * res->numFonts);
                res->fontList[res->numFonts - 1].name = _String(buffer1);
                LoadFont(buffer2, buffer3, &res->fontList[res->numFonts - 1]);
                UnbufferTexture(res->fontList[res->numFonts - 1].fontTexture);
            }
            fclose(aFile);
        }

        else if (strcmp (buffer1, "object") == 0)
        {
            FILE * aFile = fopen(buffer2, "r");
            if (aFile == NULL) continue;
            if (res->numObjects == 0) res->objectList = NULL;
            Quad q;
            q.isTextured = 1;
            q.isColored = 0;
            int upd, col, anim, type, life, hidden;
            float  speed;

            while (fscanf (aFile, "name=%s%*[ ]type=%d%*[ ]life=%d%*[ ]speed=%f%*[ ]qx=%f%*[ ]qy=%f%*[ ]width=%f%*[ ]height=%f%*[ ]texture=%s%*[ ]texx=%f%*[ ]texy=%f%*[ ]texw=%f%*[ ]texh=%f%*[ ]upd=%d%*[ ]col=%d%*[ ]anim=%d%*[ ]hidden=%d\n",
                            buffer1, &type, &life, &speed, &q.pos.x, &q.pos.y, &q.pos.width, &q.pos.height, buffer2, &q.texCoord.x, &q.texCoord.y, &q.texCoord.width, &q.texCoord.height, &upd, &col, &anim, &hidden) != EOF)
            {


                res->numObjects++;
                res->objectList = realloc (res->objectList, sizeof(GameObject) * res->numObjects);

                res->objectList[res->numObjects - 1].pos.x = 0; // onde esse objeto está?
                res->objectList[res->numObjects - 1].pos.y = 0;
                res->objectList[res->numObjects - 1].direction[0] = 0.0f;
                res->objectList[res->numObjects - 1].direction[1] = 0.0f;
                res->objectList[res->numObjects - 1].hidden = hidden;
                res->objectList[res->numObjects - 1].timeCounter = 0.0f;

                res->objectList[res->numObjects - 1].name = _String(buffer1);
                res->objectList[res->numObjects - 1].type = type;
                res->objectList[res->numObjects - 1].numQuads = 1;
                res->objectList[res->numObjects - 1].quadList = malloc (sizeof(Quad));
                memcpy(res->objectList[res->numObjects - 1].quadList, &q, sizeof(Quad));
                res->objectList[res->numObjects - 1].quadList[0].tex = GetResourceTexture(buffer2, res);
                res->objectList[res->numObjects - 1].animationFunction = GetAnimationFunc(anim);
                res->objectList[res->numObjects - 1].updateObject = GetUpdateFunc(upd);
                res->objectList[res->numObjects - 1].collisionFunction = GetCollisionFunc(col);
                res->objectList[res->numObjects - 1].life = life;
                res->objectList[res->numObjects - 1].speed = speed;
            }
            fclose(aFile);
        }
        else if (strcmp (buffer1, "map") == 0)
        {
            FILE * aFile = fopen(buffer2, "r");
            char  buffer3[100];
            char num[3];
            num[2] = 0;
            if (aFile == NULL) continue;
            if (res->numMaps == 0) res->mapList = NULL;
            res->numMaps++;
            res->mapList = realloc (res->mapList, sizeof(Map) * res->numMaps);
            #define POS res[0].numMaps - 1

            res->mapList[POS].objectListFirst = NULL;
            res->mapList[POS].objectListLast  = NULL;
            res->mapList[POS].colisionMatrix  = NULL;
            res->mapList[POS].monsterTypeList = NULL;
            res->mapList[POS].numMonsters     = 0;
            res->mapList[POS].time            = 0.0f;
            res->mapList[POS].life            = 10;
            res->mapList[POS].gun             = NULL;

            fscanf (aFile, "name=%s%*[ ]back=%s%*[ ]x=%d%*[ ]y=%d%*[ ]monsters=%d%*[ ]px=%f%*[ ]py=%f%*[ ]dx=%f%*[ ]dy=%f%*[ ]hidden=%d\n",
                    buffer3, buffer1, &res->mapList[POS].maxX, &res->mapList[POS].maxY, &res->mapList[POS].numMonsterTypes, &res->mapList[POS].startPos.x,
                    &res->mapList[POS].startPos.y, &res->mapList[POS].dropPos.x, &res->mapList[POS].dropPos.y, &res->mapList[POS].hidden  );
            res->mapList[POS].name = _String(buffer3);
            res->mapList[POS].back = BuildBackground(GetResourceTexture(buffer1, res ));
            res->mapList[POS].monsterTypeList = malloc (sizeof (char*) * res->mapList[POS].numMonsterTypes);
            fread(num, 1, 2, aFile );
            res->mapList[POS].string = malloc(sizeof(char) * (atoi(num) + 1));
            fread (res->mapList[POS].string, 1, atoi(num), aFile);
            res->mapList[POS].string[atoi(num)] = 0;
            for (i = 0; i < res->mapList[POS].numMonsterTypes; i++)
            {
                fread(num, 1, 2, aFile );
                res->mapList[POS].monsterTypeList[i] = malloc(sizeof (char) * (atoi(num) + 1));
                fread (res->mapList[POS].monsterTypeList[i], 1, atoi(num), aFile);
                res->mapList[POS].monsterTypeList[i][atoi(num)] = 0;
            }
            fgetc(aFile);

            res->mapList[POS].colisionMatrix = malloc(sizeof(char*) * res->mapList[POS].maxY );
            for (i = 0; i < res->mapList[POS].maxY; i++)
            {
                res->mapList[POS].colisionMatrix[i] = malloc(sizeof(char) * res->mapList[POS].maxX);
                fread (res->mapList[POS].colisionMatrix[i], 1, res->mapList[POS].maxX, aFile );
                fgetc(aFile);
            }
            #undef POS
        }
        else if (strcmp (buffer1, "unlock") == 0)
        {
            int type, sec, min;
            char aux[3];
            FILE * aFile = fopen(buffer2, "r");
            aux[2] = 0;
            while (fscanf(aFile, "name=%s%*[ ]map=%s%*[ ]type=%d%*[ ]min=%d%*[ ]sec=%d\n", buffer1, buffer2, &type, &min, &sec) != EOF)
            {
                res->numUnlocks++;
                res->unlockList = realloc(res->unlockList, sizeof(Unlock) * res->numUnlocks);
                #define A res->unlockList[res->numUnlocks - 1]
                A.type       = type;
                A.min        = min;
                A.sec        = sec;
                A.unlockName = _String(buffer1);
                A.mapName    = _String (buffer2);
                fread(aux, 1, 2, aFile);
                A.string = malloc (sizeof(char) * (atoi(aux) + 1 ));
                fread (A.string, 1, atoi(aux), aFile);
                A.string[atoi(aux)] = 0;
                fgetc(aFile);
                #undef A
            }
            fclose(aFile);
        }
    }


    fclose(mFile);
    res->overlayList = CreateOverlays(9);
    res->numOverlays = 9;
    // um menusinho basico pra começa =D

    res->overlayList[0].name = _String ("rightclickmenu");
    res->overlayList[0].z = 0.0f;
    res->overlayList[0].pos.width = 128.0f;
    res->overlayList[0].pos.height = 200.0f;

    res->overlayList[0].numButtons = 5;
    res->overlayList[0].buttonList = CreateButtons (5);

    res->overlayList[0].buttonList[0].hidden = 0;
    res->overlayList[0].buttonList[0].numQuads = 1;
    res->overlayList[0].buttonList[0].quadList = CreateQuads(1);
    res->overlayList[0].buttonList[0].quadList[0].pos.width = 128.0f;
    res->overlayList[0].buttonList[0].quadList[0].pos.height = 40.0f;
    res->overlayList[0].buttonList[0].quadList[0].texCoord.width = 0.5f;
    res->overlayList[0].buttonList[0].quadList[0].texCoord.height = 0.5f;
    res->overlayList[0].buttonList[0].quadList[0].tex = &res->textureList[1];
    res->overlayList[0].buttonList[0].quadList[0].isTextured = 1;
    res->overlayList[0].buttonList[0].numTexts = 1;
    res->overlayList[0].buttonList[0].textList = CreateTexts(1);
    res->overlayList[0].buttonList[0].textList[0].name = _String ("b");
    res->overlayList[0].buttonList[0].textList[0].string = _String("Botão 1");
    res->overlayList[0].buttonList[0].textList[0].pos.x = 15.0f;
    res->overlayList[0].buttonList[0].textList[0].pos.y = 8.0f;
    res->overlayList[0].buttonList[0].textList[0].hidden = 0;
    res->overlayList[0].buttonList[0].textList[0].B = 1.0f;
    res->overlayList[0].buttonList[0].textList[0].font = &res->fontList[0];

    res->overlayList[0].buttonList[1].hidden = 0;
    res->overlayList[0].buttonList[1].numQuads = 1;
    res->overlayList[0].buttonList[1].pos.y = 40.0f;
    res->overlayList[0].buttonList[1].quadList = CreateQuads(1);
    res->overlayList[0].buttonList[1].quadList[0].pos.width = 128.0f;
    res->overlayList[0].buttonList[1].quadList[0].pos.height = 40.0f;
    res->overlayList[0].buttonList[1].quadList[0].texCoord.width = 0.5f;
    res->overlayList[0].buttonList[1].quadList[0].texCoord.height = 0.5f;
    res->overlayList[0].buttonList[1].quadList[0].tex = &res->textureList[1];
    res->overlayList[0].buttonList[1].quadList[0].isTextured = 1;
    res->overlayList[0].buttonList[1].numTexts = 1;
    res->overlayList[0].buttonList[1].textList = CreateTexts(1);
    res->overlayList[0].buttonList[1].textList[0].name = _String ("b");
    res->overlayList[0].buttonList[1].textList[0].string = _String ("Botão 2");
    res->overlayList[0].buttonList[1].textList[0].pos.x = 15.0f;
    res->overlayList[0].buttonList[1].textList[0].pos.y = 8.0f;
    res->overlayList[0].buttonList[1].textList[0].hidden = 0;
    res->overlayList[0].buttonList[1].textList[0].B = 1.0f;
    res->overlayList[0].buttonList[1].textList[0].font = &res->fontList[0];

    res->overlayList[0].buttonList[2].hidden = 0;
    res->overlayList[0].buttonList[2].numQuads = 1;
    res->overlayList[0].buttonList[2].pos.y = 80.0f;
    res->overlayList[0].buttonList[2].quadList = CreateQuads(1);
    res->overlayList[0].buttonList[2].quadList[0].pos.width = 128.0f;
    res->overlayList[0].buttonList[2].quadList[0].pos.height = 40.0f;
    res->overlayList[0].buttonList[2].quadList[0].texCoord.width = 0.5f;
    res->overlayList[0].buttonList[2].quadList[0].texCoord.height = 0.5f;
    res->overlayList[0].buttonList[2].quadList[0].tex = &res->textureList[1];
    res->overlayList[0].buttonList[2].quadList[0].isTextured = 1;
    res->overlayList[0].buttonList[2].numTexts = 1;
    res->overlayList[0].buttonList[2].textList = CreateTexts(1);
    res->overlayList[0].buttonList[2].textList[0].name = _String ("b");
    res->overlayList[0].buttonList[2].textList[0].string = _String("Botão 3");
    res->overlayList[0].buttonList[2].textList[0].pos.x = 15.0f;
    res->overlayList[0].buttonList[2].textList[0].pos.y = 8.0f;
    res->overlayList[0].buttonList[2].textList[0].hidden = 0;
    res->overlayList[0].buttonList[2].textList[0].B = 1.0f;
    res->overlayList[0].buttonList[2].textList[0].font = &res->fontList[0];

    res->overlayList[0].buttonList[3].hidden = 0;
    res->overlayList[0].buttonList[3].numQuads = 1;
    res->overlayList[0].buttonList[3].pos.y = 120.0f;
    res->overlayList[0].buttonList[3].quadList = CreateQuads(1);
    res->overlayList[0].buttonList[3].quadList[0].pos.width = 128.0f;
    res->overlayList[0].buttonList[3].quadList[0].pos.height = 40.0f;
    res->overlayList[0].buttonList[3].quadList[0].texCoord.width = 0.5f;
    res->overlayList[0].buttonList[3].quadList[0].texCoord.height = 0.5f;
    res->overlayList[0].buttonList[3].quadList[0].tex = &res->textureList[1];
    res->overlayList[0].buttonList[3].quadList[0].isTextured = 1;
    res->overlayList[0].buttonList[3].numTexts = 1;
    res->overlayList[0].buttonList[3].textList = CreateTexts(1);
    res->overlayList[0].buttonList[3].textList[0].name = _String ("b");
    res->overlayList[0].buttonList[3].textList[0].string = _String("Botão 4");
    res->overlayList[0].buttonList[3].textList[0].pos.x = 15.0f;
    res->overlayList[0].buttonList[3].textList[0].pos.y = 8.0f;
    res->overlayList[0].buttonList[3].textList[0].hidden = 0;
    res->overlayList[0].buttonList[3].textList[0].B = 1.0f;
    res->overlayList[0].buttonList[3].textList[0].font = &res->fontList[0];

    res->overlayList[0].buttonList[4].hidden = 0;
    res->overlayList[0].buttonList[4].numQuads = 1;
    res->overlayList[0].buttonList[4].pos.y = 160.0f;
    res->overlayList[0].buttonList[4].quadList = CreateQuads(1);
    res->overlayList[0].buttonList[4].quadList[0].pos.width = 128.0f;
    res->overlayList[0].buttonList[4].quadList[0].pos.height = 40.0f;
    res->overlayList[0].buttonList[4].quadList[0].texCoord.width = 0.5f;
    res->overlayList[0].buttonList[4].quadList[0].texCoord.height = 0.5f;
    res->overlayList[0].buttonList[4].quadList[0].tex = &res->textureList[1];
    res->overlayList[0].buttonList[4].quadList[0].isTextured = 1;
    res->overlayList[0].buttonList[4].numTexts = 1;
    res->overlayList[0].buttonList[4].textList = CreateTexts(1);
    res->overlayList[0].buttonList[4].textList[0].name = _String ("b");
    res->overlayList[0].buttonList[4].textList[0].string = _String("Botão 5");
    res->overlayList[0].buttonList[4].textList[0].pos.x = 15.0f;
    res->overlayList[0].buttonList[4].textList[0].pos.y = 8.0f;
    res->overlayList[0].buttonList[4].textList[0].hidden = 0;
    res->overlayList[0].buttonList[4].textList[0].B = 1.0f;
    res->overlayList[0].buttonList[4].textList[0].font = &res->fontList[0];

    res->overlayList[1].name = _String ("mainmenu");
    res->overlayList[1].z = 0.0f;
    res->overlayList[1].pos.x = 310.0f;
    res->overlayList[1].pos.y = 280.0f;

    res->overlayList[1].numButtons = 3;
    res->overlayList[1].buttonList = CreateButtons(3);

    res->overlayList[1].buttonList[0].hidden = 0;
    res->overlayList[1].buttonList[0].name = _String ("newgame");
    res->overlayList[1].buttonList[0].Parameters[0] = 1;
    res->overlayList[1].buttonList[0].numQuads = 1;
    res->overlayList[1].buttonList[0].quadList = CreateQuads(1);
    res->overlayList[1].buttonList[0].quadList[0].tex = &res->textureList[1];
    res->overlayList[1].buttonList[0].quadList[0].pos.width = 180.0f;
    res->overlayList[1].buttonList[0].quadList[0].pos.height = 55.0f;
    res->overlayList[1].buttonList[0].quadList[0].texCoord.width = 0.5f;
    res->overlayList[1].buttonList[0].quadList[0].texCoord.height = 0.5f;
    res->overlayList[1].buttonList[0].quadList[0].isTextured = 1;
    res->overlayList[1].buttonList[0].numTexts = 1;
    res->overlayList[1].buttonList[0].textList = CreateTexts(1);
    res->overlayList[1].buttonList[0].textList[0].name = _String ("new");
    res->overlayList[1].buttonList[0].textList[0].string = _String("Novo Jogo");
    res->overlayList[1].buttonList[0].textList[0].pos.x = 28.0f;
    res->overlayList[1].buttonList[0].textList[0].pos.y = 10.0f;
    res->overlayList[1].buttonList[0].textList[0].hidden = 0;
    res->overlayList[1].buttonList[0].textList[0].B = 1.0f;
    res->overlayList[1].buttonList[0].textList[0].font = &res->fontList[0];

    res->overlayList[1].buttonList[1].hidden = 0;
    res->overlayList[1].buttonList[1].name = _String ("load");
    res->overlayList[1].buttonList[1].pos.y = 64.0f;
    res->overlayList[1].buttonList[1].Parameters[0] = 2;
    res->overlayList[1].buttonList[1].numQuads = 1;
    res->overlayList[1].buttonList[1].quadList = CreateQuads(1);
    res->overlayList[1].buttonList[1].quadList[0].tex = &res->textureList[1];
    res->overlayList[1].buttonList[1].quadList[0].pos.width = 180.0f;
    res->overlayList[1].buttonList[1].quadList[0].pos.height = 55.0f;
    res->overlayList[1].buttonList[1].quadList[0].texCoord.width = 0.5f;
    res->overlayList[1].buttonList[1].quadList[0].texCoord.height = 0.5f;
    res->overlayList[1].buttonList[1].quadList[0].isTextured = 1;
    res->overlayList[1].buttonList[1].numTexts = 1;
    res->overlayList[1].buttonList[1].textList = CreateTexts(1);
    res->overlayList[1].buttonList[1].textList[0].name = _String ("load");
    res->overlayList[1].buttonList[1].textList[0].string = _String("Carregar Jogo");
    res->overlayList[1].buttonList[1].textList[0].pos.x = 11.0f;
    res->overlayList[1].buttonList[1].textList[0].pos.y = 10.0f;
    res->overlayList[1].buttonList[1].textList[0].hidden = 0;
    res->overlayList[1].buttonList[1].textList[0].B = 1.0f;
    res->overlayList[1].buttonList[1].textList[0].font = &res->fontList[0];

    res->overlayList[1].buttonList[2].hidden = 0;
    res->overlayList[1].buttonList[2].name = _String ("exit");
    res->overlayList[1].buttonList[2].pos.y = 128.0f;
    res->overlayList[1].buttonList[2].Parameters[0] = 3;
    res->overlayList[1].buttonList[2].numQuads = 1;
    res->overlayList[1].buttonList[2].quadList = CreateQuads(1);
    res->overlayList[1].buttonList[2].quadList[0].tex = &res->textureList[1];
    res->overlayList[1].buttonList[2].quadList[0].pos.width = 180.0f;
    res->overlayList[1].buttonList[2].quadList[0].pos.height = 55.0f;
    res->overlayList[1].buttonList[2].quadList[0].texCoord.width = 0.5f;
    res->overlayList[1].buttonList[2].quadList[0].texCoord.height = 0.5f;
    res->overlayList[1].buttonList[2].quadList[0].isTextured = 1;
    res->overlayList[1].buttonList[2].numTexts = 1;
    res->overlayList[1].buttonList[2].textList = CreateTexts(1);
    res->overlayList[1].buttonList[2].textList[0].name = _String ("exit");
    res->overlayList[1].buttonList[2].textList[0].string = _String("Sair");
    res->overlayList[1].buttonList[2].textList[0].pos.x = 65.0f;
    res->overlayList[1].buttonList[2].textList[0].pos.y = 10.0f;
    res->overlayList[1].buttonList[2].textList[0].hidden = 0;
    res->overlayList[1].buttonList[2].textList[0].B = 1.0f;
    res->overlayList[1].buttonList[2].textList[0].font = &res->fontList[0];


    res->overlayList[2].name = _String ("mainmenudemo");
    res->overlayList[2].z = 0.0f;
    res->overlayList[2].pos.x = 20.0f;
    res->overlayList[2].pos.y = 280.0f;

    res->overlayList[2].numTexts = 1;
    res->overlayList[2].textList = CreateTexts(1);
    res->overlayList[2].textList[0].font = &res->fontList[0];
    res->overlayList[2].textList[0].R = 1.0f;
    res->overlayList[2].textList[0].pos.y = -40.0f;
    res->overlayList[2].textList[0].hidden = 0;
    res->overlayList[2].textList[0].name = _String ("demo");
    res->overlayList[2].textList[0].string = _String ("Demo");

    res->overlayList[2].numButtons = 1;
    res->overlayList[2].buttonList = CreateButtons(1);

    res->overlayList[2].buttonList[0].hidden = 0;
    res->overlayList[2].buttonList[0].name = _String ("maptest");
    res->overlayList[2].buttonList[0].numQuads = 1;
    res->overlayList[2].buttonList[0].quadList = CreateQuads(1);
    res->overlayList[2].buttonList[0].quadList[0].tex = &res->textureList[1];
    res->overlayList[2].buttonList[0].quadList[0].pos.width = 180.0f;
    res->overlayList[2].buttonList[0].quadList[0].pos.height = 55.0f;
    res->overlayList[2].buttonList[0].quadList[0].texCoord.width = 0.5f;
    res->overlayList[2].buttonList[0].quadList[0].texCoord.height = 0.5f;
    res->overlayList[2].buttonList[0].quadList[0].isTextured = 1;
    res->overlayList[2].buttonList[0].numTexts = 1;
    res->overlayList[2].buttonList[0].textList = CreateTexts(1);
    res->overlayList[2].buttonList[0].textList[0].name = _String ("t");
    res->overlayList[2].buttonList[0].textList[0].string = _String("Testar Mapa");
    res->overlayList[2].buttonList[0].textList[0].pos.x = 15.0f;
    res->overlayList[2].buttonList[0].textList[0].pos.y = 10.0f;
    res->overlayList[2].buttonList[0].textList[0].hidden = 0;
    res->overlayList[2].buttonList[0].textList[0].R = 1.0f;
    res->overlayList[2].buttonList[0].textList[0].font = &res->fontList[0];


    res->overlayList[3].name = _String ("transquad");
    res->overlayList[3].z = 1.0f;

    res->overlayList[3].numQuads = 1;
    res->overlayList[3].quadList = CreateQuads (1);
    res->overlayList[3].quadList[0].isColored = 1;
    for (i = 0; i< 16; i++) res->overlayList[3].quadList[0].color[i] = 0.3f;

    res->overlayList[4].name = _String ("counters");
    res->overlayList[4].z = -0.1f;
    res->overlayList[4].pos.x = 10.0f;
    res->overlayList[4].pos.y = 1.0f;

    res->overlayList[4].numQuads = 1;
    res->overlayList[4].quadList = CreateQuads(1);
    res->overlayList[4].quadList[0].tex = GetResourceTexture("igmenu", res);
    res->overlayList[4].quadList[0].texCoord.x = 0.0f;
    res->overlayList[4].quadList[0].texCoord.y = 0.0f;
    res->overlayList[4].quadList[0].texCoord.width = 0.546875f;
    res->overlayList[4].quadList[0].texCoord.height = 0.07421875f;
    res->overlayList[4].quadList[0].pos.width = 280.0f;
    res->overlayList[4].quadList[0].pos.height = 38.0f;
    res->overlayList[4].quadList[0].isTextured = 1;

    res->overlayList[4].numTexts = 2;
    res->overlayList[4].textList = CreateTexts(2);
    res->overlayList[4].textList[0].name = _String ("t");
    res->overlayList[4].textList[0].hidden = 0;
    res->overlayList[4].textList[0].R = 1.0f;
    res->overlayList[4].textList[0].B = 1.0f;
    res->overlayList[4].textList[0].G = 1.0f;
    res->overlayList[4].textList[0].font = GetResourceFont("default", res);
    res->overlayList[4].textList[0].pos.y = 3.0f;
    res->overlayList[4].textList[0].pos.x = 10.0f;

    res->overlayList[4].textList[1].name = _String ("a");
    res->overlayList[4].textList[1].hidden = 0;
    res->overlayList[4].textList[1].R = 1.0f;
    res->overlayList[4].textList[1].B = 1.0f;
    res->overlayList[4].textList[1].G = 1.0f;
    res->overlayList[4].textList[1].font = GetResourceFont("default", res);
    res->overlayList[4].textList[1].pos.y = 3.0f;
    res->overlayList[4].textList[1].pos.x = 213.0f;
    res->overlayList[4].textList[1].string = malloc(sizeof(char) * 4);
    res->overlayList[4].textList[1].string[0] = 0;
    res->overlayList[4].textList[1].string[4] = 0;

    res->overlayList[4].numButtons = 5;
    res->overlayList[4].buttonList = CreateButtons(5);
    res->overlayList[4].buttonList[0].numQuads = 1;
    res->overlayList[4].buttonList[0].quadList = CreateQuads(1);
    res->overlayList[4].buttonList[0].pos.y = 11.0f;
    res->overlayList[4].buttonList[0].quadList[0].pos.width = 17;
    res->overlayList[4].buttonList[0].quadList[0].pos.height = 14;
    res->overlayList[4].buttonList[0].quadList[0].tex = GetResourceTexture("igmenu", res);
    res->overlayList[4].buttonList[0].quadList[0].texCoord.x = 0.546875f;
    res->overlayList[4].buttonList[0].quadList[0].texCoord.y = 0.0f;
    res->overlayList[4].buttonList[0].quadList[0].texCoord.width = 0.033203125f;
    res->overlayList[4].buttonList[0].quadList[0].texCoord.height = 0.02734375f;
    res->overlayList[4].buttonList[0].quadList[0].isTextured = 1;
    res->overlayList[4].buttonList[0].hidden = 0;
    memcpy(&res->overlayList[4].buttonList[1], res->overlayList[4].buttonList, sizeof(Button));
    memcpy(&res->overlayList[4].buttonList[2], res->overlayList[4].buttonList, sizeof(Button) * 2);
    memcpy(&res->overlayList[4].buttonList[4], res->overlayList[4].buttonList, sizeof(Button));

    res->overlayList[4].buttonList[0].name = _String("1");
    res->overlayList[4].buttonList[0].pos.x = 91.0f;

    res->overlayList[4].buttonList[1].name = _String("2");
    res->overlayList[4].buttonList[1].pos.x = 109.0f;

    res->overlayList[4].buttonList[2].name = _String("3");
    res->overlayList[4].buttonList[2].pos.x = 127.0f;

    res->overlayList[4].buttonList[3].name = _String("4");
    res->overlayList[4].buttonList[3].pos.x = 145.0f;

    res->overlayList[4].buttonList[4].name = _String("5");
    res->overlayList[4].buttonList[4].pos.x = 163.0f;


    res->overlayList[5].name = _String ("escmenu");
    res->overlayList[5].pos.x = 267.0f;
    res->overlayList[5].pos.y = 174.5f;
    res->overlayList[5].numQuads = 1;
    res->overlayList[5].quadList = CreateQuads(1);
    res->overlayList[5].quadList[0].pos.width = 266.0f;
    res->overlayList[5].quadList[0].pos.height = 251.0f;
    res->overlayList[5].quadList[0].tex = GetResourceTexture ("igmenu", res);
    res->overlayList[5].quadList[0].isTextured = 1;
    res->overlayList[5].quadList[0].texCoord.x = 0.0f;
    res->overlayList[5].quadList[0].texCoord.y = 0.080078125f;
    res->overlayList[5].quadList[0].texCoord.width = 0.51953125f;
    res->overlayList[5].quadList[0].texCoord.height = 0.48828125f;
    res->overlayList[5].numButtons = 3;
    res->overlayList[5].buttonList = CreateButtons(3);

    res->overlayList[5].buttonList[0].name = _String("cont");
    res->overlayList[5].buttonList[0].pos.x = 18.0f;
    res->overlayList[5].buttonList[0].pos.y = 20.0f;
    res->overlayList[5].buttonList[0].pos.width = 229.0f;
    res->overlayList[5].buttonList[0].pos.height = 65.0f;
    res->overlayList[5].buttonList[0].hidden = 0;
    res->overlayList[5].buttonList[0].numTexts = 1;
    res->overlayList[5].buttonList[0].Parameters[0] = 1;
    res->overlayList[5].buttonList[0].textList = CreateTexts(1);
    res->overlayList[5].buttonList[0].textList[0].font = GetResourceFont("default", res);
    res->overlayList[5].buttonList[0].textList[0].R = 1.0f;
    res->overlayList[5].buttonList[0].textList[0].B = 1.0f;
    res->overlayList[5].buttonList[0].textList[0].G = 1.0f;
    res->overlayList[5].buttonList[0].textList[0].name = _String("t");
    res->overlayList[5].buttonList[0].textList[0].hidden = 0;
    res->overlayList[5].buttonList[0].textList[0].string = _String ("Continuar");
    res->overlayList[5].buttonList[0].textList[0].pos.x = 55.0f;
    res->overlayList[5].buttonList[0].textList[0].pos.y = 19.5f;
    res->overlayList[5].buttonList[0].onClick = &OnClickInGameMenu;

    res->overlayList[5].buttonList[1].name = _String("save");
    res->overlayList[5].buttonList[1].pos.x = 18.0f;
    res->overlayList[5].buttonList[1].pos.y = 92.0f;
    res->overlayList[5].buttonList[1].pos.width = 229.0f;
    res->overlayList[5].buttonList[1].pos.height = 65.0f;
    res->overlayList[5].buttonList[1].hidden = 0;
    res->overlayList[5].buttonList[1].numTexts = 1;
    res->overlayList[5].buttonList[1].Parameters[0] = 2;
    res->overlayList[5].buttonList[1].textList = CreateTexts(1);
    res->overlayList[5].buttonList[1].textList[0].font = GetResourceFont("default", res);
    res->overlayList[5].buttonList[1].textList[0].R = 1.0f;
    res->overlayList[5].buttonList[1].textList[0].B = 1.0f;
    res->overlayList[5].buttonList[1].textList[0].G = 1.0f;
    res->overlayList[5].buttonList[1].textList[0].name = _String("t");
    res->overlayList[5].buttonList[1].textList[0].hidden = 0;
    res->overlayList[5].buttonList[1].textList[0].string = _String ("Salvar e Sair");
    res->overlayList[5].buttonList[1].textList[0].pos.x = 40.0f;
    res->overlayList[5].buttonList[1].textList[0].pos.y = 19.5f;
    res->overlayList[5].buttonList[1].onClick = &OnClickInGameMenu;

    res->overlayList[5].buttonList[2].name = _String("sair");
    res->overlayList[5].buttonList[2].pos.x = 18.0f;
    res->overlayList[5].buttonList[2].pos.y = 165.0f;
    res->overlayList[5].buttonList[2].pos.width = 229.0f;
    res->overlayList[5].buttonList[2].pos.height = 65.0f;
    res->overlayList[5].buttonList[2].hidden = 0;
    res->overlayList[5].buttonList[2].numTexts = 1;
    res->overlayList[5].buttonList[2].textList = CreateTexts(1);
    res->overlayList[5].buttonList[2].Parameters[0] = 3;
    res->overlayList[5].buttonList[2].textList[0].font = GetResourceFont("default", res);
    res->overlayList[5].buttonList[2].textList[0].R = 1.0f;
    res->overlayList[5].buttonList[2].textList[0].B = 1.0f;
    res->overlayList[5].buttonList[2].textList[0].G = 1.0f;
    res->overlayList[5].buttonList[2].textList[0].name = _String("t");
    res->overlayList[5].buttonList[2].textList[0].hidden = 0;
    res->overlayList[5].buttonList[2].textList[0].string = _String ("Sair");
    res->overlayList[5].buttonList[2].textList[0].pos.x = 84.5f;
    res->overlayList[5].buttonList[2].textList[0].pos.y = 19.5f;
    res->overlayList[5].buttonList[2].onClick = &OnClickInGameMenu;

    res->overlayList[6].name = _String ("endmenu");
    res->overlayList[6].pos.x = 266.0f;
    res->overlayList[6].pos.y = 218.5f;
    res->overlayList[6].numTexts = 1;
    res->overlayList[6].textList = CreateTexts(1);
    res->overlayList[6].textList[0].R = 1.0f;
    res->overlayList[6].textList[0].B = 1.0f;
    res->overlayList[6].textList[0].G = 1.0f;
    res->overlayList[6].textList[0].name = _String("t");
    res->overlayList[6].textList[0].font = GetResourceFont("default", res);
    res->overlayList[6].textList[0].hidden = 0;
    res->overlayList[6].textList[0].pos.x = 98.5f;
    res->overlayList[6].textList[0].pos.y = 60.5f;
    res->overlayList[6].textList[0].string = malloc(sizeof(char) * 8);
    res->overlayList[6].numQuads = 1;
    res->overlayList[6].quadList = CreateQuads(1);
    res->overlayList[6].quadList[0].pos.width = 268.0f;
    res->overlayList[6].quadList[0].pos.height = 163.0f;
    res->overlayList[6].quadList[0].tex = GetResourceTexture ("igmenu", res);
    res->overlayList[6].quadList[0].isTextured = 1;
    res->overlayList[6].quadList[0].texCoord.x = 0.0f;
    res->overlayList[6].quadList[0].texCoord.y = 0.681640625f;
    res->overlayList[6].quadList[0].texCoord.width = 0.5234375f;
    res->overlayList[6].quadList[0].texCoord.height = 0.318359375f;
    res->overlayList[6].numButtons = 2;
    res->overlayList[6].buttonList = CreateButtons(2);

    res->overlayList[6].buttonList[0].name = _String("re");
    res->overlayList[6].buttonList[0].pos.x = 39.0f;
    res->overlayList[6].buttonList[0].pos.y = 107.0f;
    res->overlayList[6].buttonList[0].pos.width = 48.0f;
    res->overlayList[6].buttonList[0].pos.height = 47.0f;
    res->overlayList[6].buttonList[0].hidden = 0;
    res->overlayList[6].buttonList[0].Parameters[0] = 4;
    res->overlayList[6].buttonList[0].onClick = &OnClickInGameMenu;

    res->overlayList[6].buttonList[1].name = _String("exit");
    res->overlayList[6].buttonList[1].pos.x = 181.0f;
    res->overlayList[6].buttonList[1].pos.y = 107.0f;
    res->overlayList[6].buttonList[1].pos.width = 48.0f;
    res->overlayList[6].buttonList[1].pos.height = 47.0f;
    res->overlayList[6].buttonList[1].hidden = 0;
    res->overlayList[6].buttonList[1].Parameters[0] = 3;
    res->overlayList[6].buttonList[1].onClick = &OnClickInGameMenu;

    res->overlayList[7].name = _String("unlock");
    res->overlayList[7].hidden = 1;
    res->overlayList[7].numQuads = 1;
    res->overlayList[7].pos.x = 225.0f;
    res->overlayList[7].pos.y = 158.5f;
    res->overlayList[7].quadList = CreateQuads(1);
    res->overlayList[7].quadList[0].pos.width = 350.0f;
    res->overlayList[7].quadList[0].pos.height = 55.0f;
    res->overlayList[7].quadList[0].isTextured = 1;
    res->overlayList[7].quadList[0].tex = GetResourceTexture("igmenu", res);
    res->overlayList[7].quadList[0].texCoord.x = 0.0f;
    res->overlayList[7].quadList[0].texCoord.y = 0.572265625f;
    res->overlayList[7].quadList[0].texCoord.width = 0.68359375f;
    res->overlayList[7].quadList[0].texCoord.height = 0.107421875f;
    res->overlayList[7].numTexts = 1;
    res->overlayList[7].textList = CreateTexts(1);
    res->overlayList[7].textList[0].R = 1.0f;
    res->overlayList[7].textList[0].B = 1.0f;
    res->overlayList[7].textList[0].G = 1.0f;
    res->overlayList[7].textList[0].font = GetResourceFont("default", res);
    res->overlayList[7].textList[0].name = _String ("t");
    res->overlayList[7].textList[0].hidden = 0;
    res->overlayList[7].textList[0].string = malloc (sizeof (char) * 150 );
    res->overlayList[7].textList[0].pos.x = 30.0f;
    res->overlayList[7].textList[0].pos.y = 14.5f;

    res->overlayList[8].name = _String("mapsel");
    res->overlayList[8].z = 0.75f;
    res->overlayList[8].hidden = 1;
    res->overlayList[8].pos.x = 50.0f;
    res->overlayList[8].pos.y = 50.0f;
    res->overlayList[8].numQuads = 2;
    res->overlayList[8].quadList = CreateQuads(2);
    res->overlayList[8].quadList[0].pos.width = 700.0f;
    res->overlayList[8].quadList[0].pos.height = 450.0f;
    res->overlayList[8].quadList[0].isTextured = 1;
    res->overlayList[8].quadList[0].tex = GetResourceTexture("mapsel", res);
    res->overlayList[8].quadList[0].texCoord.x = 0.0f;
    res->overlayList[8].quadList[0].texCoord.y = 0.0f;
    res->overlayList[8].quadList[0].texCoord.width = 1.0f;
    res->overlayList[8].quadList[0].texCoord.height = 0.8823529411764f;

    res->overlayList[8].quadList[1].pos.x = 150.0f;
    res->overlayList[8].quadList[1].pos.y = 75.0f;
    res->overlayList[8].quadList[1].pos.width = 400.0f;
    res->overlayList[8].quadList[1].pos.height = 300.0f;
    res->overlayList[8].quadList[1].texCoord.x = 0.0f;
    res->overlayList[8].quadList[1].texCoord.y = 0.0f;
    res->overlayList[8].quadList[1].texCoord.width = 1.0f;
    res->overlayList[8].quadList[1].texCoord.height = 1.0f;

    res->overlayList[8].numButtons = 4;
    res->overlayList[8].buttonList = CreateButtons(4);
    res->overlayList[8].buttonList[0].name = _String ("exit");
    res->overlayList[8].buttonList[0].hidden = 0;
    res->overlayList[8].buttonList[0].pos.x = 638.0f;
    res->overlayList[8].buttonList[0].pos.y = 10.0f;
    res->overlayList[8].buttonList[0].pos.width = 48.0f;
    res->overlayList[8].buttonList[0].pos.height = 44.0f;
    res->overlayList[8].buttonList[0].numQuads = 0;
    res->overlayList[8].buttonList[0].onClick = &OnClickMapSelector;
    res->overlayList[8].buttonList[0].Parameters[0] = 1;

    res->overlayList[8].buttonList[1].name = _String("jogar");
    res->overlayList[8].buttonList[1].hidden = 0;
    res->overlayList[8].buttonList[1].pos.x = 266.0f;
    res->overlayList[8].buttonList[1].pos.y = 378.0f;
    res->overlayList[8].buttonList[1].pos.width = 136.0f;
    res->overlayList[8].buttonList[1].pos.height = 56.0f;
    res->overlayList[8].buttonList[1].numQuads = 0;
    res->overlayList[8].buttonList[1].onClick = &OnClickMapSelector;
    res->overlayList[8].buttonList[1].Parameters[0] = 2;

    res->overlayList[8].buttonList[2].name = _String("foward");
    res->overlayList[8].buttonList[2].hidden = 0;
    res->overlayList[8].buttonList[2].pos.x = 673.0f;
    res->overlayList[8].buttonList[2].pos.y = 197.5f;
    res->overlayList[8].buttonList[2].onClick = &OnClickMapSelector;
    res->overlayList[8].buttonList[2].numQuads = 1;
    res->overlayList[8].buttonList[2].quadList = CreateQuads (1);
    res->overlayList[8].buttonList[2].quadList[0].pos.width = 54.0f;
    res->overlayList[8].buttonList[2].quadList[0].pos.height = 55.0f;
    res->overlayList[8].buttonList[2].quadList[0].isTextured = 1;
    res->overlayList[8].buttonList[2].quadList[0].tex = GetResourceTexture("mapsel", res);
    res->overlayList[8].buttonList[2].quadList[0].texCoord.x = 0.0f;
    res->overlayList[8].buttonList[2].quadList[0].texCoord.y = 0.893f;
    res->overlayList[8].buttonList[2].quadList[0].texCoord.width = 0.07714285714285714285714285714286f;
    res->overlayList[8].buttonList[2].quadList[0].texCoord.height = 0.107f;
    res->overlayList[8].buttonList[2].Parameters[0] = 3;

    res->overlayList[8].buttonList[3].name = _String("backward");
    res->overlayList[8].buttonList[3].hidden = 0;
    res->overlayList[8].buttonList[3].pos.x = -27.0f;
    res->overlayList[8].buttonList[3].pos.y = 197.5f;
    res->overlayList[8].buttonList[3].onClick = &OnClickMapSelector;
    res->overlayList[8].buttonList[3].numQuads = 1;
    res->overlayList[8].buttonList[3].quadList = CreateQuads (1);
    res->overlayList[8].buttonList[3].quadList[0].pos.width = 54.0f;
    res->overlayList[8].buttonList[3].quadList[0].pos.height = 55.0f;
    res->overlayList[8].buttonList[3].quadList[0].isTextured = 1;
    res->overlayList[8].buttonList[3].quadList[0].tex = GetResourceTexture("mapsel", res);
    res->overlayList[8].buttonList[3].quadList[0].texCoord.x = 0.07714285714285714285714285714286f;
    res->overlayList[8].buttonList[3].quadList[0].texCoord.y = 0.893f;
    res->overlayList[8].buttonList[3].quadList[0].texCoord.width = -0.07714285714285714285714285714286f;
    res->overlayList[8].buttonList[3].quadList[0].texCoord.height = 0.107f;
    res->overlayList[8].buttonList[3].Parameters[0] = 4;
}


int UpdateTextStringOV (const char * name, const char * string, Overlay * ov)
{
    int i;
    for (i = 0; i< ov->numTexts; i++)
        if (!strcmp (name, ov->textList[i].name))
        {
            if (ov->textList[i].string != NULL) free (ov->textList[i].string);
            ov->textList[i].string = _String (string);
            return 1;
        }
    return 0;
}

int UpdateTextStringBT (const char * name, const char * string, Button * bt)
{
    int i;
    for (i = 0; i< bt->numTexts; i++)
        if (!strcmp (name, bt->textList[i].name))
        {
            if (bt->textList[i].string != NULL) free (bt->textList[i].string);
            bt->textList[i].string = _String (string);
            return 1;
        }
    return 0;
}


Texture * GetResourceTexture (const char * name, Resources * res)
{
    int i;
    for (i = 0; i < res->numTextures; i++)
        if (res->textureList[i].name != NULL)
            if (!strcmp (name,res->textureList[i].name))
                return &res->textureList[i];

    return NULL;
}

Font * GetResourceFont (const char * name, Resources * res)
{
    int i;
    for (i = 0; i < res->numFonts; i++)
        if (res->fontList[i].name != NULL)
            if (!strcmp (name,res->fontList[i].name))
                return &res->fontList[i];

    return NULL;
}

Overlay * GetResourceOverlay (const char * name, Resources * res)
{
    int i;
    for (i = 0; i < res->numOverlays; i++)
        if (res->overlayList[i].name != NULL)
            if (!strcmp (name,res->overlayList[i].name))
                return &res->overlayList[i];
    return NULL;
}

GameObject * GetResourceObjectCopy (const char * name, Resources * res)
{
    int i;
    for (i = 0; i < res->numObjects; i++)
        if (res->objectList[i].name != NULL)
            if (!strcmp (name,res->objectList[i].name))
            {
                GameObject * ptr = malloc (sizeof (GameObject));
                memcpy(ptr, &res->objectList[i], sizeof(GameObject));
                ptr->quadList = malloc (sizeof (Quad) * ptr->numQuads);
                memcpy(ptr->quadList, res->objectList[i].quadList, sizeof (Quad) * ptr->numQuads );
                return ptr;
            }
    return NULL;
}

Map * GetResourceMap (const char * mapName, Resources * res)
{
    int i;
    for (i = 0; i < res->numMaps; i++)
        if (res->mapList[i].name != NULL)
            if (!strcmp (mapName,res->mapList[i].name))
            {
                Map * ptr = malloc (sizeof(Map));
                memcpy (ptr, &res->mapList[i], sizeof(Map));
                return ptr;
            }
    return NULL;
}

int GetNextUnlockPos (const char * mapName, Resources * res, int min, int sec)
{
    int i;
    for (i = 0; i < res->numUnlocks; i++)
        if (strcmp(res->unlockList[i].mapName, mapName) == 0)
            if ( res->unlockList[i].min < min || (res->unlockList[i].min == min && res->unlockList[i].sec <= sec)) return i;
    return -1;
}

void FreeQuad (Quad * ptr, int num)
{
    free (ptr);
}

void FreeText (Text * ptr, int num)
{
    int i;
    for (i = 0; i < num ; i ++)
    {
        if (ptr[i].name != NULL) free (ptr[i].name);
        if (ptr[i].string != NULL) free (ptr[i].string);
    }
    free (ptr);
}

void FreeButton (Button * ptr, int num)
{
    if (ptr->numQuads > 0) FreeQuad(ptr->quadList, ptr->numQuads);
    if (ptr->numTexts > 0) FreeText(ptr->textList, ptr->numTexts);
    free (ptr);
}

void FreeBackground (Background * ptr)
{
    if (ptr->numQuads > 0) FreeQuad(ptr->quadList, ptr->numQuads);
    free (ptr);
}

void FreeGameObject (GameObject * ptr)
{
    free (ptr->quadList);
    free (ptr);
}

void FreeObjectList (ObjectList * ptr)
{
    ObjectList * aux;
    while (ptr != NULL)
    {
        aux = ptr->next;
        if (ptr->obj != NULL) FreeGameObject (ptr->obj);
        free (ptr);
        ptr = aux;
    }
}

void FreeMap(Map * ptr)
{
    int i;
    for (i= 0; i < ptr->numGuns; i++) free (ptr->gunList[i].quadList);
    if (ptr->numGuns > 0) free (ptr->gunList);
    FreeObjectList (ptr->objectListFirst);
}

Background * BuildBackground (Texture * tex)
{
    Background * ptr = malloc (sizeof (Background));
    ptr->quadList = CreateQuads(1);
    ptr->numQuads = 1;
    ptr->quadList[0].pos.width = 800;
    ptr->quadList[0].pos.height = 600;
    ptr->quadList[0].tex = tex;
    ptr->quadList[0].texCoord.width = 1.0f;
    ptr->quadList[0].texCoord.height = 1.0f;
    ptr->quadList[0].isTextured = 1;

    return ptr;
}

void BuildMainMenu (GameState * gs)
{
    int i;
    if (gs->map != NULL)
    {
        FreeMap(gs->map);
    }

    if (gs->numOverlays > 0)
    {
        free (gs->overlayList);
        gs->numOverlays = 0;
    }
    for (i = 0; i < 10; i++) gs->keybuffer[i] = 0;
    gs->renderFunction = &RenderMap;
    gs->inputFunction = &InputMainMenu;
    gs->numOverlays = 3;
    gs->overlayList = malloc (sizeof (Overlay*) * 3);
    gs->overlayList[1] = GetResourceOverlay ("mainmenu", gs->res);
    gs->overlayList[1]->buttonList[0].onClick = &OnClickMainMenuButton;
    gs->overlayList[1]->buttonList[1].onClick = &OnClickMainMenuButton;
    gs->overlayList[1]->buttonList[2].onClick = &OnClickMainMenuButton;
    gs->overlayList[2] = GetResourceOverlay ("transquad", gs->res);
    gs->overlayList[1]->z = 0.0f;
    gs->overlayList[1]->hidden = 0;
    gs->overlayList[0] = GetResourceOverlay("mapsel", gs->res);
    gs->overlayList[0]->hidden = 1;
    gs->map = CreateMap();
    gs->map->back = BuildBackground (GetResourceTexture("mainmenuback", gs->res));
    gs->map->maxY = 0;
    gs->map->maxX = 0;
    gs->map->objectListFirst = NULL;
}


void BuildMap (GameState * gs, const char * mapName)
{
    int i, x, y;
    if (gs->map != NULL) FreeMap (gs->map);
    if (gs->numOverlays > 0)
    {
        free (gs->overlayList);
        gs->numOverlays = 0;
    }
    for (i = 0; i < 10; i++) gs->keybuffer[i] = 0;
    gs->map = GetResourceMap (mapName, gs->res);
    if (gs->map == NULL) printf ("Erro ao ler mapa %s", mapName);
    gs->renderFunction = &RenderMap;
    gs->inputFunction  = &UpdateMap;
    gs->map->counter = 5.0f;

    gs->numOverlays = 4;
    gs->overlayList = malloc (sizeof(Overlay*) * 4);
    gs->overlayList[0] = GetResourceOverlay("counters", gs->res);
    if (gs->overlayList[0]->textList[0].string != NULL) free (gs->overlayList[0]->textList[0].string);
    gs->overlayList[0]->buttonList[0].hidden = 0; gs->overlayList[0]->buttonList[1].hidden = 0; gs->overlayList[0]->buttonList[2].hidden = 0;
    gs->overlayList[0]->buttonList[3].hidden = 0; gs->overlayList[0]->buttonList[4].hidden = 0;
    gs->overlayList[0]->textList[0].string = _String("00'00''");
    gs->overlayList[0]->textList[0].hidden = 0;
    gs->overlayList[0]->hidden = 0;

    gs->overlayList[1] = GetResourceOverlay("escmenu", gs->res);
    gs->overlayList[1]->hidden = 1;

    gs->overlayList[2] = GetResourceOverlay("endmenu", gs->res);
    gs->overlayList[2]->hidden = 1;

    gs->overlayList[3] = GetResourceOverlay("unlock", gs->res);
    gs->overlayList[3]->hidden = 1;


    ObjectList * ol = malloc (sizeof(ObjectList));
    ol->obj = GetResourceObjectCopy("player", gs->res);
    ol->next = NULL;
    ol->prev = NULL;
    gs->map->objectListFirst = ol;

    ol->obj->pos.x = gs->map->startPos.x;
    ol->obj->pos.y = gs->map->startPos.y;
    ol->obj->hidden = 0;

    gs->map->gunList = NULL;
    gs->map->numGuns = 0;
    for (i = 0; i < gs->res->numObjects; i++)
        if (gs->res->objectList[i].type == 4 && gs->res->objectList[i].hidden == 0)
        {
            gs->map->numGuns++;
            gs->map->gunList = realloc (gs->map->gunList, sizeof (GameObject) * gs->map->numGuns);
            memcpy(&gs->map->gunList[gs->map->numGuns - 1], &gs->res->objectList[i], sizeof(GameObject));
            gs->map->gunList[gs->map->numGuns - 1].direction[0] = 1.0f;
            gs->map->gunList[gs->map->numGuns - 1].quadList = malloc (sizeof (Quad) * gs->map->gunList[gs->map->numGuns - 1].numQuads);
            memcpy(gs->map->gunList[gs->map->numGuns - 1].quadList, gs->res->objectList[i].quadList, sizeof (Quad) * gs->map->gunList[gs->map->numGuns - 1].numQuads );
        }
    if (gs->map->numGuns > 0)
    {
        gs->map->gun = &gs->map->gunList[0];
        gs->map->ammo = gs->map->gun->speed;
    }
    i = 0;
    do
    {
        int j;
        x = rand() % gs->map->maxX;
        y = (rand() % (gs->map->maxY - 1)) + 1;
        for (j = y; j < gs->map->maxY; j++)
            if (gs->map->colisionMatrix[j][x] == '0' && gs->map->colisionMatrix[j - 1][x] == '1') break;
        if (j < gs->map->maxY)
        {
            i = 1;
            y = j;
        }
    } while (i == 0);
    ol = malloc(sizeof(ObjectList));
    ol->obj = GetResourceObjectCopy ("box", gs->res);
    ol->obj->pos.x = x * (WINDOW_WIDTH / gs->map->maxX) + (((WINDOW_WIDTH / gs->map->maxX) - ol->obj->quadList[0].pos.width) / 2);
    ol->obj->pos.y = y * (WINDOW_HEIGHT / gs->map->maxY) - ol->obj->quadList[0].pos.height;
    gs->map->objectListLast = ol;
    gs->map->objectListFirst->next = ol;
    ol->next = NULL;
    gs->map->lock = 0;
    gs->map->end = 0;
    gs->map->life = 5;
    glfwSetTime(0.0);
}


void * GetUpdateFunc (int n)
{
    switch (n)
    {
        case 1: return &UpdatePlayer;
        case 2: return &UpdateSimpleEnemy;
        case 3: return &UpdateBullet;
        case 4: return &UpdatePistol;
        case 5: return &UpdateDualPistol;
        case 6: return &UpdateShotgun;
        case 7: return &UpdateRifle;
        default: return NULL;
    }
    return NULL;
}
void * GetAnimationFunc (int n)
{
    return NULL;
}

void * GetCollisionFunc (int n)
{
    switch (n)
    {
        case 1: return &CollisionPlayer;
        case 2: return &CollisionBullet;
        default: return NULL;
    }
}

void MapCollision (Map * map, GameObject * obj, double time)
{
    // 00 - 10
    // |     |
    // 10 - 11
    int i, j, pa;
    float x[2];
    float y[2];
    int px[4];
    int py[4];
    float sizeX, sizeY;
    x[0] = obj->pos.x + obj->quadList[0].pos.x + 0.1f * obj->quadList[0].pos.width ;
    x[1] = obj->pos.x + obj->quadList[0].pos.x + 0.9f * obj->quadList[0].pos.width;

    y[0] = obj->pos.y + obj->quadList[0].pos.y + 0.1f * obj->quadList[0].pos.height;
    y[1] = obj->pos.y + obj->quadList[0].pos.y + 0.9f * obj->quadList[0].pos.height;

    sizeX = 800 / map->maxX;
    sizeY = 600 / map->maxY;

    px[0] = (int)((x[0] + (time * obj->direction[0]))/ sizeX);
    px[1] = (int)((x[1] + (time * obj->direction[0]))/ sizeX);
    px[2] = (int)((x[0])/ sizeX);
    px[3] = (int)((x[1])/ sizeX);

    if (px[0] > px[1])
    {
        int aux = px[0];
        px[0] = px[1];
        px[1] = aux;
        aux = px[2];
        px[2] = px[3];
        px[3] = aux;
    }

    py[0] = (int)((y[0] + (time * obj->direction[1]))/ sizeY);
    py[1] = (int)((y[1] + (time * obj->direction[1]))/ sizeY);
    py[2] = (int)((y[0])/ sizeY);
    py[3] = (int)((y[1])/ sizeY);

    if (py[0] > py[1])
    {
        int aux = py[0];
        py[0] = py[1];
        py[1] = aux;
        aux = py[2];
        py[2] = py[3];
        py[3] = aux;
    }

    pa = 1;

    // verifica se as posições finais são validas com ambos componentes
    #define RANGE_X(i) ( i < map->maxX && i >= 0 )
    #define RANGE_Y(i) ( i < map->maxY && i >= 0 )
    // metodo antigo verificava apenas os 4 quadrantes onde cada ponto do quadrado principal estavam
    // deu problema com criaturas com weight ou height > size(X/Y)
    // esse metodo verifica todos os quadrantes dentro do quadrado do objeto
    for (i = px[0]; i <= px[1] && pa == 1; i++)
        for (j = py[0]; j <= py[1]; j++)
            if (RANGE_X(i) && RANGE_Y(j))
                if (map->colisionMatrix[j][i] != '1')
                {
                    pa = 0;
                    break; // sai do for interno apenas(?)
                }
    if (pa == 1)
    {
        return;
    }

    pa = 1;
    for (i = px[0]; i <= px[1] && pa == 1; i++)
        for (j = py[2]; j <= py[3]; j++)
            if (RANGE_X(i) && RANGE_Y(j))
                if (map->colisionMatrix[j][i] != '1')
                {
                    pa = 0;
                    break;
                }
    if (pa == 1)
    {
        obj->direction[1] = 0.0f;
        return;
    }

    pa = 1;
    for (i = px[2]; i <= px[3] && pa == 1; i++)
        for (j = py[0]; j <= py[1]; j++)
            if (RANGE_X(i) && RANGE_Y(j))
                if (map->colisionMatrix[j][i] != '1')
                {
                    pa = 0;
                    break;
                }
    if (pa == 1)
    {
        obj->direction[0] = 0.0f;
        return;
    }
    #undef RANGE_X
    #undef RANGE_Y
    // remove todo o movimento
    obj->direction[0] = 0.0f;
    obj->direction[1] = 0.0f;
}

void updateGun (GameObject * player, Map * map, double time)
{
    GameObject * gun = map->gun;
    float prev = gun->direction[0];
    if (player->direction[0] != 0.0f)
    {
        if (player->direction[0] > 0.0f) gun->direction[0] = 1.0f;
        else gun->direction[0] = -1.0f;
    }
    if (prev != gun->direction[0])
    {
        gun->quadList[0].pos.x = - gun->quadList[0].pos.x;
        gun->quadList[0].pos.width = -gun->quadList[0].pos.width;
    }

    if (prev == 1.0f) prev = 0.75;
    else prev = 0.25f;
    gun->pos.x = player->pos.x + (prev * player->quadList[0].pos.width);
    gun->pos.y = player->pos.y + 5;

    if (gun->updateObject != NULL) gun->updateObject (gun, time );
}

int _isIn (int * ptr, int num, int t)
{
    int i;
    for (i =0; i<t; i++) if (ptr[i] == num) return 1;
    return 0;
}

ObjectList * ObjectCollision (ObjectList * ptr, GameObject * self, int cont, int numTypes, ...)
{
    va_list listPointer;
    int i;
    int * types;
    float centerAX, centerAY, distA;
    float distP = 0.3f * sqrt (((self->quadList[0].pos.width) * (self->quadList[0].pos.width)) + ((self->quadList[0].pos.height) * (self->quadList[0].pos.height)));
    float centerPX = self->pos.x + self->quadList[0].pos.x + (self->quadList[0].pos.width/2);
    float centerPY = self->pos.y + self->quadList[0].pos.y + (self->quadList[0].pos.height/2);
    if (cont == 1) ptr = ptr->next;

    va_start(listPointer, numTypes);
    types = malloc (sizeof (int) * numTypes);
    for (i = 0; i < numTypes; i++) types[i] = va_arg(listPointer, int );
    va_end(listPointer);
    while (ptr != NULL)
    {
        if (ptr->obj != self && ptr->obj->hidden == 0 && _isIn(types, ptr->obj->type, numTypes) == 1 )
        {
            centerAX = ptr->obj->pos.x + ptr->obj->quadList[0].pos.x + (ptr->obj->quadList[0].pos.width/2) - centerPX;
            centerAY = ptr->obj->pos.y + ptr->obj->quadList[0].pos.y + (ptr->obj->quadList[0].pos.height/2) - centerPY;
            distA = 0.3f * sqrt (((ptr->obj->quadList[0].pos.width) * (ptr->obj->quadList[0].pos.width)) + ((ptr->obj->quadList[0].pos.height) * (ptr->obj->quadList[0].pos.height)));
            if (sqrt((centerAX * centerAX) + (centerAY * centerAY) ) <= distA + distP )
            {
            #define RANGE(X, Y) (( (X >= ptr->obj->pos.x + ptr->obj->quadList[0].pos.x && X <= ptr->obj->pos.x + ptr->obj->quadList[0].pos.x + ptr->obj->quadList[0].pos.width) \
                                || (X <= ptr->obj->pos.x + ptr->obj->quadList[0].pos.x && X >= ptr->obj->pos.x + ptr->obj->quadList[0].pos.x + ptr->obj->quadList[0].pos.width) ) \
                                && ((Y >= ptr->obj->pos.y + ptr->obj->quadList[0].pos.y && Y <= ptr->obj->pos.y + ptr->obj->quadList[0].pos.y + ptr->obj->quadList[0].pos.height) \
                                || (Y <= ptr->obj->pos.y + ptr->obj->quadList[0].pos.y && Y >= ptr->obj->pos.y + ptr->obj->quadList[0].pos.y + ptr->obj->quadList[0].pos.height) ))
                if (RANGE(self->pos.x + self->quadList[0].pos.x, self->pos.y + self->quadList[0].pos.y)
                    || RANGE(self->pos.x + self->quadList[0].pos.x + ptr->obj->quadList[0].pos.width, self->pos.y + self->quadList[0].pos.y)
                    || RANGE(self->pos.x + self->quadList[0].pos.x, self->pos.y + self->quadList[0].pos.y + ptr->obj->quadList[0].pos.height)
                    || RANGE(self->pos.x + self->quadList[0].pos.x + ptr->obj->quadList[0].pos.width, self->pos.y + self->quadList[0].pos.y + ptr->obj->quadList[0].pos.height))
                {
                    free (types);
                    return ptr;
                }
            #undef RANGE
            }
        }
        ptr = ptr->next;
    }
    free (types);
    return NULL;
}


void AddToProfile(const char * mapName)
{
    FILE * file;
    int size;
    file = fopen ("Data/profile.res", "ab");
    size = strlen(mapName);
    fwrite(&size, sizeof(int), 1, file);
    fwrite (mapName, sizeof(char), size, file);
    fclose(file);
}

void LoadUnlockProfile(GameState * gs)
{
    FILE * file;
    file = fopen ("Data/profile.res", "rb");
    if (file != NULL)
    {
        int size, i, y;
        char buffer[100];
        do
        {
            fread(&size, 4, 1, file);
            if (feof(file)) return; // se o fread de cima setar feof saimos, a operação de baixo nunca irá setar feof
            fread (buffer, 1, size, file);
            buffer[size] = 0;
            i = GetNextUnlockPos(buffer, gs->res, 99, 59);
            if (i != -1)
            {
                if (gs->res->unlockList[i].type == 1)
                {
                    for (y = 0; y < gs->res->numObjects; y++)
                        if (strcmp(gs->res->objectList[y].name, gs->res->unlockList[i].unlockName) == 0) break;
                    gs->res->objectList[y].hidden = 0;
                }
                else
                {
                    for (y = 0; y < gs->res->numMaps; y++)
                        if (strcmp(gs->res->mapList[y].name, gs->res->unlockList[i].unlockName) == 0) break;
                    gs->res->mapList[y].hidden = 0;
                }

                if ( i < gs->res->numUnlocks - 1) memmove(&gs->res->unlockList[i], &gs->res->unlockList[i+1], sizeof(Unlock) * (gs->res->numUnlocks - i));
                gs->res->numUnlocks -= 1;
                gs->res->unlockList = realloc(gs->res->unlockList, gs->res->numUnlocks * sizeof(Unlock));
            }
        } while (1);

        fclose(file);
    }
}

void SaveMap (Map * map)
{
    FILE * file;
    fpos_t pos2;
    int size, i, pos;
    ObjectList * ptr;
    file = fopen("Data/save.res", "wb");

    size = strlen(map->name);
    fwrite (&size, sizeof (int), 1, file);              // tamanho nome do mapa
    fwrite (map->name, sizeof(char), size, file );      // nome do mapa
    fwrite (&map->time, sizeof (float), 1, file);       // tempo
    fwrite (&map->counter, sizeof(float), 1, file);     // counter
    fwrite (&map->life, sizeof(int), 1, file);          // life
    fwrite (&map->ammo, sizeof(int), 1, file);          // ammo
    fwrite (&map->numMonsters, sizeof (int), 1, file);  // numMonsters
    fwrite (&map->numGuns, sizeof (int), 1, file);      // numGuns
    for (i = 0; i < map->numGuns; i++)
    {
        size = strlen (map->gunList[i].name);
        if (strcmp(map->gunList[i].name, map->gun->name) == 0) pos = i;
        fwrite (&size, sizeof(int), 1, file);                         // tamanho nome da arma
        fwrite (map->gunList[i].name, sizeof(char), size, file);      // nome da arma
        fwrite (map->gunList[i].direction, sizeof(float), 2, file);   // direcao arma (2 float)
        fwrite (&map->gunList[i].timeCounter, sizeof(float), 1, file); // contador
    }
    fwrite(&pos, sizeof(int), 1, file); // arma atual
    fgetpos(file, &pos2);
    pos = ftell (file);

    fwrite(&pos, sizeof(int), 1, file); // para guardar o numero de objetos posteiormente
    i = 0;
    ptr = map->objectListFirst;
    while (ptr != NULL)
    {
        size = strlen (ptr->obj->name);
        fwrite (&size, sizeof(int), 1, file);                  // tamanho do nome do objeto
        fwrite (ptr->obj->name, sizeof(char), size, file);     // nome
        fwrite (&ptr->obj->life, sizeof(int), 1, file);        // life
        fwrite (&ptr->obj->pos.x, sizeof (float), 1, file);    // pos.x
        fwrite (&ptr->obj->pos.y, sizeof (float), 1, file);    // pos.y
        fwrite (&ptr->obj->direction, sizeof (float), 2, file);// direction (2 float)
        fwrite (&ptr->obj->hidden, sizeof(int), 1, file);      // hidden
        i++;
        ptr = ptr->next;
    }
    rewind(file);
    fsetpos(file, &pos2);
    if (ftell(file) != pos) printf ("erro\n");
    fwrite(&i, sizeof(int), 1, file); // numero de objetos
    fclose (file);
}

void LoadMap (GameState * gs)
{
    FILE * file;
    Map * map;
    ObjectList * first, *last, *ptr;
    int size;
    int i, j;
    char buffer[100];
    file = fopen ("Data/save.res", "rb");
    if (file == NULL) return;
    fread(&size, sizeof(int), 1, file);
    if (feof(file) ||  ferror(file)) return;
    fread(buffer, sizeof(char), size, file);
    buffer[size] = 0;
    BuildMap(gs, buffer);
    FreeMap(gs->map); // isso irá limpar o object list e o gunlist
    map = gs->map;
    map->objectListFirst = NULL;
    map->objectListLast = NULL;
    #define READ(x, y, z) fread(x, sizeof(y), z, file)
    READ(&map->time, float, 1);
    READ(&map->counter, float, 1);
    READ(&map->life, int, 1 );
    for (i = map->life; i < 5; i++) gs->overlayList[0]->buttonList[i].hidden = 1;
    READ(&map->ammo, int, 1);
    READ(&map->numMonsters, int, 1);
    READ(&map->numGuns, int, 1);
    map->gunList = malloc (sizeof (GameObject) * map->numGuns);
    for (i = 0; i < map->numGuns; i++)
    {
        READ(&size, int, 1);
        READ(buffer, char, size);
        buffer[size] = 0;
        for (j = 0; j < gs->res->numObjects; j++)
            if (gs->res->objectList[j].type == 4)
                if (strcmp (gs->res->objectList[j].name, buffer) == 0) break;
        memcpy(&map->gunList[i], &gs->res->objectList[j], sizeof(GameObject));
        map->gunList[i].quadList = malloc (sizeof (Quad) * map->gunList[i].numQuads);
        memcpy(gs->map->gunList[i].quadList, gs->res->objectList[j].quadList, sizeof (Quad) * gs->map->gunList[i].numQuads );
        READ(map->gunList[i].direction, float, 2);
        if (map->gunList[i].direction[0] == -1.0f)
        {
            map->gunList[i].quadList[0].pos.x = -map->gunList[i].quadList[0].pos.x;
            map->gunList[i].quadList[0].pos.width = -map->gunList[i].quadList[0].pos.width;
        }
        READ(&map->gunList[i].timeCounter, float, 1);
    }
    READ(&size, int, 1);
    map->gun = &map->gunList[size];
    READ(&size, int, 1);
    if ( size > 0)
    {
        first = malloc (sizeof(ObjectList));
        first->next = NULL;
        first->prev = NULL;
        last = first;
    }
    for (i = 0; i < size - 1; i++)
    {
        last->next = malloc (sizeof(ObjectList));
        last->next->prev = last;
        last = last->next;
        last->next = NULL;
    }
    ptr = first;
    while (ptr != NULL)
    {
        int t;
        READ (&t, int, 1);
        READ (buffer, char, t);
        buffer[t] = 0;
        ptr->obj = GetResourceObjectCopy(buffer, gs->res);
        READ (&ptr->obj->life, int, 1);
        READ (&ptr->obj->pos.x, float, 1);
        READ (&ptr->obj->pos.y, float, 1);
        READ (ptr->obj->direction, float, 2);
        READ (&ptr->obj->hidden, int, 1);
        ptr = ptr->next;
    }
    map->objectListFirst = first;
    map->objectListLast = last;
    gs->keybuffer[2] = 1;
    fclose(file);
    if (feof(file) || ferror(file))
    {
        printf ("Erro ao carregar arquivo salvo\n");
        BuildMainMenu(gs);
        return;
    }
    #undef READ

    if (remove ("Data/save.res") != 0) fclose(fopen("Data/save.res", "w"));
}

void FreeGameState(GameState * gs)
{
    int i, j;

    if (gs->map != NULL)
    {
        FreeMap (gs->map);
        gs->map = NULL;
    }

    if (gs->numOverlays > 0)
    {
        free (gs->overlayList);
    }

    for (i = 0; i < gs->res->numTextures; i++)
    {
        if (gs->res->textureList[i].Buffer != 0) UnbufferTexture(&gs->res->textureList[i]);
        if (gs->res->textureList[i].Data != NULL) FreeTexture(&gs->res->textureList[i]);
        if (gs->res->textureList[i].file != NULL) free (gs->res->textureList[i].file);
        if (gs->res->textureList[i].name != NULL) free (gs->res->textureList[i].name);
    }
    if (gs->res->numTextures > 0) free (gs->res->textureList);

    for (i = 0; i < gs->res->numFonts; i++)
    {
        if (gs->res->fontList[i].fontTexture != NULL)
        {
            if (gs->res->fontList[i].fontTexture->Buffer != 0) UnbufferTexture(&gs->res->textureList[i]);
            if (gs->res->fontList[i].fontTexture->name != NULL) free (gs->res->textureList[i].name);
        }
        if (gs->res->fontList[i].name != NULL) free(gs->res->fontList[i].name);
        glDeleteLists(gs->res->fontList[i].listBase, 256);
    }
    if (gs->res->numFonts > 0) free (gs->res->fontList);

    for (i = 0; i < gs->res->numOverlays; i++)
    {
        if (gs->res->overlayList[i].name != NULL) free (gs->res->overlayList[i].name);
        if (gs->res->overlayList[i].numQuads > 0) free (gs->res->overlayList[i].quadList);
        for (j = 0; j < gs->res->overlayList[i].numTexts; j++)
        {
            if (gs->res->overlayList[i].textList[j].name != NULL) free(gs->res->overlayList[i].textList[j].name);
            if (gs->res->overlayList[i].textList[j].string != NULL) free (gs->res->overlayList[i].textList[j].string);
        }
        if (gs->res->overlayList[i].numTexts > 0) free (gs->res->overlayList[i].textList);

        for (j = 0; j < gs->res->overlayList[i].numButtons; j++)
        {
            int k;
            if (gs->res->overlayList[i].buttonList[j].name != NULL) free(gs->res->overlayList[i].buttonList[j].name);
            if (gs->res->overlayList[i].buttonList[j].numQuads > 0) free (gs->res->overlayList[i].buttonList[j].quadList);
            for (k = 0; k < gs->res->overlayList[i].buttonList[j].numTexts; k++)
            {
                if (gs->res->overlayList[i].buttonList[j].textList[k].name != NULL) free(gs->res->overlayList[i].buttonList[j].textList[k].name);
                if (gs->res->overlayList[i].buttonList[j].textList[k].string != NULL) free (gs->res->overlayList[i].buttonList[j].textList[k].string);
            }
            if (gs->res->overlayList[i].buttonList[j].numTexts > 0) free (gs->res->overlayList[i].buttonList[j].textList);
        }
        if (gs->res->overlayList[i].numButtons > 0) free (gs->res->overlayList[i].buttonList);
    }
    if (gs->res->numOverlays > 0) free (gs->res->overlayList);

    for (i = 0; i < gs->res->numObjects; i++)
    {
        if (gs->res->objectList[i].name != NULL) free (gs->res->objectList[i].name);
        if (gs->res->objectList[i].numQuads > 0) free (gs->res->objectList[i].quadList);
    }
    if (gs->res->numObjects > 0) free (gs->res->objectList);

    for (i = 0; i < gs->res->numUnlocks; i++)
    {
        if (gs->res->unlockList[i].mapName != NULL) free (gs->res->unlockList[i].mapName);
        if (gs->res->unlockList[i].string != NULL)  free (gs->res->unlockList[i].string);
        if (gs->res->unlockList[i].unlockName != NULL) free (gs->res->unlockList[i].unlockName);
    }
    if (gs->res->numUnlocks > 0) free (gs->res->unlockList);

    for (i = 0; i < gs->res->numMaps; i++)
    {
        if (gs->res->mapList[i].name != NULL) free (gs->res->mapList[i].name);
        if (gs->res->mapList[i].string != NULL) free (gs->res->mapList[i].string);
        if (gs->res->mapList[i].back != NULL)
        {
            if (gs->res->mapList[i].back->numQuads > 0) free (gs->res->mapList[i].back->quadList);
            free (gs->res->mapList[i].back);
        }

        for (j = 0; j < gs->res->mapList[i].maxY; j++)
            free (gs->res->mapList[i].colisionMatrix[j]);
        if (gs->res->mapList[i].maxY > 0) free (gs->res->mapList[i].colisionMatrix);

        for (j = 0; j < gs->res->mapList[i].numMonsterTypes; j++)
            free (gs->res->mapList[i].monsterTypeList[j]);
        if (gs->res->mapList[i].numMonsterTypes > 0) free (gs->res->mapList[i].monsterTypeList);
    }
    if (gs->res->numMaps > 0) free (gs->res->mapList);
    free (gs->res);
    free (gs);
}


