#include "CallFunctions.h"
#include "GameInclude.h"
#include "string.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

#define itoa(x, s, y) itoa_a(x, s, y)
// itoa nao eh definido em linux
char* itoa_a(int value, char* str, int radix)
{
    static char dig[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz";
    int n = 0, neg = 0;
    unsigned int v;
    char* p, *q;
    char c;
    if (radix == 10 && value < 0)
    {
        value = -value;
        neg = 1;
    }
    v = value;
    do
    {
        str[n++] = dig[v%radix];
        v /= radix;
    } while (v);
    if (neg)
        str[n++] = '-';
    str[n] = '\0';
    for (p = str, q = p + (n-1); p < q; ++p, --q)
        c = *p, *p = *q, *q = c;
    return str;
}


void InputMainMenu()
{
    int x, y, i;
    Coord pos;
    glfwGetMousePos (&x, &y);
    pos.x = x;
    pos.y = y;

    x = 0;
    y = 0;

    for (i = 0; i < curGS->numOverlays; i ++)
    {
        y = ClickOverlay (pos, curGS->overlayList[i], 0, &x);
        if ((y!= 0)) break;
    }

    if (glfwGetMouseButton (GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if ( curGS->keybuffer[0] == 0) curGS->keybuffer [0] = 1;
    }
    else if (curGS->keybuffer[0] == 1)
    {
        if (y == 2) if (curGS->overlayList[i]->buttonList[x].onClick != NULL) curGS->overlayList[i]->buttonList[x].onClick (curGS->overlayList[i]->buttonList[x].Parameters, NULL);
        curGS->keybuffer[0] = 0;
    }
    else if (y == 2)
    {
        if (curGS->overlayList[i]->buttonList[x].numQuads > 0)
        {
            curGS->overlayList[2]->pos.x = curGS->overlayList[i]->pos.x +  curGS->overlayList[i]->buttonList[x].pos.x + curGS->overlayList[i]->buttonList[x].quadList[0].pos.x;
            curGS->overlayList[2]->pos.y = curGS->overlayList[i]->pos.y +  curGS->overlayList[i]->buttonList[x].pos.y + curGS->overlayList[i]->buttonList[x].quadList[0].pos.y;
            curGS->overlayList[2]->quadList[0].pos.width = curGS->overlayList[i]->buttonList[x].quadList[0].pos.width;
            curGS->overlayList[2]->quadList[0].pos.height = curGS->overlayList[i]->buttonList[x].quadList[0].pos.height;
        }
        else
        {
            curGS->overlayList[2]->pos.x = curGS->overlayList[i]->pos.x +  curGS->overlayList[i]->buttonList[x].pos.x ;
            curGS->overlayList[2]->pos.y = curGS->overlayList[i]->pos.y +  curGS->overlayList[i]->buttonList[x].pos.y ;
            curGS->overlayList[2]->quadList[0].pos.width = curGS->overlayList[i]->buttonList[x].pos.width;
            curGS->overlayList[2]->quadList[0].pos.height = curGS->overlayList[i]->buttonList[x].pos.height;
        }
        curGS->overlayList[2]->hidden = 0;
    }
    else curGS->overlayList[2]->hidden = 1;
}
void OnClickMainMenuButton(int * p, void * null)
{
    int i;
    switch (p[0])
    {
        case 1:

            for (i = 0; i < curGS->res->numMaps; i++)
                if (curGS->res->mapList[i].hidden == 0) break;
            if (i != curGS->res->numMaps) // mapa disponivel
            {
                curGS->overlayList[0]->buttonList[1].Parameters[1] = i;
                curGS->overlayList[0]->quadList[1].tex = curGS->res->mapList[i].back->quadList[0].tex;
                curGS->overlayList[0]->quadList[1].isTextured = 1;
                curGS->overlayList[0]->buttonList[3].hidden = 1;
                curGS->overlayList[0]->buttonList[2].hidden = 1;
                for (i += 1; i <curGS->res->numMaps; i++)
                    if (curGS->res->mapList[i].hidden == 0)
                    {
                        curGS->overlayList[0]->buttonList[2].hidden = 0;
                        break;
                    }
                curGS->overlayList[0]->hidden = 0;
            }
            break;
        case 2:
            LoadMap(curGS);
            break;
        case 3:
            curGS->end = 1;
    }
}


void OnClickDemoButton(int * p, void * null)
{
    if (p[0] == 1)
    {
        BuildMap (curGS, "map1");
    }

}

void OnClickInGameMenu(int * p, void * null)
{
    switch (p[0])
    {
        case 1:         // continue
            curGS->map->lock = 0;
            curGS->overlayList[1]->hidden = 1;
            break;
        case 2:
            SaveMap(curGS->map);
        case 3:
            BuildMainMenu(curGS);
            break;
        case 4:
            BuildMap(curGS, curGS->map->name);
            break;
    }
}

void OnClickMapSelector(int * p, void * null)
{
    int i;
    switch (p[0])
    {
        case 1: // fecha o seletor
            curGS->overlayList[0]->hidden = 1;
            break;
        case 2: // inicia o mapa
            BuildMap(curGS, curGS->res->mapList[p[1]].name);
            break;
        case 3: // proximo mapa
            i = curGS->overlayList[0]->buttonList[1].Parameters[1];
            for (i += 1; i < curGS->res->numMaps; i++)
                if (curGS->res->mapList[i].hidden == 0)
                {
                    curGS->overlayList[0]->buttonList[1].Parameters[1] = i;
                    curGS->overlayList[0]->quadList[1].tex = curGS->res->mapList[i].back->quadList[0].tex;
                    curGS->overlayList[0]->quadList[1].isTextured = 1;
                    break;
                }
            curGS->overlayList[0]->buttonList[3].hidden = 0; // sabemos que tem um mapa anterior pelo menos
            curGS->overlayList[0]->buttonList[2].hidden = 1;
            for (i += 1; i < curGS->res->numMaps; i++)
                if (curGS->res->mapList[i].hidden == 0)
                {
                    curGS->overlayList[0]->buttonList[2].hidden = 0;
                    break;
                }

            break;
        case 4: // mapa anterior
            i = curGS->overlayList[0]->buttonList[1].Parameters[1];
            for (i -= 1; i >= 0; i--)
                if (curGS->res->mapList[i].hidden == 0)
                {
                    curGS->overlayList[0]->buttonList[1].Parameters[1] = i;
                    curGS->overlayList[0]->quadList[1].tex = curGS->res->mapList[i].back->quadList[0].tex;
                    curGS->overlayList[0]->quadList[1].isTextured = 1;
                    break;
                }
            curGS->overlayList[0]->buttonList[3].hidden = 1;
            curGS->overlayList[0]->buttonList[2].hidden = 0; // sabemos que tem um mapa depois pelo menos
            for (i -= 1; i >= 0; i--)
                if (curGS->res->mapList[i].hidden == 0)
                {
                    curGS->overlayList[0]->buttonList[3].hidden = 0;
                    break;
                }
            break;
    }
}


void RenderMap()
{
    int i;
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    DrawMap(curGS->map);
    for (i = 0; i < curGS->numOverlays; i++) if (curGS->overlayList[i]->hidden == 0) DrawOverlay (curGS->overlayList[i]);
    glfwSwapBuffers();
}

void UpdatePlayer(void * ptr, double time)
{
    ObjectList * ol = (ObjectList*) ptr;
    GameObject * self = ol->obj;
    self->direction[1] +=  (float) (1200.0f * time);
    if (self->direction[1] > 600.0f) self->direction[1] = 600.0f;
    self->direction[0] = 0.0f;
    if (glfwGetKey(GLFW_KEY_LEFT) == 1) self->direction[0] -= self->speed;
    if (glfwGetKey(GLFW_KEY_RIGHT) == 1) self->direction[0] += self->speed;
    MapCollision(curGS->map, self, time);
    if (glfwGetKey(GLFW_KEY_UP) == 1 )
    {
        if (self->direction[1] == 0 && curGS->keybuffer[0] != 1)
        {
            self->direction[1] -= 600.0f;
            MapCollision(curGS->map, self, time);
            curGS->keybuffer[0] = 1;
        }
    }
    else if (curGS->keybuffer[0] == 1)
    {
        if (self->direction[1] < 0.0f) self->direction[1] = 0.0f;
        curGS->keybuffer[0] = 0;
    }
    self->pos.y += self->direction[1] * time;
    if (self->direction[1] != 0.0f)self->pos.y -= 0.0001; // arruma um erro de aproximação que faz ficar preso no chão
    self->pos.x += self->direction[0] * time;
    if (self->pos.y  > 650.0f) curGS->map->end = 1;

    if (curGS->map->gun != NULL) updateGun (self, curGS->map, time);
}

void UpdateMap()
{
    int min, sec;
    double time = glfwGetTime();
    glfwSetTime(0.0);
    min = curGS->map->time / 60;
    sec = (int)curGS->map->time % 60;
    if (min >= 100) min = 99;

    if (curGS->map->lock == 0 && time < 0.15f) // jogo está rodando
    {
        ObjectList * ptr = curGS->map->objectListFirst;
        ObjectList * aux;

        curGS->map->time += time;
        curGS->map->counter -= time;

        // atualiza cada objeto
        while (ptr != NULL)
        {
            aux = ptr->next;
            if (ptr->obj->updateObject != NULL) ptr->obj->updateObject(ptr, time);
            ptr = aux;
        }
        // agora que tudo está posicionado no frame, busca colisões
        ptr = curGS->map->objectListFirst;
        while (ptr != NULL)
        {
            if (ptr->obj->hidden != 1)
            {
                if (ptr->obj->collisionFunction != NULL)ptr = ptr->obj->collisionFunction(ptr);
                else ptr = ptr->next;
            }
            else ptr = ptr->next;
        }


        if ((curGS->map->counter <= 0.0f || curGS->map->numMonsters == 0) && curGS->map->numMonsters < 1000)
        {
            curGS->map->counter = (rand() % 4) + 0.5f - (curGS->map->time/60.0f);
            if (curGS->map->counter < 0.5f) curGS->map->counter = 0.5f;
            if (rand() % 100 < 60)
            {
                ObjectList * ol;
                int i, t;
                for (i = 1, t = 0; i <= curGS->map->numMonsterTypes; t += i , i++);
                int n = rand() % 100;
                i = 0;
                do
                {
                    i++;
                    n -= (int) (100 / t) * (t - i);
                } while (n > 0);
                i--;
                ol = malloc (sizeof (ObjectList));
                ol->obj = GetResourceObjectCopy(curGS->map->monsterTypeList[i], curGS->res);
                ol->prev = curGS->map->objectListLast;
                ol->next = NULL;
                ol->obj->pos.x = curGS->map->dropPos.x - (ol->obj->quadList[0].pos.width/2);
                ol->obj->pos.y = curGS->map->dropPos.y;
                ol->obj->hidden = 0;
                curGS->map->objectListLast->next = ol;
                curGS->map->objectListLast = ol;
                curGS->map->numMonsters++;
            }
        }


        if (min < 10) itoa(min, &curGS->overlayList[0]->textList[0].string[1], 10);
        else itoa(min, curGS->overlayList[0]->textList[0].string, 10);
        if (sec < 10)
        {
            itoa(sec, &curGS->overlayList[0]->textList[0].string[4], 10);
            curGS->overlayList[0]->textList[0].string[3] = '0';
        }
        else itoa (sec, &curGS->overlayList[0]->textList[0].string[3], 10);
        curGS->overlayList[0]->textList[0].string[5] = '\'';
        curGS->overlayList[0]->textList[0].string[2] = '\'';

         itoa(curGS->map->ammo, curGS->overlayList[0]->textList[1].string, 10);
    }
    else if (curGS->map->lock == 1)
    {
        if (glfwGetMouseButton (GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            curGS->keybuffer [3] = 1;
        }
        else if (curGS->keybuffer[3] == 1)
        {
            int i, x, y;
            Coord pos;
            glfwGetMousePos (&x, &y);
            pos.x = x;
            pos.y = y;
            y = 0;
            for (i = 1; i < curGS->numOverlays; i ++)
            {
                y = ClickOverlay (pos, curGS->overlayList[i], 1, NULL);
                if ((y!= 0)) break;
            }
            curGS->keybuffer[3] = 0;
            if (y!= 0)  return;
        }
    }

    if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS) curGS->keybuffer[2] = 1;
    else if (curGS->keybuffer[2] == 1 )
    {
        if (curGS->overlayList[1]->hidden == 0 && curGS->map->lock == 1 )
        {
            curGS->map->lock = 0;
            curGS->overlayList[1]->hidden = 1;

        }
        else if (curGS->overlayList[1]->hidden == 1 && curGS->map->lock == 0)
        {
            curGS->map->lock = 1;
            curGS->overlayList[1]->hidden = 0;
        }
        curGS->keybuffer[2] = 0;
    }

    if (curGS->map->end != 0)
    {
        int y;
        curGS->map->lock = 1;
        curGS->overlayList[1]->hidden = 1;
        curGS->overlayList[2]->hidden = 0;
        sprintf(curGS->overlayList[2]->textList[0].string, "%.2d'%.2d''", min, sec);
        curGS->map->end = 0;

        int i = GetNextUnlockPos(curGS->map->name, curGS->res, min, sec);
        if (i != -1)
        {
            if (curGS->res->unlockList[i].type == 1)
            {
                for (y = 0; y < curGS->res->numObjects; y++)
                    if (strcmp(curGS->res->objectList[y].name, curGS->res->unlockList[i].unlockName) == 0) break;
                curGS->res->objectList[y].hidden = 0;
                sprintf (curGS->overlayList[3]->textList[0].string, "Nova arma: %s", curGS->res->unlockList[i].string);
            }
            else
            {
                for (y = 0; y < curGS->res->numMaps; y++)
                    if (strcmp(curGS->res->mapList[y].name, curGS->res->unlockList[i].unlockName) == 0) break;
                curGS->res->mapList[y].hidden = 0;
                sprintf (curGS->overlayList[3]->textList[0].string, "Nova mapa: %s", curGS->res->unlockList[i].string);
            }

            AddToProfile(curGS->res->unlockList[i].mapName);
            curGS->overlayList[3]->hidden = 0;
            if ( i < curGS->res->numUnlocks - 1) memmove(&curGS->res->unlockList[i], &curGS->res->unlockList[i+1], sizeof(Unlock) * (curGS->res->numUnlocks - i));
            curGS->res->numUnlocks -= 1;
            curGS->res->unlockList = realloc(curGS->res->unlockList, curGS->res->numUnlocks * sizeof(Unlock));
        }
    }
}

void UpdateSimpleEnemy (void * ptr, double time)
{
    float f;
    ObjectList * ol = (ObjectList*) ptr;
    GameObject * self = ol->obj;
    if (self->direction[0] == 0)
    {
        if (rand() % 2 == 0)self->direction[0] = self->speed;
        else self->direction[0] = -self->speed;
    }
    self->direction[1] +=  (float) (1200.0f * time);
    if (self->direction[1] > 600.0f) self->direction[1] = 600.0f;
    f = self->direction[0];
    MapCollision(curGS->map, self, time);
    self->pos.y += self->direction[1] * time;
    if (self->direction[1] != 0.0f)self->pos.y -= 0.0001; // arruma um erro de aproximação que faz ficar preso no chão
    self->pos.x += self->direction[0] * time;
    if (self->direction[0] == 0 && self->direction[1] == 0) self->direction[0] = -f;
    else self->direction[0] = f;
    if (self->pos.y  > 650.0f)
    {
        if(ol->prev != NULL) ol->prev->next = ol->next;
        if(ol->next != NULL) ol->next->prev = ol->prev;
        if (curGS->map->objectListFirst == ol) curGS->map->objectListFirst = ol->next;
        if (curGS->map->objectListLast == ol)
        {
            curGS->map->objectListLast = ol->prev;
            if (ol->prev != NULL) ol->prev->next = NULL;
        }
        free (ol->obj->quadList);
        free (ol->obj);
        free (ol);
        curGS->map->numMonsters--;
        curGS->map->life--;
        curGS->overlayList[0]->buttonList[curGS->map->life].hidden = 1;
        if (curGS->map->life == 0) curGS->map->end = 1;
   }
}

void UpdatePistol (void * ptr, double time)
{
    GameObject * self = ptr;
    self->timeCounter -= time;
    if (glfwGetKey(GLFW_KEY_SPACE) == 1  )
    {
        if (self->timeCounter <= 0.0f && curGS->keybuffer[1] == 0 && curGS->map->ammo > 0)
        {
            // atira
            ObjectList * ol = malloc (sizeof (ObjectList));
            ol->obj = GetResourceObjectCopy("bullet", curGS->res);
            ol->obj->direction[0] = 1000.0f * self->direction[0];
            ol->obj->direction[1] = 0.0f;
            ol->obj->pos.x = self->pos.x +  self->quadList[0].pos.x + self->quadList[0].pos.width + ((-self->direction[0]) * ol->obj->quadList[0].pos.width / 2);
            ol->obj->pos.y = self->pos.y + 3;
            ol->obj->hidden = 0;
            ol->obj->life = self->life;
            ol->next = NULL;
            ol->prev = curGS->map->objectListLast;
            curGS->map->objectListLast->next = ol;
            curGS->map->objectListLast = ol;
            curGS->map->ammo -= 1;
            self->timeCounter = 0.2f;
            curGS->keybuffer[1] = 1;
        }

    }
    else if (curGS->keybuffer[1] == 1) curGS->keybuffer[1] = 0;
}

void UpdateDualPistol (void * ptr, double time)
{
    GameObject * self = ptr;
    self->timeCounter -= time;
    if (glfwGetKey(GLFW_KEY_SPACE) == 1  )
    {
        if (self->timeCounter <= 0.0f && curGS->keybuffer[1] == 0 && curGS->map->ammo > 0)
        {
            // atira
            ObjectList * ol = malloc (sizeof (ObjectList));
            ol->obj = GetResourceObjectCopy("bullet", curGS->res);
            ol->obj->direction[0] = 1000.0f * self->direction[0];
            ol->obj->direction[1] = 0.0f;
            ol->obj->pos.x = self->pos.x +  self->quadList[0].pos.x + self->quadList[0].pos.width + ((-self->direction[0]) * ol->obj->quadList[0].pos.width / 2);
            ol->obj->pos.y = self->pos.y + 3;
            ol->obj->hidden = 0;
            ol->obj->life = self->life;
            ol->next = NULL;
            ol->prev = curGS->map->objectListLast;
            curGS->map->objectListLast->next = ol;
            curGS->map->objectListLast = ol;

            ol = malloc (sizeof (ObjectList));
            ol->obj = GetResourceObjectCopy("bullet", curGS->res);
            ol->obj->direction[0] = 1000.0f * ( -self->direction[0]);
            ol->obj->direction[1] = 0.0f;
            ol->obj->pos.x = self->pos.x +  self->quadList[0].pos.x + ((-self->direction[0]) * ol->obj->quadList[0].pos.width / 2);
            ol->obj->pos.y = self->pos.y + 3;
            ol->obj->hidden = 0;
            ol->obj->life = self->life;
            ol->next = NULL;
            ol->prev = curGS->map->objectListLast;
            curGS->map->objectListLast->next = ol;
            curGS->map->objectListLast = ol;
            curGS->map->ammo -= 2;
            self->timeCounter = 0.2f;
            curGS->keybuffer[1] = 1;
        }

    }
    else if (curGS->keybuffer[1] == 1) curGS->keybuffer[1] = 0;
}

void UpdateBullet(void * ptr, double time)
{
    ObjectList * ol = (ObjectList*) ptr;
    GameObject * self = ol->obj;
    float x, y;
    x = self->direction[0];
    y = self->direction[1];

    MapCollision(curGS->map, self, time);
    if (self->hidden == 1) self->timeCounter -= time;
    if (x != self->direction[0] || y != self->direction[1] || (self->hidden == 1 && self->timeCounter <= 0.0f))
    {
        if(ol->prev != NULL) ol->prev->next = ol->next;
        if(ol->next != NULL) ol->next->prev = ol->prev;
        if (curGS->map->objectListFirst == ol) curGS->map->objectListFirst = ol->next;
        if (curGS->map->objectListLast == ol)
        {
            curGS->map->objectListLast = ol->prev;
            if (ol->prev != NULL) ol->prev->next = NULL;
        }
        free (ol->obj->quadList);
        free (ol->obj);
        free (ol);
    }
    else
    {
        self->pos.y += self->direction[1] * time;
        self->pos.x += self->direction[0] * time;
    }
}


void * CollisionPlayer (void * ptr)
{
    ObjectList * os = ptr;
    GameObject * self = os->obj;
    ObjectList * ol = curGS->map->objectListFirst;
    ol = ObjectCollision(ol, self, 0, 2, 2, 5 );
    while (ol != NULL)
    {
        ObjectList * aux = ObjectCollision (ol, self, 1, 1, 2);

        if (ol->obj->type == 5) // jogador pegou uma caixa
        {
            int i = 0;
            int x, y;
            do
            {
                int j;
                x = rand() % curGS->map->maxX;
                y = (rand() % (curGS->map->maxY - 1)) + 1;
                for (j = y; j < curGS->map->maxY; j++)
                    if (curGS->map->colisionMatrix[j][x] == '0' && curGS->map->colisionMatrix[j - 1][x] == '1') break;
                if (j < curGS->map->maxY)
                {
                    i = 1;
                    y = j;
                }
            } while (i == 0);

            ol->obj->pos.x = x * (WINDOW_WIDTH / curGS->map->maxX) + (((WINDOW_WIDTH / curGS->map->maxX) - ol->obj->quadList[0].pos.width) / 2);
            ol->obj->pos.y = y * (WINDOW_HEIGHT / curGS->map->maxY) - ol->obj->quadList[0].pos.height;
            // TODO: criar um objeto "smoke" pra esconder a caixa aparecendo do nada e a arma trocando
            do
            {
            i = rand() % curGS->map->numGuns;
            } while (&curGS->map->gunList[i] == curGS->map->gun);
            curGS->map->gun = &curGS->map->gunList[i];
            curGS->map->ammo = curGS->map->gun->speed;

            i = rand() % 100;
            if (i > 39 && curGS->map->life < 5) curGS->overlayList[0]->buttonList[curGS->map->life++].hidden = 0;
        }
        else if (ol->obj->type == 2)
        {
            curGS->map->end = 1;
        }
        ol = aux;
    }

    return os->next;
}

void * CollisionBullet (void * ptr)
{
    ObjectList * os = ptr;
    GameObject * self = os->obj;
    ObjectList * ol = curGS->map->objectListFirst;
    ObjectList * next = os->next;
    ol = ObjectCollision(ol, self, 0, 1, 2 );
    if (ol != NULL)
    {
        ol->obj->life -= self->life;
        os->obj->hidden = 1;
        os->obj->timeCounter =  ((rand() % 2)) * 0.008f;

        if (ol->obj->life <= 0)
        {
            if (next == ol) next = ol->next;
            if(ol->prev != NULL) ol->prev->next = ol->next;
            if(ol->next != NULL) ol->next->prev = ol->prev;
            if (curGS->map->objectListFirst == ol) curGS->map->objectListFirst = ol->next;
            if (curGS->map->objectListLast == ol)
            {
                curGS->map->objectListLast = ol->prev;
                if (ol->prev != NULL) ol->prev->next = NULL;
            }
            free (ol->obj->quadList);
            free (ol->obj);
            free (ol);
        }
    }
    return next;
}

void UpdateShotgun (void * ptr, double time)
{
    GameObject * self = ptr;
    self->timeCounter -= time;
    if (glfwGetKey(GLFW_KEY_SPACE) == 1  )
    {
        if (self->timeCounter <= 0.0f  && curGS->keybuffer[1] == 0 && curGS->map->ammo > 0 )
        {
            // atira
            int i = 6 + (rand() % 6);
            ObjectList * last;
            ObjectList * ol = malloc (sizeof (ObjectList));
            last = ol;
            ol->obj = GetResourceObjectCopy("bullet", curGS->res);
            ol->obj->direction[0] = 500.0f * self->direction[0];
            ol->obj->direction[1] = (rand() % 150) - 100.0f;
            ol->obj->pos.x = self->pos.x +  self->quadList[0].pos.x + self->quadList[0].pos.width + ((-self->direction[0]) * ol->obj->quadList[0].pos.width / 2);
            ol->obj->pos.y = self->pos.y + 6;

            ol->obj->hidden = 0;
            ol->obj->life = self->life;
            ol->next = NULL;
            ol->prev = curGS->map->objectListLast;
            curGS->map->objectListLast->next = ol;
            do
            {
                ol = malloc (sizeof(ObjectList));
                ol->obj = GetResourceObjectCopy("bullet", curGS->res);
                ol->obj->direction[0] = (500.0f + (rand() % 171) - 80)* self->direction[0]; // 171 escolhido por marsupa no dia 6/6/2012 as 18:12
                ol->obj->direction[1] = (rand() % 150) - 100.0f;
                ol->obj->pos.x = self->pos.x +  self->quadList[0].pos.x + self->quadList[0].pos.width + ((-self->direction[0]) * ol->obj->quadList[0].pos.width / 2);
                ol->obj->pos.y = self->pos.y + 6;
                ol->obj->hidden = 0;
                ol->obj->life = self->life;
                ol->next = NULL;
                last->next = ol;
                ol->prev = last;
                last = ol;
                i--;
            } while (i > 0);
            curGS->map->objectListLast = last;

            curGS->map->ammo -= 1;
            self->timeCounter = 0.6f;
            curGS->keybuffer[1] = 1;
        }

    }
    else if (curGS->keybuffer[1] == 1) curGS->keybuffer[1] = 0;
}

void UpdateRifle (void * ptr, double time)
{
    GameObject * self = ptr;
    self->timeCounter -= time;
    if (glfwGetKey(GLFW_KEY_SPACE) == 1  )
    {
        if (self->timeCounter <= 0.0f && curGS->map->ammo > 0)
        {
            // atira
            ObjectList * ol = malloc (sizeof (ObjectList));
            ol->obj = GetResourceObjectCopy("bullet", curGS->res);
            ol->obj->direction[0] = 1000.0f * self->direction[0];
            ol->obj->direction[1] = (rand() % 80) - 40.0f;
            ol->obj->pos.x = self->pos.x +  self->quadList[0].pos.x + self->quadList[0].pos.width + ((-self->direction[0]) * ol->obj->quadList[0].pos.width / 2);
            ol->obj->pos.y = self->pos.y + 3;
            ol->obj->hidden = 0;
            ol->obj->life = self->life;
            ol->next = NULL;
            ol->prev = curGS->map->objectListLast;
            curGS->map->objectListLast->next = ol;
            curGS->map->objectListLast = ol;
            curGS->map->ammo -= 1;
            self->timeCounter = 0.05f;
            curGS->keybuffer[1] = 1;
        }

    }
    else if (curGS->keybuffer[1] == 1) curGS->keybuffer[1] = 0;
}
