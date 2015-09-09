/*
Header dos sistemas relativos ao OpenGL
Alguns comandos do opengl podem ser chamados a partir de fun��es de render,
mas a maioria dos m�todos est�o abstraidos neste arquivo
*/
#ifndef _OPENGL_INCLUDE_H
#define _OPENGL_INCLUDE_H

#include <stdlib.h>
#include "GameStructs.h"




// Inicializa todos os sistemas necess�rios (glfw, glew, matriz de proje��o, glClearColor, etc)
void InitAll();

// Le um arquivo .TGA para armazenar como uma textura
// retorna 1 se a textura foi lida com sucesso
int LoadTexture (const char * file, Texture * target);

// Cria um buffer na placa de video para a textura
void BufferTexture (Texture * tex);
// Destroi o buffer da texture
void UnbufferTexture (Texture * tex);

//Liberta a memoria armazenada para a textura
void FreeTexture (Texture * tex);

// Desenha um overlay e todos os seus bot�es visiveis na tela
void DrawOverlay (Overlay * ov);

// verifica se a posi��o est� dentro do overlay
//se event == 1, dispara os eventos onClick do overlay
int ClickOverlay (Coord pos, Overlay * ov, int event, int * button);

// Ordena um vetor de overlay pelo Z para facilitar a prioriza��o deles
//implementado bubble sort
void OrderOverlayByZ (Overlay * ov, int size);

// le as informa��es sobre uma fonte de um arquivo .fnt (criado pelo BMFont)
// e carrega a textura da fonte
int LoadFont(const char * file, const char * texFile, Font * font);

// desenha o texto na tela
void DrawText (Text * text);

// Desenha num quadrados
// A fun��o est� otimizada para desenhar o maximo possivel de quadrados na sequencia que eles est�o no vetor
// sem sair do modo imediato
// Possivel otimiza��o: marcar quadrados que precisam de uma mudan�a de estado para renderizar em uma outras passagens
void DrawQuads (int num, Quad * quads);

// fun��o auxiliar param simplificar algumas outras
// verifica se box.x <= pos.x <= box.x + box.width e box.y <= pos.y <= box.y + box.width
int isInside (Coord pos, Coord box);


// fun��es auxiliares para criar structs e inicializar seus valores automaticamente
Overlay * CreateOverlays (int num);

Button * CreateButtons (int num);

Text * CreateTexts (int num);

Quad * CreateQuads (int num);

// func�o que desenha um game object
void DrawGameObject (GameObject * ptr, int num );

// fun��o que desenha um mapa (background, gameobjects e overlays)
void DrawMap(Map * map);

#endif
