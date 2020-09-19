#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "SDL2/SDL.h"

#define internal static

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef float f32;

struct v2i
{
    i32 x, y;
};

struct v2
{
    f32 x, y;
};

struct particle
{
    i32 id;
    u32 color;
    v2 velocity;
    bool updated;
};



//SDL
static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Texture* screen_texture;

// Core settings
const static char* title = "Sandbox";
const static i32 width = 800 / 6;
const static i32 height = 600 / 6;
const static i32 window_width = 800;
const static i32 window_height = 600;

static bool interrupted = false;
static v2i mouse_coords = {};
static bool mlb; // Mouse left button
static bool mrb; // Mouse right button

// Sandbox

static particle *world_data;
static const f32 gravity = 9.81f;

enum particle_type : u8
{
    air = 0, 
    sand = 1,
    water = 2
};

particle empty_particle {0, 0, v2{0, 0}, false};
particle sand_particle { sand, 0xDE9B1FFF, v2{0, 0}, false };

// Functions declorations
internal void Init();
internal void Run();
internal void HandleEvents();
internal void Update();
internal void Render();

internal void UpdateSand(i32 x, i32 y);

internal inline u32 GetIndex(i32 x, i32 y);
internal inline particle* GetParticle(u32 index);
internal bool IsInBounds(i32 x, i32 y);

internal void WriteData(i32 index, particle prt);

internal void SetParticle(u32 x, u32 y, particle prt);
internal void EraseParticle(u32 x, u32 y);







internal void
Init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_RenderSetLogicalSize(renderer, width, height);
    
    mouse_coords = {0, 0};
    
    world_data = (particle*)malloc(sizeof(particle) * width * height);
    for(i32 i = 0; i < width * height; i++)
        world_data[i] = empty_particle;
    
    Run();
}

internal void
HandleEvents()
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
                        interrupted = true;
                    } break;
                }
            } break;
            case SDL_MOUSEMOTION:
            {
                mouse_coords.x = event.motion.x;
                mouse_coords.y = event.motion.y;
            } break;
            case SDL_MOUSEBUTTONDOWN:
            {
                if(event.button.button == SDL_BUTTON_LEFT)
                    mlb = true;
                if(event.button.button == SDL_BUTTON_RIGHT)
                    mrb = true;
            } break;
            case SDL_MOUSEBUTTONUP:
            {
                if(event.button.button == SDL_BUTTON_LEFT)
                    mlb = false;
                if(event.button.button == SDL_BUTTON_RIGHT)
                    mrb = false;
            } break;
            case SDL_KEYDOWN:
            {
                if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    interrupted = true;
            } break;
        }
        
    }
}


internal void 
Run()
{
    for(;!interrupted;)
    {
        HandleEvents();
        
        Update();
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        
        SDL_RenderClear(renderer);
        
        Render();
        
        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / 60);
    }
}

internal void 
Update()
{
    
    if(mlb)
    {
        SetParticle((u32)mouse_coords.x, (u32)mouse_coords.y, sand_particle);
    }
    if(mrb)
    {
        EraseParticle((u32)mouse_coords.x, (u32)mouse_coords.y);
    }
    
    for(u32 y = height - 1; y > 0; y--)
    {
        for(u32 x = 0; x < width; x++)
        {
            u32 index = GetIndex(x, y);
            particle* prt = GetParticle(index); 
            
            
            if(prt == NULL)
                continue;
            if(prt->updated)
                continue;
            
            
            switch(prt->id)
            {
                case(air): {  } break;
                case(sand): { UpdateSand(x, y); } break;
            };
            
            world_data[index].updated = true;
            
        }
    }
    
    for(u32 i = 0; i < width * height; i++)
        world_data[i].updated = false;
    
}

internal void
Render()
{
    u32 data[width * height] = {};
    for(i32 i = 0; i < width * height; i++)
    {
        particle* prt = GetParticle(i);
        if(prt->id == air)
            continue;
        data[i] = prt->color;
    }
    SDL_UpdateTexture(screen_texture, NULL, &data[0], sizeof(u32) * width);
    SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
}


internal inline u32 
GetIndex(i32 x, i32 y)
{
    return x + y * width;
}

internal inline particle*
GetParticle(u32 index)
{
    return &world_data[index];
}

internal void
WriteData(i32 index, particle prt)
{
    world_data[index] = prt;
}

internal bool
IsInBounds(i32 x, i32 y)
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

internal void
SetParticle(u32 x, u32 y, particle prt)
{
    if(!IsInBounds(x, y))
        return;
    u32 index = GetIndex(x, y);
    if(GetParticle(index)->id == air)
        WriteData(index, prt);
    
}

internal void
EraseParticle(u32 x, u32 y)
{
    if(!IsInBounds(x, y))
        return;
    u32 index = GetIndex(x, y);
    WriteData(index, empty_particle);
}

internal void
UpdateSand(i32 x, i32 y)
{
    i32 index = GetIndex(x, y);
    i32 b = GetIndex(x, y + 1);
    i32 br = GetIndex(x - 1, y + 1);
    i32 bl = GetIndex(x + 1, y + 1);
    
    particle prt = *GetParticle(b);
    particle prt_br = *GetParticle(br);
    particle prt_bl = *GetParticle(bl);
    
    if(prt.id == air && IsInBounds(x, y + 1))
    {
        WriteData(b, sand_particle);
        WriteData(index, empty_particle);
    }else if(prt_br.id == air && IsInBounds(x - 1, y + 1))
    {
        WriteData(br, sand_particle);
        WriteData(index, empty_particle);
    }else if(prt_bl.id == air && IsInBounds(x + 1, y + 1))
    {
        WriteData(bl, sand_particle);
        WriteData(index, empty_particle);
    }
    
}

int main(int argc, char** argv)
{
    Init();
    return 0;
}