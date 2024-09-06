#include <windows.h>
#include <gl/gl.h>
#include "SOIL.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define DISTANCIA 0.0875
#define INFINITY 207000
#define MAXCHAR 257000
#define MAX 239
#define TAMANHO_BOLINHA 0.004

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
char diretorio[100], cenario[100], pacman[100], cereja[100], fantasma[100], tela[100];
float xpacman, ypacman, x[231], y[231], movx=0, movy=0, xcereja[4], ycereja[4], xfantasma[4], yfantasma[4], xbolinha[231], ybolinha[231];
int animacao=0, direcao=1, atualizar=0, timer=0, destino[4], G[MAX][MAX], caminho[4][231], distancia[4], f_atual, posicao[4], status_fantasma[4];
int desacelerar=0, posicao_pacman, posicao_fantasma[4], segir=0, fugir=0, morto[4], pego[231], bolinhas_pegas=0, jogo=0, inicio=0;
HWND hwnd;


void Testar_Bolinhas();
void Imprimir_Bolinhas();
void Testar_Colisao();
void Mover_fantasma();
void Ativar_matriz();
void dijkstra(int startnode, int ultimo, int f_atual);
void Gerar_destino();
void Imprimir_Fantasmas();
void Testar_Cereja();
void Atualizar_posicao();
void Definir_Coordenadas();
void Pegar_Diretorio();
void Teclado();
void Desenhar();
void Imprimir_imagem(char nome[], float x, float y, float tamanho);


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    WPARAM hFont;
    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(99, 18, 1));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;
    int bolinha_atual;
    for(bolinha_atual=0;bolinha_atual<231;bolinha_atual++)
    {
        pego[bolinha_atual]=0;
        xbolinha[bolinha_atual]=2;
        ybolinha[bolinha_atual]=2;
    }
    int fantasma_atual;
    for(fantasma_atual=0;fantasma_atual<4;fantasma_atual++)
    {
        status_fantasma[fantasma_atual]=0;
        morto[fantasma_atual]=0;
    }
    Ativar_matriz();
    srand(time(NULL));
    Definir_Coordenadas();
    movx=x[184];
    movy=y[184];
    xcereja[0]=x[23];ycereja[0]=y[23];
    xcereja[1]=x[84];ycereja[1]=y[84];
    xcereja[2]=x[160];ycereja[2]=y[160];
    xcereja[3]=x[140];ycereja[3]=y[140];
    xfantasma[0]=x[1];yfantasma[0]=y[1];
    xfantasma[1]=x[0];yfantasma[1]=y[0];
    xfantasma[2]=x[2];yfantasma[2]=y[2];
    xfantasma[3]=x[0];yfantasma[3]=y[0];
    Pegar_Diretorio();
    sprintf(cenario,"%sPacMan Cenario.png",diretorio);
    sprintf(pacman,"%sPacMan.png",diretorio);
    sprintf(cereja,"%sCereja.png",diretorio);
    sprintf(tela,"%sINICIO.png",diretorio);


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          300,
                          50,
                          800,
                          800,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);
    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glPushMatrix();
            if(inicio==1)
            {
                if(jogo==0)
                {
                    Teclado();
                    if(atualizar==0)
                    {
                        Atualizar_posicao();
                    }
                    Testar_Cereja();
                    Desenhar();
                    Imprimir_Fantasmas();
                    if(desacelerar%5!=0)
                    {
                        Gerar_destino();
                        Mover_fantasma();
                    }
                    desacelerar++;
                    if(timer!=0)
                    {
                        //printf("%d - %d\n",timer,fugir);
                        timer--;
                        if(timer==0)
                        {
                            fugir=0;
                        }
                    }

                    atualizar=0;
                    Testar_Bolinhas();
                    if(bolinhas_pegas==223)
                    {
                        jogo=1;
                        sprintf(tela,"%sGAMEWIN.png",diretorio);
                    }
                    Testar_Colisao();

                }
                else
                if(jogo==1)
                {
                    Imprimir_imagem(tela,0,0,1);
                }

            }
            else
            if(inicio==0)
            {
                Imprimir_imagem(tela,0,0,1);
                if(GetAsyncKeyState(VK_RETURN))
                {
                    inicio=1;
                }
            }
            glPopMatrix();
            SwapBuffers(hDC);
            glFlush();
            Sleep (280);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_CTLCOLORSTATIC:
                SetTextColor((HDC)wParam,RGB(0,255,0));
                SetBkMode((HDC)wParam,TRANSPARENT);
        return GetClassLong(hwnd, GCL_HBRBACKGROUND);

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
void Desenhar()
{
    int cereja_atual;
    xpacman=movx;
    ypacman=movy;
    Imprimir_imagem(cenario,0,0,1);
    Imprimir_Bolinhas();
    if(animacao==0)
    {
        sprintf(pacman,"%sPacMan%d.png",diretorio,direcao);
    }
    else
    if(animacao==1)
    {
        sprintf(pacman,"%sPacMan_Fechado%d.png",diretorio,direcao);
    }
    Imprimir_imagem(pacman,xpacman,ypacman,0.05);
    animacao=1-animacao;
    for(cereja_atual=0;cereja_atual<4;cereja_atual++)
    {
        Imprimir_imagem(cereja,xcereja[cereja_atual],ycereja[cereja_atual],0.06);
    }
}

