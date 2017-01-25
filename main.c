#include<stdio.h>
#include<stdlib.h>
#include <SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL2_gfxPrimitives.h>
#include<libpng12/png.h>
#include<string.h>
int screenwidth ;
int screenheight;
Uint32* pixels;
int leftMouseButtonDown = 0;
int quit = 0;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* background;
SDL_Texture* Canvas;
SDL_Texture* colors[10];
SDL_Texture* tools[10];
SDL_Texture *canvas_tools[5] ;
int color_state[10] ;
int tool_state[10] ;
SDL_Texture *selector ;
SDL_Texture* WhiteColor;
SDL_Texture* isSelected;
 SDL_DisplayMode screen ;
int xold , yold , xnew , ynew ;
int currentColor ;
int currentTool  ;
int previousTool ;
typedef struct colorElement
{
        int r ;
        int g ;
        int b ;
}ColorElement ;
ColorElement avail_Colors[10] ;
enum {
        TOOL_PENCIL ,
        TOOL_BRUSH ,
        TOOL_BRUSH_THICK ,
        TOOL_ERASER ,
        TOOL_BUCKET ,
        TOOL_PNG ,
        TOOL_CLEAR
} ;
// Funtions Prototypes
int init() ;  // Initialization of SDL
void initBackground() ; // Load Background IMAGE to surface
void initColors() ;  // Load Colors IMAGE to surface
void initTools() ; // Load Tools IMAGE to surface
void handleColors(SDL_Event * event) ; // Handling with Color Buttons
void handleTools(SDL_Event * event) ;  // Handling with Tool Buttons
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
void handleEvent() ; // Handle Mouse And Other Events
void render() ; // Display Everthing on screen

