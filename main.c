#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL2_gfxPrimitives.h>
#include<libpng12/png.h>
#include<string.h>
#define TOTAL_TOOLS 7
#define TOTAL_COLORS 10
#define TOTAL_PRIMARY_COLORS 3
int screenwidth ;
int screenheight ;
int offset_width = 110 ;
int offset_height = 100 ;
Uint32* pixels;
int leftMouseButtonDown = 0;
int quit = 0;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture_background;
SDL_Texture* Canvas;
SDL_Texture* texture_colors[TOTAL_COLORS];
SDL_Texture* texture_tools[TOTAL_TOOLS];
SDL_Texture *canvas_tools[5] ;
int color_state[TOTAL_COLORS] ;
int tool_state[TOTAL_TOOLS] ;
SDL_Texture *selector ;
SDL_Texture* WhiteColor;
SDL_Texture* isSelected;
 SDL_DisplayMode screen ;
int xold , yold , xnew , ynew ;
int currentColor ;
int currentTool  ;
int previousTool ;
int xCenter , yCenter , x ,y ;
int checkMotion = 0 , radius = 0 ;
typedef struct colorElement
{
        int r ;
        int g ;
        int b ;
}ColorElement ;
ColorElement avail_Colors[TOTAL_COLORS] = {{0 , 0 , 0} , {255 , 255 , 255} , {194 ,194 ,194} , {255 , 0 , 0} , {237 , 114 , 5} ,
					{255 , 255 , 0} , {221, 101 , 225} , {0 , 255 , 255} , {0 , 0 , 255} , {27 , 119 , 10}};
/*	avail_Colors[0] =  ;
        avail_Colors[1] =  ;
        avail_Colors[2] =  ;
        avail_Colors[3] = {255 , 0 , 0} ;
        avail_Colors[4] = {237 , 114 , 5} ;
        avail_Colors[5] = {255 , 255 , 0} ;
        avail_Colors[6] = {221, 101 , 225} ;
        avail_Colors[7] = {0 , 255 , 255} ;
        avail_Colors[8] = {0 , 0 , 255} ;
        avail_Colors[9] = {27 , 119 , 10} ;*/
