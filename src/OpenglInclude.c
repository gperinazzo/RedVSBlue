#include "OpenglInclude.h"
#include "CallFunctions.h"
#include <stdio.h>
#include <string.h>

void InitAll()
{
    // inicializa o glfw
    if( !glfwInit() )
    {
        exit( EXIT_FAILURE );
    }

    // Ajusta a versao do opengl para 2.1
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 1);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 5);
    // impede que o tamanho da janela seja modificado
    glfwOpenWindowHint (GLFW_WINDOW_NO_RESIZE, GL_TRUE);

    // Abre a janela (através do glfw)
    if( !glfwOpenWindow( WINDOW_WIDTH, WINDOW_HEIGHT, 0,0,0,0,32,0, GLFW_WINDOW) )
    {
        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    glfwSetWindowTitle("Red vs Blue");
    // Inicializa o glew (OpenGL expansion wrangler)
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        exit (EXIT_FAILURE);
    }
    // Algumas coisas padrões do OpenGL
    glClearColor (1.0, 1.0, 1.0, 0.0);
    glClearDepth (1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);

    //arruma a matriz de projeção
    glViewport (0, 0, 800, 600);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

    //deixa na modelview matrix
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}


int LoadTexture (const char * file, Texture * target)
{
    if (target->Data == NULL) target->Data = malloc (sizeof (GLFWimage));
    if (glfwReadImage (file, target->Data, GLFW_ORIGIN_UL_BIT) != GL_TRUE)
    {
        free (target->Data);
        target->Data = NULL;
        return 0;
    }
    target->Buffer = 0;
    return 1;
}