void Imprimir_imagem(char nome[], float x, float y, float tamanho)
{
    GLuint tex_2d = SOIL_load_OGL_texture
            (
            nome,
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB
            );
            if( 0 == tex_2d )
            {
                printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
                printf("%s\n",nome);
            }
            glColor3f(1.0, 1.0, 1.0);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tex_2d);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                    glBegin(GL_QUADS);
                        glTexCoord2f(1.0f,1.0f); glVertex2f(x+tamanho, y+tamanho);
                        glTexCoord2f(1.0f,0.0f); glVertex2f(x+tamanho, y-tamanho);
                        glTexCoord2f(0.0f,0.0f); glVertex2f(x-tamanho, y-tamanho);
                        glTexCoord2f(0.0f,1.0f); glVertex2f(x-tamanho, y+tamanho);
                    glEnd();

            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);

}


void Teclado()
{
    float xbunker,ybunker,movimentacaox,movimentacaoy;
    int posicao=0, direcaobunker;
    xbunker=movx;
    ybunker=movy;
    direcaobunker=direcao;
    movimentacaox=0;
    movimentacaoy=0;
    if(atualizar==0)
    {
        if(GetAsyncKeyState(VK_UP))
        {
            if(direcao!=0)
            {
                //sprintf(cabeca,"%sCobra_Cabeça_Baixo.png",diretorio);
                direcao=0;
                movy+=DISTANCIA;
                atualizar=1;
                movimentacaoy=DISTANCIA;
                //return;
            }
        }
        else
        if(GetAsyncKeyState(VK_RIGHT))
        {
            if(direcao!=1)
            {
             //   sprintf(cabeca,"%sCobra_Cabeça_Direita.png",diretorio);
                direcao=1;
                movx+=DISTANCIA;
                atualizar=1;
                if(((movx>x[114]+DISTANCIA/2)&&(movx<x[114]+DISTANCIA+DISTANCIA/2))&&((movy<y[114]+DISTANCIA/2)&&(movy>y[114]-DISTANCIA/2)))
                {
                    movx=x[119];
                }
                movimentacaox=DISTANCIA;
                //return;
            }
        }
        else
        if(GetAsyncKeyState(VK_LEFT))
        {
            if(direcao!=2)
            {
               // sprintf(cabeca,"%sCobra_Cabeça_Esquerda.png",diretorio);
                direcao=2;
                movx-=DISTANCIA;
                atualizar=1;
                if(((movx<x[119]-DISTANCIA/2)&&(movx>x[119]-DISTANCIA-DISTANCIA/2))&&((movy<y[119]+DISTANCIA/2)&&(movy>y[119]-DISTANCIA/2)))
                {
                    movx=x[114];
                }
                movimentacaox=-DISTANCIA;
                //return;
            }
        }
        else
        if(GetAsyncKeyState(VK_DOWN))
        {
            if(direcao!=3)
            {
               // sprintf(cabeca,"%sCobra_Cabeça_Cima.png",diretorio);
                direcao=3;
                movy-=DISTANCIA;
                atualizar=1;
                movimentacaoy=-DISTANCIA;
                //return;
            }
        }

        while(posicao!=231)
        {
            if(((movx<x[posicao]+DISTANCIA/2)&&(movx>x[posicao]-DISTANCIA/2))&&((movy<y[posicao]+DISTANCIA/2)&&(movy>y[posicao]-DISTANCIA/2)))
            {
                if(posicao!=3)
                {

                    break;
                }

            }
            posicao++;
        }
        if(posicao==231)
        {
            movx=xbunker;
            movy=ybunker;
        }
        posicao=0;
        if(direcao!=direcaobunker)
        {
        while(posicao!=231)
        {
            if(((movx+movimentacaox<x[posicao]+DISTANCIA/2)&&(movx+movimentacaox>x[posicao]-DISTANCIA/2))&&((movy+movimentacaoy<y[posicao]+DISTANCIA/2)&&(movy+movimentacaoy>y[posicao]-DISTANCIA/2)))
            {
                if(posicao!=3)
                {

                    return;
                }

            }
            posicao++;
        }
        direcao=direcaobunker;
        }

    }

}