enum
{
        TOOL_PENCIL , //# 0
        TOOL_BRUSH , //# 1
        TOOL_BRUSH_THICK , //# 2
        TOOL_ERASER , //# 3
        TOOL_BUCKET , //# 4
        TOOL_PNG , //# 5
        TOOL_CLEAR //# 6
} ;
enum
{
        COLOR_BLACK , // 0
        COLOR_WHITE , // 1
        COLOR_GREY , // 2
        COLOR_RED , // 3
        COLOR_ORANGE , // 4
        COLOR_YELLOW , // 5
        COLOR_PINK , // 6
        COLOR_CYAN , // 7
        COLOR_BLUE , // 8
        COLOR_GREEN // 9
} ;
// Funtions Prototypes
int init() ;  // Initialization of SDL
void initBackground() ; // Load Background IMAGE to surface
void initColors() ;  // Load Colors IMAGE to surface
void initTools() ; // Load Tools IMAGE to surface
void handleColors(SDL_Event * event) ; // Handling with Color Buttons
void handleColorsWithKeyboard(SDL_Event * event) ;
void handleTools(SDL_Event * event) ;  // Handling with Tool Buttons
void handleToolsWithKeyboard(SDL_Event * event) ; // Handlin Tools Buttons with KeyBoard
void renderColors() ; // Display Color Buttons on screen(window of computer screen)
void renderTools() ;  // Display Tool Buttons on screen(window of computer screen)
void putPixel_Screen(int x , int y , Uint32 c) ; // Checking pixel position and put it on Canvas
void putPixel(int width , int x , int y , int r , int g ,int b) ; // Put pixel with different width onCanvas
void swap(int*a , int * b ) ; // Swap two integer values
void drawLine(  int x1 , int y1 , int x2 , int y2) ; // Draw a Line Between two Positions on Canvas
void erase( int x , int y) ; // Erase pixels on Canvas
void getPixel(int x , int y , int*r , int*g , int* b) ; // Get color of  particular pixel from Canvas
int colors_close(int r1 , int g1 , int b1 , int r2 , int g2 , int b2) ; // Check Closeness for two colors
void do_flood_fill(int x , int y , int r1 , int g1 , int b1 , int r2 , int g2 , int b2) ; // Fill Canvas with color
void clearCanvas() ; // Clear Canvas or Make a new window
void writeImage() ; // Save image in PNG formate
void renderTooI_icons() ; // Movement of tool icons with mouse on Canvas
void renderBackgroundImage() ;
void renderCanvas() ;
void putEraserRectangleOnCanvas() ;
void putToolIconsOnCanvas() ;
void onKeyDown(SDL_Event *event) ;
void onMouseButtonDown(SDL_Event *event) ;
void onMouseButtonUp(SDL_Event *event) ;
void onMouseMotion(SDL_Event *event) ;
void handleEvent() ; // Handle Mouse And Other Events
void render() ; // Display Everthing on screen
void drawAALine(int x1 , int y1 , int x2 , int y2) ;
void drawCircle(int centerX , int centerY , int radius);
void circlePutPixel(int X , int Y , int P , int Q) ;
void plotCircle(int xm , int ym , int r) ;
// Functions definations
int init()
{
        // Initialize everything in sdl
        SDL_Init(SDL_INIT_EVERYTHING) ;
	int i , z ;
        // Get Screen Resolution
        for(i = 0 ; i < SDL_GetNumVideoDisplays() ; i++ )
                z = SDL_GetCurrentDisplayMode( i , &screen) ;
        int width = screen.w ;

        screenwidth = screen.w ;
        screenheight = screen.h ;
//        printf("%d\n%d\n",screenheight,screenwidth);
        int canvasWidth = screenwidth - offset_width ;
        int canvasHeight = screenheight - offset_height ;

        window = SDL_CreateWindow("Paint Max",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenwidth, screenheight, SDL_WINDOW_FULLSCREEN);

        //Checking Window is NULL or NOT
        if(window == NULL)
                return 0;
        // Create  a Renderer
        renderer = SDL_CreateRenderer(window, -1, 0);
        if(renderer == NULL)
                return 0;
        // Make a texture of canvaswidth and canvasheight
        Canvas = SDL_CreateTexture(renderer,
                        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, canvasWidth ,canvasHeight);
        // Allocating memeory to pixels
        pixels = (Uint32*)malloc( sizeof(Uint32)*canvasWidth * canvasHeight);
        // Set Pixels in Memory With White Background
        memset(pixels, 255 , canvasWidth*canvasHeight * sizeof(Uint32));
        
	// Make ToolState and ColorState to NOT SELECTED
        for ( i = 0 ;i <TOTAL_COLORS ; i++)
        {
                color_state[i] = 0;
        }
        for( i = 0 ; i < TOTAL_TOOLS ; i++)
        {
                tool_state[i] = 0 ;
        }
        // Load image "selector.png" to surface
        SDL_Surface *s = IMG_Load("./res/selector.png") ;
        selector = SDL_CreateTextureFromSurface(renderer , s) ;
        SDL_FreeSurface(s) ;
        // Set Position of Cursor to Middle of Canvas
        xold = xnew = canvasWidth/2;
        yold = ynew = canvasHeight/2;
        // Put Cursor in Middle of Canvas
        SDL_WarpMouseInWindow(window , xold , yold) ;
        // Initially Selected Pencil And Black Color
        currentColor = 0 ;
        currentTool = 0 ;
        color_state[0] = 1;
        tool_state[0] = 1 ;
        //drawAALine(120 , 50, 150 , 140) ;
        //drawAALine(280 , 280 , 400 , 380) ;
        //drawLine(280 , 280 , 400 , 350) ;
//        drawAALine(400 , 400 , 800 , 600) ;
//        drawAALine(500 , 300 , 100 , 200) ;
        //plotCircle(500 , 300 , 200) ;
        drawCircle(300 , 500 , 100) ;
        previousTool = 0 ;
        return 1;

}
void initBackground()
{
        /* Load image and create a texture from that image */
        SDL_Surface* back;
        if(screenwidth == 1366)
               back = IMG_Load("./res/grids.png");
        else
               back = IMG_Load("./res/grids1080.png");
        texture_background = SDL_CreateTextureFromSurface(renderer , back);
        SDL_FreeSurface(back);
}
void initColors()
{
        /* Load image and create a texture from that image */
        SDL_Surface* base = IMG_Load("./res/white.png");
        WhiteColor = SDL_CreateTextureFromSurface(renderer , base);
        SDL_FreeSurface(base);

        SDL_Surface* temp;
        temp = IMG_Load("./res/black.png");
        texture_colors[0] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/white1.png");
        texture_colors[1] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/grey.png");
        texture_colors[2] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/red.png");
        texture_colors[3] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/orange.png");
        texture_colors[4] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/yellow.png");
        texture_colors[5] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/pink.png");
        texture_colors[6] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/cyan.png");
        texture_colors[7] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/blue.png");
        texture_colors[8] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("./res/green.png");
        texture_colors[9] = SDL_CreateTextureFromSurface(renderer , temp);
        SDL_FreeSurface(temp);
}
void initTools()
{
        /* Load image and create a texture from that image */
        SDL_Surface *tool_surface  ;
        tool_surface = IMG_Load("./res/penciltool.png") ;
        texture_tools[0] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/brush1.png") ;
        texture_tools[1] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/brush2.png") ;
        texture_tools[2] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/eraser.png") ;
        texture_tools[3] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/bucket2.png") ;
        texture_tools[4] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/exportPdf.png") ;
        texture_tools[5] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/clear.png") ;
        texture_tools[6] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

}
void handleColors(SDL_Event *event)
{
        int i  , xPositionFirstColorButton = 140 , y = screenheight - offset_height ;
        int offsetBetweenColors = 30 ;
        int widthColorButton = 70 ;
        int difference_colorButtons  = 100 ;
        /* Handle Mouse Motion with different  xand y  poistions on screen i.e. handle that which color is SELECTED or NOT SELECTED */
        for ( i = 0 ; i < TOTAL_PRIMARY_COLORS ; i++)
        {

                if(event->motion.x > xPositionFirstColorButton + i*difference_colorButtons && event->motion.x <  xPositionFirstColorButton + (i+1)*widthColorButton+i*offsetBetweenColors
                && event->motion.y > y +7 && event->motion.y < screenheight )
                {
                        color_state[i] = 1 ;
                        int j ;
                        for( j = 0 ; j < TOTAL_COLORS ; j++)
                                if(j!=i)
                                        color_state[j] = 0 ;
                        currentColor = i ;
                }
        }
        int   xPositionFourthColor = 240 ;
        for ( i = TOTAL_PRIMARY_COLORS ; i < TOTAL_COLORS ; i++)
        {

                if(event->motion.x > xPositionFourthColor + i*difference_colorButtons && event->motion.x < xPositionFourthColor + (i+1)*widthColorButton + i*offsetBetweenColors
                && event->motion.y >screenheight - offset_height +7 && event->motion.y < screenheight)
                {
                        color_state[i] = 1 ;
                         int j ;
                        for( j = 0 ; j < TOTAL_COLORS ; j++)
                                if(j!=i)
                                        color_state[j] = 0 ;
                        currentColor = i ;
                }

        }
}
void handleColorsWithKeyboard(SDL_Event * event)
{
        int i ;
        if(event->key.keysym.scancode == SDL_SCANCODE_0 || event->key.keysym.sym == SDLK_0)
        {
                color_state[COLOR_BLACK] = 1 ;
                for(i = 0 ; i < TOTAL_COLORS ; i++)
                        if(COLOR_BLACK!=i)
                                color_state[i] = 0 ;
                currentColor = COLOR_BLACK ;
        }
        else if(event->key.keysym.scancode == SDL_SCANCODE_1|| event->key.keysym.sym == SDLK_1)
        {
                color_state[COLOR_WHITE] = 1 ;
                for(i = 0 ; i < TOTAL_COLORS ; i++)
                        if(COLOR_WHITE!=i)
                                color_state[i] = 0 ;
                currentColor = COLOR_WHITE ;
        }
        else if(event->key.keysym.scancode == SDL_SCANCODE_2 || event->key.keysym.sym == SDLK_2)
        {
                color_state[COLOR_GREY] = 1 ;
                 for(i = 0 ; i < TOTAL_COLORS ; i++)
                        if(COLOR_GREY!=i)
                                color_state[i] = 0 ;
                currentColor = COLOR_GREY  ;
        }
         else if(event->key.keysym.scancode == SDL_SCANCODE_3 || event->key.keysym.sym == SDLK_3)
        {
                color_state[COLOR_RED] = 1 ;
                for(i = 0 ; i < TOTAL_COLORS ; i++)
                        if(COLOR_RED!=i)
                                color_state[i] = 0 ;
                currentColor =COLOR_RED ;
        }
        else if(event->key.keysym.scancode == SDL_SCANCODE_4 || event->key.keysym.sym == SDLK_4)
        {
                color_state[COLOR_ORANGE] = 1 ;
                for(i = 0 ; i < TOTAL_COLORS ; i++)
                        if(COLOR_ORANGE!=i)
                                color_state[i] = 0 ;
                currentColor = COLOR_ORANGE ;
        }
        else if(event->key.keysym.scancode == SDL_SCANCODE_5 || event->key.keysym.sym == SDLK_5)
        {
                color_state[COLOR_YELLOW] = 1 ;
                for(i = 0 ; i < TOTAL_COLORS ; i++)
                        if(COLOR_YELLOW!=i)
                                color_state[i] = 0 ;
                currentColor = COLOR_YELLOW ;
        }
        else if(event->key.keysym.scancode == SDL_SCANCODE_6 || event->key.keysym.sym == SDLK_6)
        {
                color_state[COLOR_PINK] = 1 ;
                for(i = 0 ; i < TOTAL_COLORS ; i++)
                        if(COLOR_PINK!=i)
                                color_state[i] = 0 ;
                currentColor = COLOR_PINK ;
        }
        else if(event->key.keysym.scancode == SDL_SCANCODE_7 || event->key.keysym.sym == SDLK_7)
        {
                color_state[COLOR_CYAN] = 1 ;
                 for(i = 0 ; i < TOTAL_COLORS ; i++)
                        if(COLOR_CYAN!=i)
                                color_state[i] = 0 ;
                currentColor = COLOR_CYAN  ;
        }
        else if(event->key.keysym.scancode == SDL_SCANCODE_8 || event->key.keysym.sym == SDLK_8)
        {
                color_state[COLOR_BLUE] = 1 ;
                for(i = 0 ; i < TOTAL_COLORS ; i++)
                        if(COLOR_BLUE!=i)
                                color_state[i] = 0 ;
                currentColor =COLOR_BLUE ;
        }
        else if(event->key.keysym.scancode == SDL_SCANCODE_9 || event->key.keysym.sym == SDLK_9)
        {
                color_state[COLOR_GREEN] = 1 ;
                for(i = 0 ; i < TOTAL_COLORS ; i++)
                        if(COLOR_GREEN!=i)
                                color_state[i] = 0 ;
                currentColor = COLOR_GREEN ;
        }
}
void handleTools(SDL_Event *event)
{
        /* Handle Tools i.e. SELECTED or NOT SELECTED */
        int i ;
        int fixed_xPos1 = 10 ;
        int fixed_xPos2 = 95 ;
        int yPosFirstToolButton = 30 ;
        int offsetBetweenToolButtons= 15 ;
        int difference_toolButtons  = 100 ;
        int height_ToolButton = 85 ;
        for ( i = 0 ; i < TOTAL_TOOLS ; i++)
        {
                if(event->motion.x > fixed_xPos1 && event->motion.x < fixed_xPos2
                && event->motion.y > yPosFirstToolButton+i*difference_toolButtons
                && event->motion.y < yPosFirstToolButton + (i+1) * height_ToolButton + i*offsetBetweenToolButtons)
                {
                        tool_state[i]  = 1 ;
//                        if(currentTool<3)
//                        previousTool = currentTool ;
                        int j ;
                        for(j = 0 ; j < TOTAL_TOOLS ; j++)
                                if(i!=j)
                                        tool_state[j] = 0 ;
                        currentTool = i ;
                }

        }
}
void handleToolsWithKeyboard(SDL_Event * event)
{
        int i ;
        if(event->key.keysym.scancode == SDL_SCANCODE_P)
        {
                tool_state[TOOL_PENCIL] = 1 ;
                for(i = 0 ; i < TOTAL_TOOLS ; i++)
                        if(TOOL_PENCIL!=i)
                                tool_state[i] = 0 ;
                currentTool = TOOL_PENCIL ;
        }
        else if(event->key.keysym.scancode == SDL_SCANCODE_B)
        {
                tool_state[TOOL_BRUSH] = 1 ;
                for(i = 0 ; i < TOTAL_TOOLS ; i++)
                        if(TOOL_BRUSH!=i)
                                tool_state[i] = 0 ;
                currentTool = TOOL_BRUSH ;
        }
        else if(event->key.keysym.scancode == SDL_SCANCODE_T)
        {
                tool_state[TOOL_BRUSH_THICK] = 1 ;
                 for(i = 0 ; i < TOTAL_TOOLS ; i++)
                        if(TOOL_BRUSH_THICK!=i)
                                tool_state[i] = 0 ;
                currentTool = TOOL_BRUSH_THICK  ;
        }
        else if(event->key.keysym.scancode == SDL_SCANCODE_E)
        {
                tool_state[TOOL_ERASER] = 1 ;
                for(i = 0 ; i < TOTAL_TOOLS ; i++)
                        if(TOOL_ERASER!=i)
                                tool_state[i] = 0 ;
                currentTool =TOOL_ERASER ;
        }
        else if(event->key.keysym.scancode == SDL_SCANCODE_F)
        {
                tool_state[TOOL_BUCKET] = 1 ;
                for(i = 0 ; i < TOTAL_TOOLS ; i++)
                        if(TOOL_BUCKET!=i)
                                tool_state[i] = 0 ;
                currentTool = TOOL_BUCKET ;
        }
//        else if(event->key.keysym.scancode == SDL_SCANCODE_C)
//        {
//                tool_state[6] = 1 ;
//                for(i = 0 ; i < TOTAL_TOOLS ; i++)
//                        if(6!=i)
//                                tool_state[i] = 0 ;
//                currentTool = TOOL_CLEAR ;
//        }
}
void renderColors()
{
        SDL_Rect s_color , d_color ;
        SDL_QueryTexture(texture_colors[7] , 0 , 0 , &s_color.w , &s_color.h);
        SDL_Rect s_white , d_white ;
        SDL_QueryTexture(WhiteColor , 0 , 0 , &s_white.w , &s_white.h ) ;
        SDL_Rect s_sel , d_sel ;
        SDL_QueryTexture(selector , 0 , 0 , &s_sel.w , &s_sel.h) ;
        d_sel.w = s_sel.w ;
        d_sel.h = s_sel.h ;
        s_sel.x = s_sel.y = 0 ;
        d_sel.y  = screenheight-14 ;
        int i ;
        int xPositionFirstColorButton = 140 ,difference_colorButtons  = 100 ;
        for ( int i = 0 ; i <3 ; i++)
        {
                d_white.w = 70;
                d_white.h = 70;
                s_white.x = s_white.y = 0;
                d_white.x = 140 + i*100;
                d_white.y = screenheight - offset_height +7;
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
                d_color.y = screenheight - offset_height + 12;
                SDL_RenderCopy(renderer , texture_colors[i] , &s_color , &d_color);
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
                d_white.y = screenheight - offset_height +7 ;
                SDL_RenderCopy(renderer, WhiteColor, &s_white, &d_white);
        }
        for ( int i = 3 ; i < 10 ; i++)
        {
                d_color.w = 60;
                d_color.h = 60;
                s_color.x = s_color.y = 0;
                d_color.x = 245 + i*100;
                d_color.y = screenheight - offset_height +12 ;
                SDL_RenderCopy(renderer , texture_colors[i] , &s_color , &d_color);
        }
}
void renderTools()
{
        SDL_Rect s , d ;
        SDL_QueryTexture(texture_tools[0] , 0 , 0 , &s.w , &s.h) ;
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
                SDL_RenderCopy(renderer , texture_tools[i] , &s , &d) ;
                if(tool_state[i] == 1)
                {
                        d_sel.y = 119+i*100 ;
                        SDL_RenderCopy(renderer , selector , &s_sel , &d_sel) ;
                }
        }
}
void putPixel_Screen(int x , int y , Uint32 c)
{
        if( x >= 0 && y>=0 && x< screenwidth-offset_width && y<screenheight-offset_height)
        {
                pixels[y*(screenwidth - offset_width) + x] = c ;

        }
}
void putPixel( int width , int x , int y, int r , int g , int b)
{
        Uint32 c ;
        Uint8 *p = (Uint8 *)(&c) ;
        p[0] = b;
        p[1] = g ;
        p[2] = r ;
        p[3] = 255 ;
                if(width == 0)
                {
                        putPixel_Screen(x , y , c ) ;
                }
                if(width == 1)
                {
                        putPixel_Screen(x, y ,c);
                        putPixel_Screen(x+1 , y , c) ;
                        putPixel_Screen(x+1,y+1, c) ;
                        putPixel_Screen(x,y+1,c);

                }
                 if(width == 2)
                {
                        int i , j ;
                        for( i = 0 ; i < 4 ; i++)
                        {
                                for( j = 0 ; j < 4 ; j++)
                                        putPixel_Screen(x+j , y+i , c) ;
                        }
                }
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
        if(x1 == x2 && y1 == y2)
                putPixel(currentTool , x1 ,y1 , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b);
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
                        putPixel(currentTool , i , j  , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b ) ;
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
                        putPixel(currentTool  , j , i  , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b  ) ;
                        j = j + m ;
                }
        }

}
void circlePutPixel(int xCenter , int yCenter , int x  , int y)
{
        putPixel(currentTool , xCenter+x , yCenter+y , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b ) ;
        putPixel(currentTool , xCenter+x , yCenter-y , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b ) ;
        putPixel(currentTool , xCenter-x , yCenter+y , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b ) ;
        putPixel(currentTool , xCenter-x , yCenter-y , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b ) ;

        putPixel(currentTool , xCenter+y , yCenter+x , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b) ;
        putPixel(currentTool , xCenter+y , yCenter-x , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b ) ;
        putPixel(currentTool , xCenter-y , yCenter+x , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b ) ;
        putPixel(currentTool , xCenter-y , yCenter-x , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b ) ;
}
void drawCircle(int xCenter , int yCenter  , int radius)
{
        int x = 0 , y  = radius ;
        int d = 1 - radius ;
        circlePutPixel(xCenter , yCenter , x , y) ;
        while( x < y)
        {
                x++ ;
                if( d < 0)
                {
                        d = d+ 2 * x + 1;
                }
                else
                {
                        y-- ;
                        d = d + 2*(x-y) + 1 ;
                }
                circlePutPixel(xCenter , yCenter , x , y) ;
        }
}
//void plotCircle(int xm, int ym, int r)
//{
//        int x = -r, y = 0, err = 2-2*r; /* II. Quadrant */
//        do
//        {
//                putPixel(currentTool , xm-x, ym+y , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b ); /*   I. Quadrant */
//                putPixel(currentTool , xm-y, ym-x, avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b); /*  II. Quadrant */
//                putPixel(currentTool , xm+x, ym-y, avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b); /* III. Quadrant */
//                putPixel(currentTool , xm+y, ym+x, avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b); /*  IV. Quadrant */
//                r  = err;
//                if (r <= y)
//                        err += ++y*2+1;           /* e_xy+e_y < 0 */
//                if (r > x || err > y)
//                        err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
//        } while (x < 0);
//}
//double calculateAlpha(double distance)
//{
//        printf("distance : %.2f     " , distance ) ;
//        double a ;
////        if(distance > 0 )
////                distance = distance - abs(distance) ;
////        else
////                distance = abs(distance - abs(distance)) ;
////        if(fabs(distance) >=0.0 && fabs(distance)<=1.5)
//        a  = (1- (fabs(distance) * 0.667)) * (1- (fabs(distance) * 0.667));
//        printf("intensity : %.2f " , a) ;
////        else
////                a = 255 / fabs(distance) ;
//        return a ;
//}
//void DrawPixelId(int x , int y , double distance , int r , int g , int b)
//{
//        printf("x : %d , y : %d " , x , y) ;
//        double  intensity = calculateAlpha((distance)) ;
//        //printf("alpha : %d\n" , alpha) ;
////        if ( alpha > 255 && alpha < alpha-255)
////                alpha = alpha - 255 ;
////        else if( alpha >=0 && alpha <=255)
//                //alpha = 255-alpha ;
////        else
////                alpha = 255 ;
////        printf("alpha : %d\n" , alpha) ;
//        double alpha = 255  - (intensity * 255) ;
//        printf("alpha : %.1f\n", alpha) ;
//        putPixel(0 , x , y , r+(int)alpha , g+(int)alpha , b+(int)alpha) ;
//}
//void drawAALine(int x1 , int y1 , int x2 , int y2)
//{
//        int dx = x2 - x1 ;
//        int dy = y2 - y1 ;
//        int du , u , uincr ;
//        int dv , v , vincr ;
//        if(abs(dx) >= abs(dy))
//        {
//                du = abs(dx) ;
//                dv = abs(dy) ;
//                u = x1 ;
//                v = y1 ;
//        }
//        else
//        {
//                du = abs(dy) ;
//                dv = abs(dx) ;
//                u = y1 ;
//                v = x1 ;
//        }
//        int uend = u + du ;
//        int d = 2 * dv - du ;
//        int incrS = 2 * dv ;
//        int incrD = 2 * (dv-du) ;
//        int twovdu = 0 ;
//        double invD = 1.0/(2.0 * sqrt(du*du+dv*dv)) ;
//        double invD2du = 2.0 * invD * du ;
//        do
//        {
//                DrawPixelId(u , v , twovdu*invD , avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b ) ;
//                DrawPixelId(u , v+1 , invD2du - twovdu*invD ,  avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b ) ;
//                DrawPixelId(u , v-1, invD2du + twovdu*invD ,  avail_Colors[currentColor].r ,avail_Colors[currentColor].g , avail_Colors[currentColor].b ) ;
//                if(d < 0)
//                {
//                        twovdu = u + du ;
//                        d = d + incrS ;
//                }
//                else
//                {
//                        twovdu = u - du ;
//                        d = d + incrD ;
//                        v = v + 1 ;
//                }
//                u = u + 1 ;
//        }while( u < uend) ;
//}

