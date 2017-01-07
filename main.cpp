#include"SDL2/SDL.h"
#include"SDL2/SDL_image.h"
#include<stdio.h>
#include<string.h>
SDL_Window * window ;
SDL_Renderer *render ;
SDL_Rect sourceRectangle ;
SDL_Rect destinationRectangle ;
int running ;
enum  {
        UP  , DOWN
} ;
typedef struct color
{
        SDL_Surface * imgButtonUp ;
        SDL_Surface * imgButtonDown ;
        SDL_Texture * imgButton ;
        int state ;
}Color ;
Color colors[10] ;
void init_color()
{
        int i = 0 ;
        for( i = 0 ;i  <10 ; i++)
        {
                colors[i].state = DOWN ;
        }
}
void display_colors()
{
        int i = 0 ;
        for( i = 0 ; i < 10 ; i ++)
        {
                int x ;
                int y ;
                x= 0 ;
                y = i * 48 ;

        }
}
int init(const char* title , int xpos , int ypos , int width , int height , int flags)
{
        if(SDL_Init(SDL_INIT_EVERYTHING) >= 0)
        {
                //std::cout << "Initilization successful\n" ;
                window = SDL_CreateWindow(title, xpos , ypos , width , height ,flags);
                if(window!=0)
                {
                        //std::cout<<"window created successfully\n" ;
                        render = SDL_CreateRenderer(window , -1 , 0) ;
                        if(render!=0)
                        {
                                //std::cout <<"renderer creation successful\n" ;
                                SDL_SetRenderDrawColor(render , 60 , 60 ,60 ,255 ) ;
                        }
                        else
                        {
                                //std::cout << "render creation fails\n" ;
                                return 0;
                        }
                }
                else
                {
                        //std::cout<<"window creation fails\n" ;
                        return 0;
                }
        }
        else
        {
                //std::cout << "Initilization fails\n" ;
                return 0 ;
        }
        //std::cout<<"Initlization Successful\n" ;
        //m_bRunning = 1 ;
        //Color color_store[10] ;
        colors[0].imgButtonUp = IMG_Load("btn_down3.png") ;
        colors[0].imgButton = SDL_CreateTextureFromSurface(render ,colors[0].imgButtonUp) ;
        SDL_FreeSurface(colors[0].imgButtonUp) ;
        SDL_QueryTexture( colors[0].imgButton, NULL , NULL , &sourceRectangle.w , &sourceRectangle.h) ;
        destinationRectangle.h = sourceRectangle.h  ;
        destinationRectangle.w = sourceRectangle.w   ;
        destinationRectangle.x = sourceRectangle.x = 0 ;
        destinationRectangle.y = sourceRectangle.y   = 0;
         return 1 ;
}
void renderer()
{
        SDL_RenderClear(render) ;
//        SDL_RenderCopy(m_render , m_texture , &m_soureceRectangle ,&destinationRectangle ) ;
        SDL_RenderCopyEx(render ,colors[0].imgButton , &sourceRectangle , &destinationRectangle , 0 , 0 , SDL_FLIP_NONE) ;
        SDL_RenderPresent(render) ;

}
void handleEvent()
{
        SDL_Event  event ;
        if(SDL_PollEvent(&event))
        {
                switch(event.type)
                {
                        case SDL_QUIT :
                                running = 0 ;
                                break;
                        default :
                                break ;
                 }
         }
}
int main()
{
        running = init("Paint" , 0 , 0  , 1366 , 768 , 0);
        while(running)
        {
                handleEvent() ;
                renderer() ;
        }
        return 0 ;
}
