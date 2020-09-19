#include <iostream>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <cstdint>
#include <cmath>
#include "world.h"

struct v2i
{
    std::int32_t x, y;
};

class Sandbox
{
    public:
    Sandbox() = default;
    Sandbox(int w, int h, int r_w, int r_h, bool fs) : width(w), height(h), r_width(r_w), r_height(r_h) {};
    
    
    void Init()
    {
        if(SDL_Init(SDL_INIT_EVERYTHING < 0))
        {
            std::cout << SDL_GetError() << std::endl;
        }
        window = SDL_CreateWindow("Sandbox", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_RenderSetLogicalSize(renderer, r_width, r_height);
        if(window == nullptr || renderer == nullptr)
            std::cout << SDL_GetError() << std::endl;
        else{
            std::cout << "SDL is initialized succesfully!" << std::endl;
            running = true;
        }
        mouseCoords = {0, 0};
        world = World(r_width, r_height);
        Run();
    } 
    private:
    
    
    void HandleEvents()
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_WINDOWEVENT:
                {
                    switch(event.window.event)
                    {
                        case SDL_WINDOWEVENT_CLOSE:
                        {
                            running = false;
                        } break;
                    }
                } break;
                case SDL_MOUSEMOTION:
                {
                    mouseCoords.x = event.motion.x;
                    mouseCoords.y = event.motion.y;
                } break;
                case SDL_MOUSEBUTTONDOWN:
                {
                    if(event.button.button == SDL_BUTTON_LEFT)
                        mouse_pressed_left = true;
                    if(event.button.button == SDL_BUTTON_RIGHT)
                        mouse_pressed_right = true;
                } break;
                case SDL_MOUSEBUTTONUP:
                {
                    if(event.button.button == SDL_BUTTON_LEFT)
                        mouse_pressed_left = false;
                    if(event.button.button == SDL_BUTTON_RIGHT)
                        mouse_pressed_right = false;
                } break;
                case SDL_KEYDOWN:
                {
                    if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                        running = false;
                } break;
            }
        }
    }
    
    void Run()
    {
        for(;running;)
        {
            HandleEvents();
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            
            if(mouse_pressed_left)
                world.DrawCircle(mouseCoords.x, mouseCoords.y, 5, SAND
                                 );
            if(mouse_pressed_right)
                world.DrawCircle(mouseCoords.x, mouseCoords.y, 5, WATER);
            world.Update();
            
            SDL_RenderClear(renderer);
            
            world.Render(renderer);
            
            SDL_RenderPresent(renderer);
            SDL_Delay(1000 / 60);
        }
    }
    
    std::uint32_t width, height;
    std::uint32_t r_width, r_height;
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool mouse_pressed_left;
    bool mouse_pressed_right;
    v2i mouseCoords;
    World world;
};

int SDL_main(int argc, char** argv)
{
    auto sandbox = new Sandbox(800, 600, 100 *2, 75* 2, false);
    sandbox->Init();
    return 0;
}