void erase( int x , int y)
{
        int i , j ;
        for( i = x-24 ; i < x+25 ; i++)
        {
                for( j = y-24 ; j < y+25 ; j++)
                {
                        if( i >=0 && j >=0)
                                putPixel(1 , i , j , 255 ,255 ,255) ;
                }
        }
}
void getPixel(int x , int y , int*r , int*g , int* b)
{
        Uint32 color = pixels[y*(screenwidth -offset_width) + x];
        Uint8 * p = (Uint8*)(&color);
        *r = p[2];
        *g=  p[1];
        *b = p[0];
}
int colors_close(int r1 , int g1 , int b1 , int r2 , int g2 , int b2)
{
        if (r1==r2 && g1==g2 && b1 == b2)
        {
                return 1;
        }
        else
        {
                int r, g, b;

                r = r1 - r2 ;
                r *= r;
                g = g1 - g2;
                g *= g;
                b = b1 - b2;
                b *= b;
                return r + g + b < 1;
        }
}
void do_flood_fill(int x, int y,int r_new , int g_new , int b_new, int r_old,int g_old , int b_old)
{
        int fillL, fillR, i, in_line;
        if (colors_close(r_new,g_new,b_new, r_old,g_old,b_old))
        return;


        fillL = x;
        fillR = x;

        in_line = 1;

        while (in_line)
        {
                putPixel(0 , fillL, y, r_new,g_new,b_new);
                fillL--;
                int r_canvas , g_canvas , b_canvas ;
                getPixel(fillL , y , &r_canvas ,&g_canvas , &b_canvas);
                in_line =
                (fillL < 0) ? 0 : colors_close( r_canvas , g_canvas , b_canvas,r_old,g_old,b_old);
        }

        fillL++;

        /* Find right side, filling along the way */

        in_line = 1;
        while (in_line)
        {
                putPixel(0,fillR, y,r_new,g_new,b_new);
                fillR++;
                int r_canvas , g_canvas , b_canvas ;
                getPixel(fillR , y , &r_canvas ,&g_canvas , &b_canvas);
                in_line = (fillR >= screenwidth-offset_width) ? 0 : colors_close( r_canvas , g_canvas , b_canvas,r_old,g_old,b_old);
        }
        fillR--;

        for (i = fillL; i <= fillR; i++)
        {
                 int r_canvas , g_canvas , b_canvas ;
                if(y>0)
                        getPixel(i , y-1 , &r_canvas ,&g_canvas , &b_canvas);

                if (y > 0 && colors_close(r_canvas , g_canvas , b_canvas,r_old,g_old,b_old))
                {
                        do_flood_fill(i, y - 1, r_new,g_new,b_new, r_old,g_old,b_old);
                }
                if(y < screenheight-offset_height -1)
                        getPixel(i , y+1 , &r_canvas ,&g_canvas , &b_canvas);

                if (y < screenheight-offset_height && colors_close(r_canvas , g_canvas , b_canvas,r_old,g_old,b_old))
                {
                        do_flood_fill(i, y + 1, r_new,g_new,b_new, r_old,g_old,b_old);
                }
        }
}
void clearCanvas()
{
        memset(pixels, 255, (screenwidth-offset_width)*(screenheight-offset_height) * sizeof(Uint32));
}
void putTools()
{
        SDL_Surface *temp = IMG_Load("./res/pencil.png") ;
        canvas_tools[0] = SDL_CreateTextureFromSurface(renderer , temp ) ;
        SDL_FreeSurface(temp ) ;

        temp = IMG_Load("./res/small_brush.png") ;
        canvas_tools[1] = SDL_CreateTextureFromSurface(renderer , temp ) ;
        SDL_FreeSurface(temp ) ;

        temp = IMG_Load("./res/brush4.png") ;
        canvas_tools[2] = SDL_CreateTextureFromSurface(renderer , temp ) ;
        SDL_FreeSurface(temp ) ;

        temp = IMG_Load("./res/bucket-icon.png") ;
        canvas_tools[3] = SDL_CreateTextureFromSurface(renderer , temp ) ;
        SDL_FreeSurface(temp ) ;
}
void renderTooI_icons()
{
        SDL_Rect s_tool , d_tool ;
        s_tool.x = s_tool.y = 0 ;
        int i ;
        if(currentTool == TOOL_PENCIL )
        {
                SDL_QueryTexture(canvas_tools[0] , 0 , 0 , &s_tool.w , &s_tool.h) ;
                d_tool.w = s_tool.w ;
                d_tool.h = s_tool.h ;
                int x_pos_of_pencil_tip = 110 ;
                int y_pos_of_pencil_tip = 28 ;
                d_tool.x = xnew +x_pos_of_pencil_tip ;
                d_tool.y =  ynew - y_pos_of_pencil_tip ;
                SDL_RenderCopy(renderer , canvas_tools[0] , &s_tool , &d_tool) ;
        }
        else if(currentTool == TOOL_BRUSH)
        {
                SDL_QueryTexture(canvas_tools[1] , 0 , 0 , &s_tool.w , &s_tool.h) ;
                d_tool.w = s_tool.w ;
                d_tool.h = s_tool.h ;
                int x_pos_of_brush_tip= 110 ;
                int y_pos_of_brush_tip = 35 ;
                d_tool.x = xnew+x_pos_of_brush_tip ;
                d_tool.y =  ynew -y_pos_of_brush_tip;
                SDL_RenderCopy(renderer , canvas_tools[1] , &s_tool , &d_tool) ;
        }
        else if(currentTool == TOOL_BRUSH_THICK )
        {
                SDL_QueryTexture(canvas_tools[2] , 0 , 0 , &s_tool.w , &s_tool.h) ;
                d_tool.w = s_tool.w ;
                d_tool.h = s_tool.h ;
                int x_pos_of_thick_brush_tip= 98 ;
                int y_pos_of_thick_brush_tip = 41 ;
                d_tool.x = xnew+x_pos_of_thick_brush_tip ;
                d_tool.y =  ynew -y_pos_of_thick_brush_tip;
                SDL_RenderCopy(renderer , canvas_tools[2] , &s_tool , &d_tool) ;
        }
        else if(currentTool == TOOL_BUCKET )
        {
                SDL_QueryTexture(canvas_tools[3] , 0 , 0 , &s_tool.w , &s_tool.h ) ;
                d_tool.w = s_tool.w ;
                d_tool.h = s_tool.h ;
                int x_pos_of_bucket_tip= 106 ;
                int y_pos_of_bucket_tip = 30 ;
                d_tool.x = xnew+x_pos_of_bucket_tip ;
                d_tool.y =  ynew -y_pos_of_bucket_tip;
                SDL_RenderCopy(renderer , canvas_tools[3] , &s_tool , &d_tool) ;
        }
}
void writeImage()
{
        char* buffer = (char*)malloc(sizeof(char)*(screenwidth-offset_width)*(screenheight-offset_height)*3);
        int i;
        int counter=0;
        for(i = 0 ; i <(screenwidth-offset_width)*(screenheight-offset_height); i++)
        {
                Uint8* p = (Uint8*)(pixels+i);

                buffer[counter++] = p[2];
                buffer[counter++] = p[1];
                buffer[counter++] = p[0];
        }
        int code =0 , index ;
        FILE * fp = fopen("saved_images/config.dat" , "rb+") ;
                fseek(fp, 0, SEEK_END);
                fseek(fp , ftell(fp)-sizeof(int) , SEEK_SET);
         int flag = fread(&index , sizeof(int) , 1 , fp);
         if(flag == 0 )
                index= 0 ;
        index++;

        fwrite( &index , sizeof(int) , 1 , fp ) ;
                fclose(fp) ;
        char str[30];
        sprintf(str, "%d", index);
        char fileName[50] = "image";
        strcat(fileName , str) ;
        strcat(fileName , ".png") ;
        char dir[100] = "saved_images/" ;
        strcat(dir , fileName) ;
        fp = fopen(dir , "wb");
        png_structp png_ptr = NULL;
        png_infop info_ptr = NULL;
        png_bytep row = NULL;
        if(fp!=NULL)

        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING , NULL,NULL,NULL);
        info_ptr = png_create_info_struct(png_ptr);

        png_init_io(png_ptr , fp);
        png_set_IHDR(png_ptr , info_ptr , screenwidth-offset_width ,screenheight-offset_height,
        8 , PNG_COLOR_TYPE_RGB , PNG_INTERLACE_NONE , PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE);

        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = "title";
        title_text.text = "hello";
        png_set_text(png_ptr , info_ptr , &title_text , 1);

        png_write_info(png_ptr , info_ptr);
        row = (png_bytep) malloc(3 * (screenwidth-offset_width)* sizeof(png_byte));
        int x , y;
        for(y = 0 ; y<screenheight-offset_height ; y++)
        {
                for(x= 0 ; x<(screenwidth-offset_width)*3 ; x++)
                {
                        row[x] = buffer[y*(screenwidth-offset_width)*3 + x] ;
                }
                png_write_row(png_ptr, row);
        }
        fclose(fp) ;
}
void onKeyDown(SDL_Event *event)
{
    if(event->key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        quit = 1;
    handleColorsWithKeyboard(event) ;
    handleToolsWithKeyboard(event) ;
}
void onMouseButtonUp(SDL_Event *event)
{
    if (event->button.button == SDL_BUTTON_LEFT || event->type == SDL_FINGERUP )
        leftMouseButtonDown = 0;
    if(checkMotion)
    {
        radius = abs(abs(xCenter - xold) * abs(xCenter - xold)) + abs(abs(yCenter - yold) * abs(yCenter - yold)) ;
//        printf("Radius : %d\n" , radius) ;
        radius = sqrt(radius) ;
//        printf("xCenter : %d , yCenter : %d\n" , xCenter , yCenter) ;
//        printf("xold : %d , yold : %d\n" , xold , yold) ;
//        printf("xnew : %d , ynew : %d\n" , xnew , ynew) ;
//        printf("Radius : %d\n" , radius) ;
        drawCircle(xCenter , yCenter , radius) ;
    }
//    if(!leftMouseButtonDown && event.button.state == SDL_RELEASED)
//    {
//            x = xnew ;
//            y = ynew ;
//            printf("Hello") ;
//            printf("x : %d , y : %d\n" , x, y) ;
//    }
}
void onMouseButtonDown(SDL_Event *event)
{
    if(event->button.button == SDL_BUTTON_LEFT || event->type==SDL_FINGERDOWN)
    {
        leftMouseButtonDown = 1;
    }
    handleTools(event) ;
    handleColors(event) ;
    if(currentTool == TOOL_CLEAR)
    {
        clearCanvas() ;
        currentTool = TOOL_PENCIL ;
        tool_state[TOOL_CLEAR] = 0 ;
        tool_state[currentTool] = 1 ;
    }
    if(currentTool == TOOL_PNG)
    {
        writeImage() ;
        currentTool = TOOL_PENCIL ;
        tool_state[TOOL_PNG] = 0 ;
        tool_state[currentTool] = 1 ;
    }
    if(currentTool == TOOL_BUCKET && event->motion.x-offset_width > 0 && event->motion.y < screenheight-offset_height)
    {
        int r1 , g1  ,b1 , r2 , g2 , b2;
        r1 = avail_Colors[currentColor].r;
        g1 = avail_Colors[currentColor].g;
        b1 = avail_Colors[currentColor].b;
        getPixel(xnew  , ynew , &r2 , &g2 ,&b2);
        do_flood_fill(xnew , ynew , r1,g1,b1 , r2 , g2 , b2);
    }
    if(currentTool == TOOL_ERASER && event->button.state == SDL_PRESSED && event->button.clicks == 1)
        erase(xnew , ynew) ;
//        if(currentTool == TOOL_PENCIL || currentTool == TOOL_BRUSH || currentTool == TOOL_BRUSH_THICK)
//        {
//                putPixel(currentTool , xnew , ynew , avail_Colors[currentColor].r, avail_Colors[currentColor].g , avail_Colors[currentColor].b) ;
//        }
    if(leftMouseButtonDown && event->button.state == SDL_PRESSED)
    {
        xCenter = xold ;
        yCenter = yold ;

    }
}
void onMouseMotion(SDL_Event *event)
{
    if(event->type == SDL_MOUSEMOTION)
    {
        xnew = event->motion.x-offset_width ;
        ynew = event->motion.y ;
    }
    if(event->type == SDL_FINGERMOTION)
    {
        xnew = event->tfinger.x ;
        ynew = event->tfinger.y ;
    }
    if(leftMouseButtonDown)
    {
        if(currentTool == TOOL_PENCIL || currentTool == TOOL_BRUSH || currentTool == TOOL_BRUSH_THICK)
        {
            SDL_ShowCursor(SDL_DISABLE) ;
            drawLine(xold , yold , xnew , ynew) ;
            checkMotion = 1 ;

            //drawCircle(xold , yold , 20) ;
        }
        else if(currentTool == TOOL_ERASER)
        {
            erase(xnew , ynew) ;
        }
    }
//    xCenter = xold ;
//    yCenter = yold ;
//printf("xold : %d , yold : %d\n" , xold , yold) ;
    xold = xnew ;
    yold = ynew ;
}
void handleEvent()
{
        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT :
                    quit = 1 ;
            break ;
            case SDL_KEYDOWN:
            {
                onKeyDown(&event) ;
            }
            break ;
            case SDL_FINGERUP : case SDL_MOUSEBUTTONUP:
            {
                onMouseButtonUp(&event) ;
            }
            break;
            case SDL_FINGERDOWN : case SDL_MOUSEBUTTONDOWN :
            {
                onMouseButtonDown(&event) ;
            }
            break ;
            case SDL_FINGERMOTION : case SDL_MOUSEMOTION :
            {
                onMouseMotion(&event) ;
            }
            break;
        }
}
void renderBackgroundImage()
{
         SDL_RenderCopy(renderer , texture_background , 0 , 0 ) ;
}
void renderCanvas()
{
        SDL_Rect canvas_source;
        SDL_Rect canvas_destination;
        SDL_QueryTexture(Canvas , 0 , 0 , &canvas_source.w , &canvas_source.h);
        canvas_source.x = canvas_source.y = 0;
        canvas_destination.x = 110 ;
        canvas_destination.y = 0;
        canvas_destination.w = canvas_source.w;
        canvas_destination.h = canvas_source.h;
        SDL_RenderCopy(renderer, Canvas, &canvas_source, &canvas_destination);
}
void putEraserRectangleOnCanvas()
{
        if(currentTool == TOOL_ERASER && xnew > 0 && ynew < screenheight - offset_height )
        {
                int eraser_rectangle_width = 50 ;
                int eraser_rectangle_height = 50 ;
                int x1 = xnew + offset_width - (eraser_rectangle_width/2) ;
                int y1 = ynew - (eraser_rectangle_height/2) ;
                int x2 = xnew + offset_width + (eraser_rectangle_width/2) ;
                int y2 = ynew + (eraser_rectangle_height/2) ;

                rectangleRGBA(renderer , x1-1 , y1-1, x2 +1, y2+1 , 0 ,0 ,0 ,255) ;
                rectangleRGBA(renderer , x1 , y1 , x2 , y2 , 255, 255 , 255 ,255) ;
                //rectangleRGBA(renderer , x1+1 , y1+1 , x2-1 , y2-1 , 0 , 0 , 0 ,255) ;
                SDL_ShowCursor(SDL_DISABLE) ;
        }
        else
               SDL_ShowCursor(SDL_ENABLE) ;
}
void putToolIconsOnCanvas()
{
        if(xnew >0 && ynew < screenheight - offset_height )
        {
                renderTooI_icons() ;
                SDL_ShowCursor(SDL_DISABLE) ;
        }
        else
                SDL_ShowCursor(SDL_ENABLE) ;
}
void render()
{
        SDL_UpdateTexture(Canvas, NULL, pixels, (screenwidth-offset_width) * sizeof(Uint32));
        SDL_RenderClear(renderer);
        renderBackgroundImage() ;
        renderColors() ;
        renderTools() ;
        renderCanvas() ;
        putEraserRectangleOnCanvas() ;
        putToolIconsOnCanvas() ;
        SDL_RenderPresent(renderer);

}
void clean()
{
        free(pixels);
        SDL_DestroyTexture(texture_background) ;
        int i ;
        for( i = 0 ; i < TOTAL_COLORS ; i++)
                SDL_DestroyTexture(texture_colors[i]) ;
        for( i = 0 ; i < TOTAL_TOOLS ; i++)
                SDL_DestroyTexture(texture_tools[i]) ;
        for ( i = 0 ; i < 5  ; i ++)
                SDL_DestroyTexture(canvas_tools[i]) ;
        SDL_DestroyTexture(Canvas);
        SDL_DestroyTexture(selector);
        SDL_DestroyTexture(WhiteColor);
        SDL_DestroyTexture(isSelected);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
}
int main(int argc, char ** argv)
{
        int success = init();
        initBackground();
        initColors();
        initTools() ;
        putTools() ;
        while (!quit && success)
        {
                handleEvent();
                render();
        }
        clean() ;
        return 0;
}