void Pegar_Diretorio()
{
    FILE *fp;
    char* filename = ".//Pacman//diretorio.txt";

    fp = fopen(filename, "r");
    if (fp == NULL){
         fp = fopen("diretorio.txt", "r");
        if (fp == NULL){
            printf("Could not open file %s",filename);
            return;
        }
    }
    while (fgets(diretorio, MAXCHAR, fp) != NULL)
    fclose(fp);
}

void Atualizar_posicao()
{
    float xbunker,ybunker;
    int posicao=0, fantasma_atual;
    xbunker=movx;
    ybunker=movy;
    switch(direcao)
    {
        case 0:
            movy+=DISTANCIA;
        break;
        case 1:
            movx+=DISTANCIA;
            if(((movx>x[114]+DISTANCIA/2)&&(movx<x[114]+DISTANCIA+DISTANCIA/2))&&((movy<y[114]+DISTANCIA/2)&&(movy>y[114]-DISTANCIA/2)))
            {
                movx=x[119];
            }
        break;
        case 2:
            movx-=DISTANCIA;
            if(((movx<x[119]-DISTANCIA/2)&&(movx>x[119]-DISTANCIA-DISTANCIA/2))&&((movy<y[119]+DISTANCIA/2)&&(movy>y[119]-DISTANCIA/2)))
            {
                movx=x[114];
            }
        break;
        case 3:
            movy-=DISTANCIA;
        break;
    }
    while(posicao!=231)
    {
        if(((movx<x[posicao]+DISTANCIA/2)&&(movx>x[posicao]-DISTANCIA/2))&&((movy<y[posicao]+DISTANCIA/2)&&(movy>y[posicao]-DISTANCIA/2)))
        {
            if(posicao!=3)
            {
                posicao_pacman=posicao;
                for(fantasma_atual=0;fantasma_atual<4;fantasma_atual++)
                {
                    if(posicao_pacman==posicao_fantasma[fantasma_atual]&&fugir==0)
                    {
                        jogo=1;
                        sprintf(tela,"%sGAMEOVER.png",diretorio);
                    }
                }
                return;

            }
        }
        posicao++;
    }

    movx=xbunker;
    movy=ybunker;

}

void Testar_Cereja()
{
    int cereja_atual;
    for(cereja_atual=0;cereja_atual<4;cereja_atual++)
    {
        if(((xpacman<xcereja[cereja_atual]+DISTANCIA/2)&&(xpacman>xcereja[cereja_atual]-DISTANCIA/2))&&((ypacman<ycereja[cereja_atual]+DISTANCIA/2)&&(ypacman>ycereja[cereja_atual]-DISTANCIA/2)))
        {
            xcereja[cereja_atual]=2;
            ycereja[cereja_atual]=2;
            timer=30;
            fugir=1;
        }
    }
}

void Imprimir_Fantasmas()
{
    int fantasma_atual;
    for(fantasma_atual=0;fantasma_atual<4;fantasma_atual++)
    {
        if(morto[fantasma_atual]==0)
        {
            if(fugir==0)
            {
                sprintf(fantasma,"%sFantasma%d.png",diretorio,fantasma_atual);
            }
            else
                sprintf(fantasma,"%sFantasma_Assustado.png",diretorio);
            Imprimir_imagem(fantasma,xfantasma[fantasma_atual],yfantasma[fantasma_atual],0.07);

        }
        else
        if(morto[fantasma_atual]!=0)
        {
            morto[fantasma_atual]--;
            xfantasma[fantasma_atual]=0;
            yfantasma[fantasma_atual]=0;
            status_fantasma[fantasma_atual]=0;

        }
    }
}

