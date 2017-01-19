#include<stdio.h>
#include<stdlib.h>
#include <SDL2/SDL.h>
#include<SDL2/SDL_image.h>
int screenwidth = 1366;
int screenheight = 768;
Uint32* pixels;
int leftMouseButtonDown = 0;
int quit = 0;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* background;
SDL_Texture* Canvas;
SDL_Texture* colors[10];
SDL_Texture* tools[10];
int color_state[10] ;
int tool_state[10] ;
SDL_Texture *selector ;
SDL_Texture* WhiteColor;
SDL_Texture* isSelected;
SDL_PixelFormat  screen_format ;
int xold , yold , xnew , ynew ;
void handleColors(SDL_Event *event)
{
        int i  , x = 140 , y = 675 ;
        for ( i = 0 ; i <3 ; i++)
        {
                if(event->motion.x > x + i*100 && event->motion.x <  x + (i+1)*75 + i*25 && event->motion.y > 675 && event->motion.y < 750)
                        color_state[i] = 1 ;
                else
                        color_state[i] = 0 ;
        }
        int j  ,  x1 = 240 ;
        for ( j = 3 ; j < 10 ; j++)
        {
                if(event->motion.x > x1 + j*100 && event->motion.x < x1 + (j+1)*75 + j*25 && event->motion.y > 675 && event->motion.y < 750)
                        color_state[j] = 1 ;
                else if( event->motion.y > 668)
                        color_state[j] = 0 ;
        }
}
void handleTools(SDL_Event *event)
{
        int i ;
        for ( i = 0 ; i <7 ; i++)
        {
                if(event->motion.x > 10 && event->motion.x < 95 && event->motion.y > 30+i*100 && event->motion.y < 30 + (i+1) * 85 + i*15)
                        tool_state[i]  = 1 ;
                else if ( event->motion.x < 110)
                        tool_state[i] = 0 ;
        }
}
void drawLine(  int x1 , int y1 , int x2 , int y2) ;
void handleEvent()
{
        SDL_Event event;
        SDL_PollEvent(&event);

        switch (event.type)
        {
                case SDL_QUIT:
                quit = 1;
                break;
                case SDL_MOUSEBUTTONUP:
                        if (event.button.button == SDL_BUTTON_LEFT)
                                leftMouseButtonDown = 0;
                        break;
                case SDL_MOUSEBUTTONDOWN:
               {
                        if (event.button.button == SDL_BUTTON_LEFT)
                                leftMouseButtonDown = 1;
                        if(event.button.button == SDL_BUTTON_LEFT)
                        {
                                handleTools(&event) ;
                                handleColors(&event) ;
                        }
                }
                case SDL_MOUSEMOTION :
                {
                        xnew = event.motion.x-110 ;
                        ynew = event.motion.y ;
                        if (leftMouseButtonDown)
                        {
                                int mouseX = event.motion.x-110;
                                int mouseY = event.motion.y;
                                if(mouseX > 0 && mouseY < 668)
                                        drawLine(xold , yold , xnew ,ynew) ;

                        }
                        xold = xnew ;
                        yold = ynew ;
                }
                        break;
        }
}
void putPixel( int x , int y, int r , int g , int b)
{
        Uint32 c ;
        Uint8 *p = (Uint8 *)(&c) ;

        p[0] = b;
        p[1] = g ;
        p[2] = r ;
         p[3] = 255 ;
        pixels[y*(screenwidth -110) + x] = c ;
}
void swap(int *a ,int *b)
{
        int temp = *a ;
        *a = *b ;
        *b =temp ;
}
void drawLine(  int x1 , int y1 , int x2 , int y2)
{
        int dx = x2 - x1 ;
        int dy  = y2 - y1 ;
        int adx =abs(dx) ;
        int ady = abs(dy) ;
        if ( adx >= ady)
        {
                if(x1 > x2)
                {
                        swap(&y1 , &y2) ;
                        swap(&x1 , &x2) ;
                }
                float m = (float)dy/(float)dx ;
                float j = y1 ;
                int i ;

                for ( i = x1 ; i < x2 ; i++)
                {
                        putPixel(i , j  , 255 , 0 , 0 ) ;
                        j = j + m ;
                }
        }
        else if (ady > adx)
        {
                if(y1 > y2)
                {
                        swap(&y1 , &y2) ;
                        swap(&x1 , &x2) ;
                }
                float m = (float)dx/(float)dy ;
                float j = x1 ;
                int i ;
                for ( i = y1 ; i < y2 ; i++)
                {
                        putPixel(j , i  , 255 , 0 , 0 ) ;
                        j = j + m ;
                }
        }
}
int init()
{
        SDL_Init(SDL_INIT_VIDEO);
        int canvasWidth = screenwidth-110;
        int canvasHeight = screenheight-100;
        SDL_Window * window = SDL_CreateWindow("SDL2 Pixel Drawing",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenwidth, screenheight, SDL_WINDOW_FULLSCREEN);
        if(window == NULL)
                return 0;

        renderer = SDL_CreateRenderer(window, -1, 0);
        if(renderer == NULL)
                return 0;
        Canvas = SDL_CreateTexture(renderer,
                SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, canvasWidth ,canvasHeight);
        pixels = (Uint32*)malloc( sizeof(Uint32)*canvasWidth * canvasHeight);
        memset(pixels, 255, canvasWidth*canvasHeight * sizeof(Uint32));
        int i ;
        for ( i = 0 ;i <10 ; i++)
        {
                color_state[i] = 0;
                tool_state[i] = 0 ;
        }
        SDL_Surface *s = IMG_Load("./res/selector.png") ;
        selector = SDL_CreateTextureFromSurface(renderer , s) ;
        SDL_FreeSurface(s) ;
        xold = xnew = canvasWidth/2;
        yold = ynew = canvasHeight/2;
        SDL_WarpMouseInWindow(window , xold , yold) ;
        drawLine(100 , 100 ,0 , 400) ;
        return 1;

}
void renderColors()
{
        SDL_Rect s_color , d_color ;
        SDL_QueryTexture(colors[7] , 0 , 0 , &s_color.w , &s_color.h);
        SDL_Rect s_white , d_white ;
        SDL_QueryTexture(WhiteColor , 0 , 0 , &s_white.w , &s_white.h ) ;
        SDL_Rect s_sel , d_sel ;
        SDL_QueryTexture(selector , 0 , 0 , &s_sel.w , &s_sel.h) ;
        d_sel.w = s_sel.w ;
        d_sel.h = s_sel.h ;
        s_sel.x = s_sel.y = 0 ;
        d_sel.y  = 750 ;
        int i ;
        for ( int i = 0 ; i <3 ; i++)
        {
                d_white.w = 70;
                d_white.h = 70;
                s_white.x = s_white.y = 0;
                d_white.x = 140 + i*100;
                d_white.y = 675;
                SDL_RenderCopy(renderer, WhiteColor, &s_white, &d_white);
                if(color_state[i] == 1)
                {
                        d_sel.x = 143+i*100 ;
                        SDL_RenderCopy(renderer , selector , &s_sel , &d_sel) ;
                }
        }
        for ( int i = 0 ; i < 3 ; i++)
        {
                d_color.w = 60;
                d_color.h = 60;
                s_color.x = s_color.y = 0;
                d_color.x = 145 + i*100;
                d_color.y = 680;
                SDL_RenderCopy(renderer , colors[i] , &s_color , &d_color);
        }
        for ( int i = 3 ; i < 10  ; i++)
        {
                if(color_state[i] == 1)
                {
                        d_sel.x = 243+i*100 ;
                        SDL_RenderCopy(renderer , selector , &s_sel , &d_sel) ;
                }
                d_white.w = 70;
                d_white.h = 70;
                s_white.x = s_white.y = 0;
                d_white.x = 240 + i*100;
                d_white.y = 675;
                SDL_RenderCopy(renderer, WhiteColor, &s_white, &d_white);
        }
        for ( int i = 3 ; i < 10 ; i++)
        {
                d_color.w = 60;
                d_color.h = 60;
                s_color.x = s_color.y = 0;
                d_color.x = 245 + i*100;
                d_color.y = 680;
                SDL_RenderCopy(renderer , colors[i] , &s_color , &d_color);
        }
}
void renderTools()
{
        SDL_Rect s , d ;
        SDL_QueryTexture(tools[0] , 0 , 0 , &s.w , &s.h) ;
        SDL_Rect s_sel , d_sel ;
        SDL_QueryTexture(selector , 0 , 0 , &s_sel.w , &d_sel.h) ;
        s_sel.x = s_sel.y = 0 ;
        d_sel.x = 20 ;
        d_sel.w= s_sel.w ;
        d_sel.h= s_sel.h ;
        int i ;
        for ( i = 0 ; i < 7 ; i++)
        {
                s.x = s.y = 0 ;
                d.x = 10 ;
                d.y = 30 + i*100 ;
                d.h = s.h ;
                d.w = s.w ;
                SDL_RenderCopy(renderer , tools[i] , &s , &d) ;
                if(tool_state[i] == 1)
                {
                        d_sel.y = 119+i*100 ;
                        SDL_RenderCopy(renderer , selector , &s_sel , &d_sel) ;
                }
        }
}
void render()
{
       SDL_UpdateTexture(Canvas, NULL, pixels, (screenwidth-110) * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer , background , 0 , 0 ) ;
        SDL_Rect s_canvas;
        SDL_Rect d_canvas;
        SDL_QueryTexture(Canvas , 0 , 0 , &s_canvas.w , &s_canvas.h);
        s_canvas.x = s_canvas.y = 0;
        d_canvas.x = 110;
        d_canvas.y = 0;
        d_canvas.w = s_canvas.w;
        d_canvas.h = s_canvas.h;
        renderColors() ;
        renderTools() ;
        SDL_RenderCopy(renderer, Canvas, &s_canvas, &d_canvas);
        SDL_RenderPresent(renderer);

}
void initBackground()
{
        SDL_Surface* back = IMG_Load("./res/grids.png");
        background = SDL_CreateTextureFromSurface(renderer , back);
        SDL_FreeSurface(back);
}
void initColors()
{
        SDL_Surface* base = IMG_Load("./res/white.png");
        WhiteColor = SDL_CreateTextureFromSurface(renderer , base);
        SDL_FreeSurface(base);
        SDL_Surface* temp;
        temp = IMG_Load("./res/black.png");
        colors[0] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/white1.png");
        colors[1] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/grey.png");
        colors[2] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/red.png");
        colors[3] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/orange.png");
        colors[4] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/yellow.png");
        colors[5] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/pink.png");
        colors[6] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/cyan.png");
        colors[7] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/blue.png");
        colors[8] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/green.png");
        colors[9] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);
}
void initTools()
{
        SDL_Surface *tool_surface = IMG_Load("./res/bucket2.png") ;
        tools[0] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/brush2.png") ;
        tools[1] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/brush1.png") ;
        tools[2] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/eraser.png") ;
        tools[3] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/penciltool.png") ;
        tools[4] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/clear.png") ;
        tools[5] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/exportPdf.png") ;
        tools[6] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

}
int main(int argc, char ** argv)
{
        int success = init();
        initBackground();
        initColors();
        initTools() ;

        while (!quit && success)
        {
                handleEvent();
                render();
        }

        free(pixels);
        SDL_DestroyTexture(Canvas);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
}