void BufferTexture (Texture * tex)
{
    if (tex->Data == NULL)
    {
        if (tex->file != NULL)
            {
                if (LoadTexture (tex->file, tex) != 1 ) printf ("Problema ao ler textura");
            }
        else return;
    }
    glGenTextures (1, &tex->Buffer);
    glBindTexture (GL_TEXTURE_2D, tex->Buffer);
    glTexImage2D (GL_TEXTURE_2D, 0, 4, tex->Data->Width, tex->Data->Height, 0, tex->Data->Format, GL_UNSIGNED_BYTE, tex->Data->Data );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void UnbufferTexture (Texture * tex)
{
    glDeleteTextures (1, &tex->Buffer);
    tex->Buffer = 0;
}

//Libera a memoria armazenada para a textura
void FreeTexture (Texture * tex)
{
    glfwFreeImage (tex->Data);
    tex->Data =NULL;
}

void DrawOverlay (Overlay * ov)
{
    int i;
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix();
    glTranslatef (ov->pos.x, ov->pos.y, ov->z);
    if (ov->numQuads > 0)
    {
        DrawQuads (ov->numQuads, ov->quadList);
    }
    glTranslatef (0, 0, 0.05);
    for (i = 0; i < ov->numTexts; i++)
        if (ov->textList[i].hidden == 0)
            DrawText (&ov->textList[i]);
    glTranslatef (0, 0, 0.05f);
    for (i = 0; i < ov->numButtons; i++)
    {
        int j;
        if (ov->buttonList[i].hidden == 1) continue;
        glPushMatrix();
        glTranslatef (ov->buttonList[i].pos.x, ov->buttonList[i].pos.y, 0);
        if (ov->buttonList[i].numQuads > 0) DrawQuads (ov->buttonList[i].numQuads, ov->buttonList[i].quadList);
        glTranslatef (0, 0, 0.05f);
        for (j = 0; j < ov->buttonList[i].numTexts; j++)
            if (ov->buttonList[i].textList[j].hidden == 0)
                DrawText (&ov->buttonList[i].textList[j]);

        glPopMatrix();
    }
    glPopMatrix();
}

int ClickOverlay (Coord pos, Overlay * ov, int event, int * button)
{
    int i, j, aux = 0;
    Coord box;
    if (ov->hidden == 0)
    {
        //verifica se é algum botão
        for (i = 0; i < ov->numButtons; i++, aux = 0)
        {
            if (ov->buttonList[i].hidden == 1) continue;
            if (ov->buttonList[i].numQuads == 0)
            {    // nenhum retangulo visivel, verifica se está dentro do tamanho padrão do botão
                box.x = ov->pos.x + ov->buttonList[i].pos.x;
                box.y = ov->pos.y + ov->buttonList[i].pos.y;
                box.width = ov->buttonList[i].pos.width;
                box.height = ov->buttonList[i].pos.height;
                if (isInside(pos, box))
                {
                    aux = 1;
                    if (button != NULL) button[0] = i;
                }
            }
            else for (j=0; j< ov->buttonList[i].numQuads; j++ )
            {   // verifica se esta em algum dos retangulos
                box.x = ov->pos.x + ov->buttonList[i].pos.x + ov->buttonList[i].quadList[j].pos.x;
                box.y = ov->pos.y + ov->buttonList[i].pos.y + ov->buttonList[i].quadList[j].pos.y;
                box.width = ov->buttonList[i].quadList[j].pos.width;
                box.height = ov->buttonList[i].quadList[j].pos.height;
                if (isInside (pos, box ))
                {
                    aux = 1;
                    if (button != NULL) button[0] = i;
                    break;
                }
            }
            if (aux == 1)
            {
                if (event == 1)
                {
                    if (ov->buttonList[i].onClick != NULL) ov->buttonList[i].onClick(ov->buttonList[i].Parameters, ov);
                    if (ov->onButtonPressed != NULL) ov->onButtonPressed(i, ov);
                }

                return 2;
            }
        }

        // se até aqui não saiu, então pos não está dentro de nenhum botão,
        //verifica se está dentro da area do overlay (ignora se não há um retangulo visivel no lugar)
        for (i = 0; i < ov->numQuads; i++)
        {
            box.x = ov->pos.x + ov->quadList[i].pos.x;
            box.y = ov->pos.y + ov->quadList[i].pos.y;
            box.width = ov->quadList[j].pos.width;
            box.height = ov->quadList[j].pos.height;
            if (isInside (pos, box)) return 1;
        }
    }
    return 0;
}

void OrderOverlayByZ (Overlay * ov, int size)
{
    int x = 1;
    int i, j;
    j = 0;
    while (x == 1)
    {
        x = 0;
        for (i = 0; i < size - 1 - j; i++)
            if (ov[i].z < ov[i + 1].z)
            {
                Overlay aux = ov[i];
                ov[i] = ov[i+1];
                ov[i+1] = aux;
                x = 1;
            }
        j++;

    }
}




int LoadFont(const char * file, const char * texFile, Font * font)
{
    FILE * pfile;
    char * buffer;
    char * line;
    int i;
    CharInfo ch[256];
    pfile = fopen (file, "r");
    if (pfile == NULL) return 0;

    line = malloc (sizeof (char) * 1024);
    for (i = 0; i < 256; i++)
    {
        ch[i].isSupported = 0;
        font->isSupported[i] = 0;
    }
    while (fgets(line, 1024, pfile) != NULL)
    {
        buffer = strtok (line, " ");
        if (!strcmp ("common", buffer))
        {
            int info[4];
            buffer = strtok (NULL, "");
            if (sscanf (buffer, "lineHeight=%d%*[ ]base=%d%*[ ]scaleW=%d%*[ ]scaleH=%d", &info[0], &info[1], &info[2], &info[3]) == 4)
            {
                font->lineHeight = info[0];
                font->base = info[1];
                font->scaleW = info[2];
                font->scaleH = info[3];
            }
            else return 0;

        }
        else if (!strcmp ("char", buffer))
        {
            int info[8];
            buffer = strtok (NULL, "");
            if (sscanf (buffer, "id=%d%*[ ]x=%d%*[ ]y=%d%*[ ]width=%d%*[ ]height=%d%*[ ]xoffset=%d%*[ ]yoffset=%d%*[ ]xadvance=%d%*[^\n]",
            &info[0], &info[1], &info[2], &info[3], &info[4], &info[5], &info[6], &info[7]) == 8)
            {
                ch[info[0]].isSupported = 1;
                ch[info[0]].pos.x = info[1];
                ch[info[0]].pos.y = info[2];
                ch[info[0]].pos.width = info[3];
                ch[info[0]].pos.height = info[4];
                ch[info[0]].xoffset =info[5];
                ch[info[0]].yoffset = info[6];
                ch[info[0]].xadvance = info[7];
            }
        }
    }
    font->fontTexture =  malloc (sizeof(Texture));
    font->fontTexture->Data = NULL;
    if (LoadTexture(texFile, font->fontTexture) == 0)
    {
        printf ("Unable to load texture\n");
        exit(EXIT_FAILURE);
    }
    BufferTexture (font->fontTexture);

    font->listBase = glGenLists (256);
    glBindTexture (GL_TEXTURE_2D, font->fontTexture->Buffer);
    for (i = 0; i < 256; i++)
    {
        glNewList (font->listBase + i, GL_COMPILE);
            if (ch[i].isSupported == 1)
            {
                glBegin(GL_QUADS);
                    glTexCoord2f (ch[i].pos.x / font->scaleW, ch[i].pos.y / font->scaleH);
                    glVertex2d (ch[i].xoffset, ch[i].yoffset);
                    glTexCoord2f ((ch[i].pos.x + ch[i].pos.width) / font->scaleW, ch[i].pos.y / font->scaleH);
                    glVertex2d (ch[i].xoffset + ch[i].pos.width, ch[i].yoffset);
                    glTexCoord2f ((ch[i].pos.x + ch[i].pos.width) / font->scaleW, (ch[i].pos.y + ch[i].pos.height) / font->scaleH);
                    glVertex2d (ch[i].xoffset + ch[i].pos.width, ch[i].yoffset + ch[i].pos.height);
                    glTexCoord2f (ch[i].pos.x / font->scaleW, (ch[i].pos.y + ch[i].pos.height) / font->scaleH);
                    glVertex2d (ch[i].xoffset, ch[i].yoffset + ch[i].pos.height);
                glEnd();
                glTranslatef (ch[i].xadvance - ch[i].xoffset, 0.0f, 0.0f);
            }
            else glTranslatef (font->base, 0, 0);
        glEndList();
    }
    free(line);
    return 1;
}

void DrawText (Text * text)
{
    int len = strlen(text->string);
    glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
    glListBase(text->font->listBase);
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix();
    glTranslatef (text->pos.x, text->pos.y, 0);
    glScalef(text->scale, text->scale, 1);
    glColor3f (text->R, text->G, text->B);
    if (text->font->fontTexture->Buffer == 0) BufferTexture (text->font->fontTexture);
    glBindTexture (GL_TEXTURE_2D, text->font->fontTexture->Buffer);
    glCallLists(len, GL_UNSIGNED_BYTE, text->string);
    glPopMatrix();
    glPopAttrib();
}

void DrawQuads (int num, Quad * quads)
{
    int i;
    Texture * boundTexture = NULL;
    int isColored = 0, isTextured = 0;
    int begin = 1;
    glBindTexture (GL_TEXTURE_2D, 0);
    glMatrixMode (GL_MODELVIEW);
    glPushAttrib (GL_CURRENT_BIT);
    for (i = 0; i<num; i++)
    {
        if (boundTexture != quads[i].tex || isColored != quads[i].isColored || isTextured != quads[i].isTextured)
        {
            if (begin == 0)
            {
                glEnd();
                begin = 1;
            }
            glPopAttrib();
            glPushAttrib(GL_CURRENT_BIT);
            if (quads[i].isTextured)
            {
                if (quads[i].tex->Buffer == 0) BufferTexture (quads[i].tex);
                glBindTexture (GL_TEXTURE_2D, quads[i].tex->Buffer);
                boundTexture = quads[i].tex;
            }
            else glBindTexture (GL_TEXTURE_2D, 0);

            isColored = quads[i].isColored;
            isTextured = quads[i].isTextured;
        }

        if (begin == 1)
        {
            glBegin(GL_QUADS);
            begin = 0;
        }
        if (quads[i].isTextured) glTexCoord2f (quads[i].texCoord.x, quads[i].texCoord.y);
        if (quads[i].isColored) glColor4f (quads[i].color[0], quads[i].color[1], quads[i].color[2], quads[i].color[3]);
        glVertex2f (quads[i].pos.x, quads[i].pos.y);
        if (quads[i].isTextured) glTexCoord2f (quads[i].texCoord.x + quads[i].texCoord.width, quads[i].texCoord.y);
        if (quads[i].isColored) glColor4f (quads[i].color[4], quads[i].color[5], quads[i].color[6], quads[i].color[7]);
        glVertex2f (quads[i].pos.x + quads[i].pos.width, quads[i].pos.y);
        if (quads[i].isTextured) glTexCoord2f (quads[i].texCoord.x + quads[i].texCoord.width, quads[i].texCoord.y + quads[i].texCoord.height);
        if (quads[i].isColored) glColor4f (quads[i].color[8], quads[i].color[9], quads[i].color[10], quads[i].color[11]);
        glVertex2f (quads[i].pos.x + quads[i].pos.width, quads[i].pos.y + quads[i].pos.height);
        if (quads[i].isTextured) glTexCoord2f (quads[i].texCoord.x, quads[i].texCoord.y + quads[i].texCoord.height);
        if (quads[i].isColored) glColor4f (quads[i].color[12], quads[i].color[13], quads[i].color[14], quads[i].color[15]);
        glVertex2f (quads[i].pos.x, quads[i].pos.y + quads[i].pos.height);

    }
    glEnd();
    glPopAttrib();

}


int isInside (Coord pos, Coord box)
{
    if (pos.x >= box.x && pos.x <= box.x + box.width && pos.y >= box.y && pos.y <= box.y + box.height) return 1;
    return 0;
}

Overlay * CreateOverlays (int num)
{
    int i;
    Overlay * ov = malloc (sizeof (Overlay)* num);
    for (i = 0; i < num; i++)
    {
        ov[i].name = NULL;
        ov[i].pos.x = 0.0f;
        ov[i].pos.y = 0.0f;
        ov[i].z = 0.0f;
        ov[i].hidden = 1;
        ov[i].buttonList = NULL;
        ov[i].numButtons = 0;
        ov[i].textList = NULL;
        ov[i].numTexts = 0;
        ov[i].quadList = NULL;
        ov[i].numQuads = 0;
        ov[i].onButtonPressed = NULL;
    }
    return ov;
}

Button * CreateButtons (int num)
{
    int i;
    int param[3] = {0, 0, 0};
    Button * ptr = malloc (sizeof (Button) * num);
    for (i = 0; i < num; i++)
    {
        ptr[i].pos.x = 0.0f;
        ptr[i].pos.y = 0.0f;
        ptr[i].pos.width = 0.0f;
        ptr[i].pos.height = 0.0f;
        ptr[i].hidden = 1;
        memcpy(ptr[i].Parameters, param, 3);
        ptr[i].onClick = NULL;
        ptr[i].textList = NULL;
        ptr[i].numTexts = 0;
        ptr[i].quadList = NULL;
        ptr[i].numQuads = 0;
    }
    return ptr;
}

Text * CreateTexts (int num)
{
    int i;
    Text * ptr = malloc (sizeof(Text) * num);
    for (i = 0; i < num ; i++)
    {
        ptr[i].name = NULL;
        ptr[i].string = NULL;
        ptr[i].pos.x = 0.0f;
        ptr[i].pos.y = 0.0f;
        ptr[i].pos.width = 0.0f;
        ptr[i].pos.height = 0.0f;
        ptr[i].scale = 1.0f;
        ptr[i].R = 0.0f;
        ptr[i].G = 0.0f;
        ptr[i].B = 0.0f;
        ptr[i].font = NULL;
        ptr[i].hidden = 1;
    }
    return ptr;

}

Quad * CreateQuads (int num)
{
    int i;
    float color[16] = {0.0f, 0.0f, 0.0f, 1.0f,0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    Quad * ptr = malloc (sizeof (Quad) * num);
    for (i=0; i<num; i++)
    {
        ptr[i].pos.x = 0.0f;
        ptr[i].pos.y = 0.0f;
        ptr[i].pos.width = 0.0f;
        ptr[i].pos.height = 0.0f;
        ptr[i].texCoord.x = 0.0f;
        ptr[i].texCoord.y = 0.0f;
        ptr[i].texCoord.width = 0.0f;
        ptr[i].texCoord.height = 0.0f;
        ptr[i].tex = NULL;
        memcpy (ptr[i].color, color, 16);
        ptr[i].isColored = 0;
        ptr[i].isTextured = 0;
    }
    return ptr;
}

void DrawGameObject (GameObject * ptr, int num )
{
    int i;
    for (i = 0; i < num ; i++)
    {
        glPushMatrix();
        glTranslatef (ptr[i].pos.x, ptr[i].pos.y, 0.0f);
        if (ptr[i].numQuads > 0) DrawQuads(ptr[i].numQuads, ptr[i].quadList);
        glPopMatrix();
    }
}

void DrawObjectList (ObjectList * ptr)
{
    while (ptr != NULL)
    {
        DrawGameObject (ptr->obj, 1);
        ptr = ptr->next;
    }
}

void DrawMap (Map * map)
{
    glPushMatrix();
    glTranslatef (0.0f, 0.0f, -1.0f);
    if (map->back != NULL)
    {
        if (map->back->numQuads > 0) DrawQuads (map->back->numQuads, map->back->quadList);
    }
    glTranslatef(0.0f, 0.0f, 0.01f);
    DrawObjectList(map->objectListFirst);
    if (map->gun != NULL)
    {
        glTranslatef(0.0f, 0.0f, 0.01f);
        DrawGameObject(map->gun, 1);
    }
    glPopMatrix();
}