void Gerar_destino()
{
    int fantasma_atual, posicao_atual;

    for(fantasma_atual=0;fantasma_atual<4;fantasma_atual++)
    {
        for(posicao_atual=0;posicao_atual<231;posicao_atual++)
        {
            if(((xfantasma[fantasma_atual]<x[posicao_atual]+DISTANCIA/2)&&(xfantasma[fantasma_atual]>x[posicao_atual]-DISTANCIA/2))&&((yfantasma[fantasma_atual]<y[posicao_atual]+DISTANCIA/2)&&(yfantasma[fantasma_atual]>y[posicao_atual]-DISTANCIA/2)))
            {
                posicao_fantasma[fantasma_atual]=posicao_atual;
                break;
            }
        }
        if(fugir==0)
        {
            if(status_fantasma[fantasma_atual]==0)
            {
                dijkstra(posicao_atual,posicao_pacman,fantasma_atual);
                if(distancia[fantasma_atual]>=6)
                {
                    destino[fantasma_atual]= rand()%227+4;
                }
                else
                if(distancia[fantasma_atual]<6)
                {
                    destino[fantasma_atual]=posicao_pacman;
                    segir=1;
                }
            }
        }
        else
        if(fugir==1)
        {
          destino[fantasma_atual]=0;
        }
        dijkstra(posicao_atual,destino[fantasma_atual],fantasma_atual);
        status_fantasma[fantasma_atual]=1;
        //printf("%d - %d\tPacman-%d\n",fantasma_atual,destino[fantasma_atual],posicao_pacman);


    }

}

void dijkstra(int startnode, int ultimo, int f_atual)
{
    int n=231;
	int cost[MAX][MAX],distance[MAX],pred[MAX];
	int visited[MAX],count,mindistance,nextnode,i,j;
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			if(G[i][j]==0)
				cost[i][j]=INFINITY;
			else
				cost[i][j]=G[i][j];
	for(i=0;i<n;i++)
	{
		distance[i]=cost[startnode][i];
		pred[i]=startnode;
		visited[i]=0;
	}

	distance[startnode]=0;
	visited[startnode]=1;
	count=1;

	while(count<n-1)
	{
		mindistance=INFINITY;
		for(i=0;i<n;i++)
			if(distance[i]<mindistance&&!visited[i])
			{
				mindistance=distance[i];
				nextnode=i;
			}
			visited[nextnode]=1;
			for(i=0;i<n;i++)
				if(!visited[i])
					if(mindistance+cost[nextnode][i]<distance[i])
					{
						distance[i]=mindistance+cost[nextnode][i];
						pred[i]=nextnode;

					}
		count++;
	}
	posicao[f_atual]=0;
		if(i!=startnode)
		{
			//printf("\nDistancia ate o vertice %d =%d",ultimo,distance[ultimo]);
			//printf("\nCaminho=%d",ultimo);

			j=ultimo;
			do
			{

				caminho[f_atual][posicao[f_atual]]=j;
				j=pred[j];
				//printf("<-%d",j);
				posicao[f_atual]++;

			}while(j!=startnode);
            posicao[f_atual]--;
            distancia[f_atual]=distance[ultimo];
			//do
           // {

                //printf("\n%d",caminho[f_atual][atual]);
                //posicao[f_atual]--;
            //}while(posicao[f_atual]>=0);
            //caminho
            //distance[ultimo]
	}
}

void Ativar_matriz()
{
    FILE *fp;
    char str[MAXCHAR],str2[MAXCHAR];
    char* filename=".//Pacman//Matriz Grafo Pacman2.txt";
	int i,j,posicao_atual,posicao_atual2=0,posicao_atual3=0,linha,coluna;
    fp = fopen(filename, "r");
    if (fp == NULL){
            fp = fopen("Matriz Grafo Pacman2.txt","r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return;
    }
    }
    while (fgets(str, MAXCHAR, fp) != NULL)
    fclose(fp);
    for(posicao_atual=0;str[posicao_atual]!='\0';posicao_atual++)
    {
        if((str[posicao_atual]=='0')||(str[posicao_atual]=='1'))
        {
            str2[posicao_atual2]=str[posicao_atual];
            posicao_atual2++;
        }
    }
    for(linha=0;linha<=230;linha++)
    {
        for(coluna=0;coluna<=230;coluna++)
        {

            G[linha][coluna]=str2[posicao_atual3]-'0';
            posicao_atual3++;
        }
    }
}