// Functions definations
int init()
{
        // Initialize everything in sdl
        SDL_Init(SDL_INIT_EVERYTHING) ;
        // Get Screen Resolution
        for( int i = 0 ; i < SDL_GetNumVideoDisplays() ; i++ )
                int z = SDL_GetCurrentDisplayMode( i , &screen) ;

        screenwidth = screen.w ;
        screenheight = screen.h ;
//        printf("%d\n%d\n",screenheight,screenwidth);
        int canvasWidth = screenwidth-110;
        int canvasHeight = screenheight-100;

        //Checking Screen Resolution for HD AND FULL HD
        if(screenheight == 768 && screenwidth == 1366)
        {
               window = SDL_CreateWindow("Paint",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenwidth, screenheight, SDL_WINDOW_FULLSCREEN);
        }
        else if(screenwidth == 1920 && screenheight == 1080)
        {
                window = SDL_CreateWindow("Paint Max",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenwidth, screenheight, SDL_WINDOW_MAXIMIZED);
        }
        //Checking Window is NULL or NOT
        if(window == NULL)
                return 0;
        // Create  a Renderer
        renderer = SDL_CreateRenderer(window, -1, 0);
        if(renderer == NULL)
                return 0;
        // Make a texture of canvaswidth and canvasheight
        Canvas = SDL_CreateTexture(renderer,
                        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, canvasWidth ,canvasHeight);
        // Allocating memeory to pixels
        pixels = (Uint32*)malloc( sizeof(Uint32)*canvasWidth * canvasHeight);
        // Set Pixels in Memory With White Background
        memset(pixels, 255, canvasWidth*canvasHeight * sizeof(Uint32));
        // Make ToolState and ColorState to NOT SELECTED
        int i ;
        for ( i = 0 ;i <10 ; i++)
        {
                color_state[i] = 0;
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
        // Assigning RGB Values to Color
        avail_Colors[0] = { 0 , 0 , 0} ;
        avail_Colors[1] = { 255 , 255 , 255} ;
        avail_Colors[2] = { 194 ,194 ,194} ;
        avail_Colors[3] = { 255 , 0 , 0} ;
        avail_Colors[4] = { 237 , 114 , 5} ;
        avail_Colors[5] = { 255 , 255 , 0} ;
        avail_Colors[6] = {  221, 101 , 225} ;
        avail_Colors[7] = { 0 , 255 , 255} ;
        avail_Colors[8] = { 0 , 0 , 255} ;
        avail_Colors[9] = { 27 , 119 , 10} ;
        previousTool = 0 ;
        return 1;

}
void initBackground()
{
        /* Load image and create a texture from that image */
        SDL_Surface* back = IMG_Load("./res/grids.png");
        background = SDL_CreateTextureFromSurface(renderer , back);
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
        /* Load image and create a texture from that image */
        SDL_Surface *tool_surface  ;
        tool_surface = IMG_Load("./res/penciltool.png") ;
        tools[0] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/brush1.png") ;
        tools[1] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/brush2.png") ;
        tools[2] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/eraser.png") ;
        tools[3] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/bucket2.png") ;
        tools[4] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/exportPdf.png") ;
        tools[5] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

        tool_surface = IMG_Load("./res/clear.png") ;
        tools[6] = SDL_CreateTextureFromSurface(renderer , tool_surface) ;
        SDL_FreeSurface(tool_surface) ;

}
void handleColors(SDL_Event *event)
{
        int i  , x = 140 , y = 675 ;
        /* Handle Mouse Motion with different  xand y  poistions on screen i.e. handle that which color is SELECTED or NOT SELECTED */
        for ( i = 0 ; i <3 ; i++)
        {

                if(event->motion.x > x + i*100 && event->motion.x <  x + (i+1)*70+i*30  && event->motion.y > 675 && event->motion.y < 768)
                {
                        color_state[i] = 1 ;
                        int j ;
                        for( j = 0 ; j < 3 ; j++)
                                if(j!=i)
                                        color_state[j] = 0 ;
                        currentColor = i ;
                }
        }
        int j  ,  x1 = 240 ;
        for ( j = 3 ; j < 10 ; j++)
        {

                if(event->motion.x > x1 + j*100 && event->motion.x < x1 + (j+1)*75 + j*25 && event->motion.y > 675 && event->motion.y < 768)
                {
                        color_state[j] = 1 ;
                         int k ;
                        for( k = 0 ; k < 10 ; k++)
                                if(k!=j)
                                        color_state[k] = 0 ;
                        currentColor = j ;
                }

        }
}
void handleTools(SDL_Event *event)
{
        /* Handle Tools i.e. SELECTED or NOT SELECTED */
        int i ;
        for ( i = 0 ; i <7 ; i++)
        {
                if(event->motion.x > 10 && event->motion.x < 95 && event->motion.y > 30+i*100 && event->motion.y < 30 + (i+1) * 85 + i*15)
                {
                        tool_state[i]  = 1 ;
//                        if(currentTool<3)
//                        previousTool = currentTool ;
                        int j ;
                        for(j = 0 ; j <7 ; j++)
                                if(i!=j)
                                        tool_state[j] = 0 ;
                        currentTool = i ;
                }

        }
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
void putPixel_Screen(int x , int y , Uint32 c)
{
        if( x >= 0 && y>=0 && x< screenwidth-110 && y<screenheight-100)
                pixels[y*(screenwidth -110) + x] = c ;
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
        Uint32 color = pixels[y*(screenwidth -110) + x];
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
void do_flood_fill(int x, int y,int r1 , int g1 , int b1, int r2,int g2 , int b2)
{
//        static int count = 0 ;
//        if(count > 350 )
//                return ;
        int fillL, fillR, i, in_line;
        if (colors_close(r1,g1,b1, r2,g2,b2))
        return;


        fillL = x;
        fillR = x;

        in_line = 1;

        while (in_line)
        {
        putPixel(0 , fillL, y, r1,g1,b1);
        fillL--;
        int r , g , b;
        getPixel(fillL , y , &r ,&g , &b);
        in_line =
        (fillL < 0) ? 0 : colors_close(r,g,b,r2,g2,b2);
        }

        fillL++;

        /* Find right side, filling along the way */

        in_line = 1;
        while (in_line)
        {
        putPixel(0,fillR, y, r1,g1,b1);
        fillR++;
        int r , g , b;
        getPixel(fillR , y , &r ,&g , &b);
        in_line = (fillR >= screenwidth-110) ? 0 : colors_close(r,g,b,r2,g2,b2);
        }
        fillR--;

        for (i = fillL; i <= fillR; i++)
        {
        int r , g , b;
        if(y>0)
        getPixel(i , y-1 , &r ,&g , &b);

        if (y > 0 && colors_close(r,g,b,r2,g2,b2))
        {
        do_flood_fill(i, y - 1, r1 , g1 , b1 , r2,g2,b2);
        }
        if(y < screenheight-100 -1)
        getPixel(i , y+1 , &r ,&g , &b);

        if (y < screenheight-100 && colors_close(r,g,b,r2,g2,b2))
        {
        do_flood_fill(i, y + 1, r1 , g1 , b1 , r2,g2,b2);
        }
        }
//        count++ ;
}
void clearCanvas()
{
        memset(pixels, 255, (screenwidth-110)*(screenheight-100) * sizeof(Uint32));
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
        SDL_Rect s_pencil , d_pencil ;
        s_pencil.x = s_pencil.y = 0 ;
        int i ;
        if(currentTool == TOOL_PENCIL)
        {
                SDL_QueryTexture(canvas_tools[0] , 0 , 0 , &s_pencil.w , &s_pencil.h) ;
                d_pencil.w = s_pencil.w ;
                d_pencil.h = s_pencil.h ;
                d_pencil.x = xnew+110 ;
                d_pencil.y =  ynew -28;
                SDL_RenderCopy(renderer , canvas_tools[0] , &s_pencil , &d_pencil) ;
        }
        else if(currentTool == TOOL_BRUSH)
        {
                SDL_QueryTexture(canvas_tools[1] , 0 , 0 , &s_pencil.w , &s_pencil.h) ;
                d_pencil.w = s_pencil.w ;
                d_pencil.h = s_pencil.h ;
                d_pencil.x = xnew+110 ;
                d_pencil.y =  ynew -35;
                SDL_RenderCopy(renderer , canvas_tools[1] , &s_pencil , &d_pencil) ;
        }
        else if(currentTool == TOOL_BRUSH_THICK)
        {
                SDL_QueryTexture(canvas_tools[2] , 0 , 0 , &s_pencil.w , &s_pencil.h) ;
                d_pencil.w = s_pencil.w ;
                d_pencil.h = s_pencil.h ;
                d_pencil.x = xnew+98 ;
                d_pencil.y =  ynew -41;
                SDL_RenderCopy(renderer , canvas_tools[2] , &s_pencil , &d_pencil) ;
        }
        else if(currentTool == TOOL_BUCKET)
        {
                SDL_QueryTexture(canvas_tools[3] , 0 , 0 , &s_pencil.w , &s_pencil.h) ;
                d_pencil.w = s_pencil.w ;
                d_pencil.h = s_pencil.h ;
                d_pencil.x = xnew+106 ;
                d_pencil.y =  ynew -30;
                SDL_RenderCopy(renderer , canvas_tools[3] , &s_pencil , &d_pencil) ;
        }
}
void writeImage()
{
        char* buffer = (char*)malloc(sizeof(char)*(screenwidth-110)*(screenheight-100)*3);
        int i;
        int counter=0;
        for(i = 0 ; i <(screenwidth-110)*(screenheight-100); i++)
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
        png_set_IHDR(png_ptr , info_ptr , screenwidth-110 ,screenheight-100,
        8 , PNG_COLOR_TYPE_RGB , PNG_INTERLACE_NONE , PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE);

        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = "title";
        title_text.text = "hello";
        png_set_text(png_ptr , info_ptr , &title_text , 1);

        png_write_info(png_ptr , info_ptr);
        row = (png_bytep) malloc(3 * (screenwidth-110)* sizeof(png_byte));
        int x , y;
        for(y = 0 ; y<screenheight-100 ; y++)
        {
        for(x= 0 ; x<(screenwidth-110)*3 ; x++){
           row[x] = buffer[y*(screenwidth-110)*3 + x] ;
           }

        png_write_row(png_ptr, row);
        }

}
void handleEvent()
{
        SDL_Event event;
        SDL_PollEvent(&event);

        switch (event.type)
        {
                case SDL_KEYDOWN:
                {
                        if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                                quit = 1;
                }
                break ;
                case SDL_QUIT :
                        quit = 1 ;
                break ;
                case SDL_FINGERUP :
                case SDL_MOUSEBUTTONUP:
                                if (event.button.button == SDL_BUTTON_LEFT || event.type == SDL_FINGERUP )
                                        leftMouseButtonDown = 0;
                break;
                case SDL_FINGERDOWN :
                case SDL_MOUSEBUTTONDOWN :
               {
                        if(event.button.button == SDL_BUTTON_LEFT || event.type==SDL_FINGERDOWN)
                        {
                                leftMouseButtonDown = 1;

                                handleTools(&event) ;
                                handleColors(&event) ;
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
                                if(currentTool == TOOL_BUCKET && event.motion.x-110 > 0&& event.motion.y < screenheight-100)
                                {
                                        int r1 , g1  ,b1 , r2 , g2 , b2;
                                        r1 = avail_Colors[currentColor].r;
                                        g1 = avail_Colors[currentColor].g;
                                        b1 = avail_Colors[currentColor].b;
                                        getPixel(xnew  , ynew , &r2 , &g2 ,&b2);
                                        do_flood_fill(xnew , ynew , r1,g1,b1 , r2 , g2 , b2);
                                }
                                if(event.button.state == SDL_PRESSED && event.button.clicks == 1&&currentTool==TOOL_ERASER)
                                                        erase(xnew , ynew) ;
                        }
                }
                break ;

                case SDL_FINGERMOTION :
                case SDL_MOUSEMOTION :
                {
                        if(event.type ==SDL_MOUSEMOTION)
                        {
                                xnew = event.motion.x-110 ;
                                ynew = event.motion.y ;
                        }
                        else
                        {
                                xnew = event.tfinger.x ;
                                ynew = event.tfinger.y ;
                        }
                        if (leftMouseButtonDown)
                        {
                                if(currentTool == TOOL_PENCIL || currentTool == TOOL_BRUSH || currentTool == TOOL_BRUSH_THICK)
                                {
                                        SDL_ShowCursor(SDL_DISABLE) ;
                                        drawLine(xold , yold , xnew ,ynew) ;
                                }
                                if(currentTool == TOOL_ERASER)
                                {
                                        erase(xnew , ynew) ;
                                }
                        }
                        xold = xnew ;
                        yold = ynew ;
                }
                break;
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
        if(currentTool == TOOL_ERASER && xnew > 0 && ynew < 668 )
        {
                rectangleRGBA(renderer , xnew-25+110 , ynew-25 , xnew+25+110 , ynew+25 , 0 , 0 , 0 ,255) ;
                SDL_ShowCursor(SDL_DISABLE) ;
        }
        else
               SDL_ShowCursor(SDL_ENABLE) ;
        if(xnew>0 && ynew < 668)
        {
                renderTooI_icons() ;
                SDL_ShowCursor(SDL_DISABLE) ;
        }
        else
                SDL_ShowCursor(SDL_ENABLE) ;
        SDL_RenderPresent(renderer);

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
        free(pixels);
        SDL_DestroyTexture(Canvas);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
}
