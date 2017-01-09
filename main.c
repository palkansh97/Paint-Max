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
        SDL_Surface * imgButtonUp_surface ;
        SDL_Surface * imgButtonDown_surface ;
        SDL_Texture * imgButtonDown_texture ;
        SDL_Texture * imgButtonUp_texture ;
        int state ;
}Color ;
Color colors[10] ;
void handleColors(SDL_Event * event)
{
        int i ;
        for(i = 0 ; i < 6 ; i++)
        {
//                if(event->motion.y < (i+1) * 48 && event->motion.y > i*48 && event->motion.x < 48)
                if(i%2==0)
                {
                        if(event->motion.y < (i+2)/2 * 48 && event->motion.y > (i/2) * 48 && event->motion.x < 48)
                                colors[i].state = DOWN ;
                        else
                                colors[i].state = UP ;
                }
                if(i%2 == 1)
                {
                        if(event->motion.x > 48 && event->motion.x < 96 && event->motion.y < (i+1)/2 *48 && event->motion.y > (i/2)*48)
                                colors[i].state = DOWN ;
                        else
                                colors[i].state = UP ;
                }
        }
}
int init(const char* title , int xpos , int ypos , int width , int height , int flags)
{
        if(SDL_Init(SDL_INIT_EVERYTHING) >= 0)
        {
//                printf("Initilization successful\n") ;
                window = SDL_CreateWindow(title, xpos , ypos , width , height ,flags);
                if(window!=0)
                {
//                        printf("window created successfully\n") ;
                        render = SDL_CreateRenderer(window , -1 , 0) ;
                        if(render!=0)
                        {
//                                printf("renderer creation successful\n") ;
                                SDL_SetRenderDrawColor(render , 255 , 255 ,255 ,255 ) ;
                        }
                        else
                        {
//                                printf("render creation fails\n" );
                                return 0;
                        }
                }
                else
                {
//                        printf("window creation fails\n") ;
                        return 0;
                }
        }
        else
        {
//                printf("Initilization fails\n") ;
                return 0 ;
        }
//        printf("Initlization Successful\n") ;
        colors[0].imgButtonDown_surface = IMG_Load("res/btn_down1.png") ;
        colors[1].imgButtonDown_surface = IMG_Load("res/btn_down2.png") ;
        colors[2].imgButtonDown_surface = IMG_Load("res/btn_down3.png") ;
        colors[3].imgButtonDown_surface = IMG_Load("res/btn_down4.png") ;
        colors[4].imgButtonDown_surface = IMG_Load("res/btn_down5.png") ;
        colors[5].imgButtonDown_surface = IMG_Load("res/btn_down6.png") ;

        colors[0].imgButtonUp_surface = IMG_Load("res/btn_off1.png") ;
        colors[1].imgButtonUp_surface = IMG_Load("res/btn_off2.png") ;
        colors[2].imgButtonUp_surface = IMG_Load("res/btn_off3.png") ;
        colors[3].imgButtonUp_surface = IMG_Load("res/btn_off4.png") ;
        colors[4].imgButtonUp_surface = IMG_Load("res/btn_off5.png") ;
        colors[5].imgButtonUp_surface = IMG_Load("res/btn_off6.png") ;
        int i ;
        for ( i = 0 ; i < 6 ; i++)
        {
                colors[i].imgButtonDown_texture= SDL_CreateTextureFromSurface(render ,colors[i].imgButtonDown_surface) ;
                SDL_FreeSurface(colors[i].imgButtonDown_surface) ;
                SDL_QueryTexture( colors[i].imgButtonDown_texture, NULL , NULL , &sourceRectangle.w , &sourceRectangle.h) ;
                colors[i].imgButtonUp_texture = SDL_CreateTextureFromSurface(render , colors[i].imgButtonUp_surface);
                SDL_FreeSurface(colors[i].imgButtonUp_surface);
                SDL_QueryTexture(colors[i].imgButtonUp_texture , NULL , NULL , &sourceRectangle.w , &sourceRectangle.h) ;

        }
         return 1 ;
}
void renderer()
{

//        SDL_RenderCopy(m_render , m_texture , &m_soureceRectangle ,&destinationRectangle ) ;
        SDL_RenderClear(render) ;
        int j = 0 , i ;
        for ( i = 0 ; i <6 ; i++)
        {
                SDL_Rect dest_off = sourceRectangle ;
                dest_off.x = (i % 2) * 48 ;
                dest_off.y = j * 48 ;
//                dest_off.x = 0 ;
//                dest_off.y = i* 48 ;
                if(colors[i].state == UP)
                        SDL_RenderCopyEx(render ,colors[i].imgButtonUp_texture , &sourceRectangle , &dest_off, 0 , 0 , SDL_FLIP_NONE) ;
                else
                        SDL_RenderCopyEx(render ,colors[i].imgButtonDown_texture , &sourceRectangle , &dest_off, 0 , 0 , SDL_FLIP_NONE) ;
                if(i%2)
                        j++ ;
        }
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
                        case SDL_MOUSEBUTTONDOWN :
                        {
                                if ( event.button.button == SDL_BUTTON_LEFT)
                                        handleColors(&event) ;
                        }
                                break ;
                        default :
                                break ;
                 }
         }
}
void clean()
{
//        printf("cleaning done\n");
//        SDL_Delay(5000) ;
        SDL_DestroyRenderer(render) ;
        SDL_DestroyWindow(window) ;
        SDL_Quit() ;
}
int main()
{
        running = init("Paint" , 0 , 0  , 1366 , 768 , 0);
        while(running)
        {
                handleEvent() ;
                renderer() ;
        }
        clean();
        return 0 ;
}