void Mover_fantasma()
{
    int fantasma_atual;

    for(fantasma_atual=0;fantasma_atual<4;fantasma_atual++)
    {
        xfantasma[fantasma_atual]=x[caminho[fantasma_atual][posicao[fantasma_atual]]];
        yfantasma[fantasma_atual]=y[caminho[fantasma_atual][posicao[fantasma_atual]]];
        posicao[fantasma_atual]--;

        dijkstra(posicao_fantasma[fantasma_atual],destino[fantasma_atual],fantasma_atual);
        if(segir==1)
        {
            if(distancia[fantasma_atual]>=6)
            {
                status_fantasma[fantasma_atual]=0;
                segir=0;
            }
        }
        if((caminho[fantasma_atual][posicao[fantasma_atual]]==destino[fantasma_atual])||(distancia[fantasma_atual]<6))
        {
            status_fantasma[fantasma_atual]=0;
        }
    }
}

void Testar_Colisao()
{
    int fantasma_atual;
    for(fantasma_atual=0;fantasma_atual<4;fantasma_atual++)
    {
        if(posicao_pacman==posicao_fantasma[fantasma_atual])
        {
            if(fugir==0)
            {
                jogo=1;
                sprintf(tela,"%sGAMEOVER.png",diretorio);
            }
            else
            if(fugir==1)
            {
                morto[fantasma_atual]=45;
            }
        }
    }
}

void Imprimir_Bolinhas()
{
    int posicao_atual;
    for(posicao_atual=4;posicao_atual<231;posicao_atual++)
    {
        if((posicao_atual!=23)&&(posicao_atual!=84)&&(posicao_atual!=160)&&(posicao_atual!=140))
        {
            if(pego[posicao_atual]==0)
            {
                xbolinha[posicao_atual]=x[posicao_atual];
                ybolinha[posicao_atual]=y[posicao_atual];
                //Imprimir_imagem(bolinha,xbolinha[posicao_atual2],ybolinha[posicao_atual2],0.03);
                glColor3f(1.0, 1.0, 1.0);
                glBegin(GL_QUADS);
                        glVertex2f(xbolinha[posicao_atual]+TAMANHO_BOLINHA, ybolinha[posicao_atual]+TAMANHO_BOLINHA);
                        glVertex2f(xbolinha[posicao_atual]+TAMANHO_BOLINHA, ybolinha[posicao_atual]-TAMANHO_BOLINHA);
                        glVertex2f(xbolinha[posicao_atual]-TAMANHO_BOLINHA, ybolinha[posicao_atual]-TAMANHO_BOLINHA);
                        glVertex2f(xbolinha[posicao_atual]-TAMANHO_BOLINHA, ybolinha[posicao_atual]+TAMANHO_BOLINHA);
                glEnd();

                //posicao_atual++;

            }
        }
    }
}

void Testar_Bolinhas()
{
    int bolinha_atual;
    for(bolinha_atual=4;bolinha_atual<231;bolinha_atual++)
    {
        if(xbolinha[bolinha_atual]!=2)
        {
            if(pego[bolinha_atual]==0)
            {
                if(((xbolinha[bolinha_atual]<xpacman+DISTANCIA/2)&&(xbolinha[bolinha_atual]>xpacman-DISTANCIA/2))&&((ybolinha[bolinha_atual]<ypacman+DISTANCIA/2)&&(ybolinha[bolinha_atual]>ypacman-DISTANCIA/2)))
                {
                    //printf("%d  %d\n",xbolinha[bolinha_atual],ybolinha[bolinha_atual]);
                    pego[bolinha_atual]=1;
                    bolinhas_pegas++;
                    //printf("%d\n",bolinhas_pegas);
                }

            }

        }
    }
}

