#include <iostream>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <cstdint>
#include <cmath>

struct Particle
{
    float weight;
};

struct v2i
{
    std::uint32_t x, y;
};

class Sandbox
{
    public:
    Sandbox() = default;
    Sandbox(int w, int h, bool fs) : width(w), height(h) {};
    
    
    void Init()
    {
        if(SDL_Init(SDL_INIT_EVERYTHING < 0))
        {
            std::cout << SDL_GetError() << std::endl;
        }
        window = SDL_CreateWindow("Sandbox", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
        SDL_RenderSetLogicalSize(renderer, width, height);
        if(window == nullptr || renderer == nullptr)
            std::cout << SDL_GetError() << std::endl;
        else{
            std::cout << "SDL is initialized succesfully!" << std::endl;
            running = true;
        }
        mouseCoords = {0, 0};
        //world = std::uint32_t[width * height];
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
        auto PutPixel = [&](std::uint32_t pixels[], const std::uint32_t x, const std::uint32_t y, const std::uint32_t color)
        {
            pixels[x + y * width] = color;
        };
        for(;running;)
        {
            HandleEvents();
            std::uint32_t pixels[width * height];
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            
            SDL_RenderClear(renderer);
            
            for(std::uint32_t x = 0; x < width; x++)
            {
                for(std::uint32_t y = 0; y < height; y++)
                {
                    PutPixel(pixels, x, y, 0x000000);
                }
            }
            
            const auto range = 30;
            for(int i = -range; i <= range; i++)
            {
                for(int j = -range; j <= range; j++)
                {
                    auto x = mouseCoords.x + i;
                    auto y = mouseCoords.y + j;
                    auto d = std::sqrt(std::pow(std::max(x, mouseCoords.x) - std::min(x, mouseCoords.x), 2) + std::pow(std::max(y, mouseCoords.y) - std::min(y, mouseCoords.y), 2));
                    if(x >= 0 && x < width && y >= 0 && y < height && d < range * 0.75f)
                        PutPixel(world, x, y, 0x252FFF);
                }
            }
            
            
            
            
            for(std::uint32_t x = 0; x < width; x++)
            {
                for(std::uint32_t y = 0; y < height; y++)
                {
                    if(world[x + y * width] != 0)
                        PutPixel(pixels, x, y, 0x124245);
                }
            }
            
            
            SDL_UpdateTexture(screenTexture, 0, &pixels, 4 * width);
            SDL_RenderCopy(renderer, screenTexture, 0, 0);
            SDL_RenderPresent(renderer);
        }
    }
    
    std::uint32_t width, height;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* screenTexture;
    bool running;
    v2i mouseCoords;
    std::uint32_t world[800 * 600];
};

int SDL_main(int argc, char** argv)
{
    auto sandbox = new Sandbox(800, 600, false);
    sandbox->Init();
    return 0;
}