void Definir_Coordenadas()
{
    int vertice;
    for(vertice=0;vertice<231;vertice++)
    {
        switch(vertice)
        {
            case 0:
                x[vertice]=0;y[vertice]=0;
            break;
            case 1:
                x[vertice]=x[0]-DISTANCIA;y[vertice]=y[0];
            break;
            case 2:
                x[vertice]=x[0]+DISTANCIA;y[vertice]=y[0];
            break;
            case 3:
                x[vertice]=x[0];y[vertice]=y[0]+DISTANCIA;
            break;
            case 4:
                x[vertice]=x[3];y[vertice]=y[3]+DISTANCIA;
            break;
            case 5:
                x[vertice]=x[4]+DISTANCIA;y[vertice]=y[4];
            break;
            case 6:
                x[vertice]=x[4]-DISTANCIA;y[vertice]=y[4];
            break;
            case 7:
                x[vertice]=x[6];y[vertice]=y[6]+DISTANCIA;
            break;
            case 8:
                x[vertice]=x[7];y[vertice]=y[7]+DISTANCIA;
            break;
            case 9:
                x[vertice]=x[8]-DISTANCIA;y[vertice]=y[8];
            break;
            case 10:
                x[vertice]=x[9]-DISTANCIA;y[vertice]=y[9];
            break;
            case 11 ... 12:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 13 ... 14:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 15 ... 18:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 19 ... 23:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 24 ... 27:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 28 ... 31:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 32:
                x[vertice]=x[14];y[vertice]=y[14]-DISTANCIA;
            break;
            case 33:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 34 ... 38:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 39:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 40:
                x[vertice]=x[33];y[vertice]=y[33]-DISTANCIA;
            break;
            case 41 ... 43:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 44 ... 45:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 46 ... 47:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 48:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 49:
                x[vertice]=x[5];y[vertice]=y[5]+DISTANCIA;
            break;
            case 50:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 51 ... 52:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 53 ... 54:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 55 ... 56:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 57 ... 60:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 61 ... 64:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 65 ... 68:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 69 ... 70:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 71 ... 74:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 75 ... 77:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 78:
                x[vertice]=x[70]-DISTANCIA;y[vertice]=y[70];
            break;
            case 79:
                x[vertice]=x[54]-DISTANCIA;y[vertice]=y[54];
            break;
            case 80:
                x[vertice]=x[60]+DISTANCIA;y[vertice]=y[60];
            break;
            case 81 ... 84:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 85 ... 88:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 89 ... 92:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 93:
                x[vertice]=x[88];y[vertice]=y[88]-DISTANCIA;
            break;
            case 94:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 95 ... 99:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 100:
                x[vertice]=x[56];y[vertice]=y[56]-DISTANCIA;
            break;
            case 101:
                x[vertice]=x[99];y[vertice]=y[99]-DISTANCIA;
            break;
            case 102 ... 104:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 105 ... 106:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 107 ... 108:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 109:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 110:
                x[vertice]=x[104]+DISTANCIA;y[vertice]=y[104];
            break;
            case 111 ... 114:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 115:
                x[vertice]=x[43]-DISTANCIA;y[vertice]=y[43];
            break;
            case 116 ... 119:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 120:
                x[vertice]=x[104];y[vertice]=y[104]-DISTANCIA;
            break;
            case 121 ... 123:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 124 ... 128:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 129 ... 130:
                 x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 131 ... 133:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 134 ... 135:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 136 ... 138:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 139 ... 140:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 141 ... 160:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 161 ... 162:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 163 ... 165:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 166 ... 167:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 168 ... 170:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 171 ... 172:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 173 ... 179:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 180 ... 181:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 182 ... 187:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 188 ... 189:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 190 ... 191:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 192 ... 193:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 194 ... 197:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 198 ... 199:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 200 ... 201:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 202:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 203:
                x[vertice]=x[149];y[vertice]=y[149]+DISTANCIA;
            break;
            case 204:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 205 ... 206:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 207 ... 208:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 209 ... 210:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 211 ... 212:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 213:
                x[vertice]=x[vertice-1]+DISTANCIA;y[vertice]=y[vertice-1];//Direita
            break;
            case 214:
                x[vertice]=x[193]+DISTANCIA;y[vertice]=y[193];
            break;
            case 215:
                x[vertice]=x[210];y[vertice]=y[210]+DISTANCIA;
            break;
            case 216:
                x[vertice]=x[189]+DISTANCIA;y[vertice]=y[189];
            break;
            case 217:
                x[vertice]=x[195];y[vertice]=y[195]+DISTANCIA;
            break;
            case 218:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 219:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 220:
                x[vertice]=x[177];y[vertice]=y[177]-DISTANCIA;
            break;
            case 221 ... 223:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]-DISTANCIA;//Descer
            break;
            case 224:
                x[vertice]=x[vertice-1]-DISTANCIA;y[vertice]=y[vertice-1];//Esquerda
            break;
            case 225:
                 x[vertice]=x[197]-DISTANCIA;y[vertice]=y[197];
            break;
            case 226:
                x[vertice]=x[177];y[vertice]=y[177]+DISTANCIA;
            break;
            case 227 ... 228:
                x[vertice]=x[vertice-1];y[vertice]=y[vertice-1]+DISTANCIA;//Subir
            break;
            case 229:
                x[vertice]=x[181];y[vertice]=y[181]+DISTANCIA;
            break;
            case 230:
                x[vertice]=x[187];y[vertice]=y[187]+DISTANCIA;
            break;
        }


    }

}

//
