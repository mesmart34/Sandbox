#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include "SDL2/SDL.h"

#define internal static
#define global static

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef float f32;
typedef double f64;

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
global SDL_Window* window;
global SDL_Renderer* renderer;
global SDL_Texture* screen_texture;

// Core settings
const global char* title = "Sandbox";
const global i32 width = 1920 / 6;
const global i32 height = 1080 / 6;
const global i32 window_width = 1920 / 2;
const global i32 window_height = 1080 / 2;
const global f32 ms_per_update = 1000.0f / 60.0f;

global bool interrupted = false;

global struct mouse_t
{
    v2i pos;
    bool left, right, middle;
} mouse;


// Sandbox

global particle *world_data;
global const f32 gravity = 9.81f;
global const i32 radius = 25;

enum particle_type : u8
{
    air = 0, 
    sand = 1,
    water = 2,
    wood = 3
};

global const u32 water_color = 0x4291FFFF;
global const u32 sand_color = 0xDE9B1FFF;
global const u32 wood_color = 0x4f3818FF;

particle empty_particle {0, 0, v2{0, 0}, false};
particle sand_particle { sand, sand_color, v2{0, 0}, false };
particle water_particle { water, water_color, v2{0, 0}, false };
particle wood_particle { wood, wood_color, v2{0, 0}, false };

// Functions declorations
internal void Init();
internal void Run();
internal void HandleEvents();
internal void Update(f32 delta_time);
internal void Render();

internal void UpdateSand(i32 x, i32 y);
internal void UpdateWater(i32 x, i32 y);

internal inline u32 GetIndex(i32 x, i32 y);
internal inline particle* GetParticle(u32 index);
internal bool IsInBounds(i32 x, i32 y);

internal void WriteData(i32 index, particle prt);

internal void SetParticle(i32 x, i32 y, particle prt);
internal void EraseParticle(i32 x, i32 y);
internal inline void MoveParticle(u32 from, u32 to, particle prt);
internal void DrawCircle(i32 x, i32 y, i32 r, particle prt);
internal void EraseCirlce(i32 x, i32 y, i32 r);

internal f32 Distance(v2 a, v2 b);
internal inline i8 Sign(f32 value);


internal void
Init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_RenderSetLogicalSize(renderer, width, height);
    
    srand(time(0));
    
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
                mouse.pos.x = event.motion.x;
                mouse.pos.y = event.motion.y;
            } break;
            case SDL_MOUSEBUTTONDOWN:
            {
                if(event.button.button == SDL_BUTTON_LEFT)
                    mouse.left = true;
                if(event.button.button == SDL_BUTTON_RIGHT)
                    mouse.right = true;
                if(event.button.button == SDL_BUTTON_MIDDLE)
                    mouse.middle = true;
            } break;
            case SDL_MOUSEBUTTONUP:
            {
                if(event.button.button == SDL_BUTTON_LEFT)
                    mouse.left = false;
                if(event.button.button == SDL_BUTTON_RIGHT)
                    mouse.right = false;
                if(event.button.button == SDL_BUTTON_MIDDLE)
                    mouse.middle = false;
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
    f32 prev = SDL_GetTicks();
    f32 lag = 0.0f;
    f32 delta_time = 0.0f;
    for(;!interrupted;)
    {
        f32 current = SDL_GetTicks();
        delta_time = (f32)(current - prev);
        prev = current;
        lag += delta_time;
        
        //delta_time = (float)((now - last) * 1000 / (float)SDL_GetPerormanceFrequency());
        
        
        
        HandleEvents();
        
        while(lag >= ms_per_update)
        {
            Update(delta_time * 0.001f);
            lag -= ms_per_update;
        }
        
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        
        SDL_RenderClear(renderer);
        
        Render();
        
        SDL_RenderPresent(renderer);
    }
}

internal void 
Update(f32 delta_time)
{
    //printf("%f\n", delta_time);
    if(mouse.left)
    {
        //DrawCircle((u32)mouse.pos.x, (u32)mouse.pos.y, radius, wood_particle);
        SetParticle((u32)mouse.pos.x, (u32)mouse.pos.y, sand_particle);
    }
    if(mouse.right)
    {
        DrawCircle((u32)mouse.pos.x, (u32)mouse.pos.y, radius, sand_particle);
    }
    if(mouse.middle)
    {
        EraseCirlce((u32)mouse.pos.x, (u32)mouse.pos.y, radius);
    }
    
    for(i32 y = height - 2; y >= 0; y--)
    {
        for(i32 x = 0; x < width; x++)
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
                case(water): { UpdateWater(x, y); } break;
            };
            
            
            
        }
    }
    
    for(i32 i = 0; i < width * height; i++)
        GetParticle(i)->updated = false;
    
}


#if 1
internal void
Render()
{
    u32 data[width * height] = {};
    for(i32 i = 0; i < width * height; i++)
    {
        particle prt = *GetParticle(i);
        if(prt.id == air)
            continue;
        data[i] = prt.color;
    }
    SDL_UpdateTexture(screen_texture, NULL, &data[0], sizeof(u32) * width);
    SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
}
#else
internal void
Render()
{
    for(i32 i = 0; i < width * height; i++)
    {
        particle* prt = GetParticle(i);
        if(prt->id == air)
            continue;
        i32 x = i % width;
        i32 y = i / width;
        u8 r = prt->color >> 24;
        u8 g = prt->color >> 16;
        u8 b = prt->color >> 8;
        u8 a = prt->color;
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderDrawPoint(renderer, x, y);
    }
}
#endif


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
SetParticle(i32 x, i32 y, particle prt)
{
    if(!IsInBounds(x, y))
        return;
    u32 index = GetIndex(x, y);
    if(GetParticle(index)->id == air)
        WriteData(index, prt);
    
}

internal void
EraseParticle(i32 x, i32 y)
{
    if(!IsInBounds(x, y))
        return;
    u32 index = GetIndex(x, y);
    WriteData(index, empty_particle);
}

internal inline i8
Sign(f32 value)
{
    if(value < 0.0f)
        return -1;
    return 1;
}

internal f32
Distance(v2 a, v2 b)
{
    return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

internal void
EraseCirlce(i32 x, i32 y, i32 r)
{
    for(i32 _x = x - r; _x < x + r; _x++)
    {
        for(i32 _y = y - r; _y < y + r; _y++)
        {
            f32 d = Distance(v2{x, y}, v2{_x, _y});
            if(d >= r * 0.75)
                continue;
            EraseParticle(_x, _y);
        }
    }
}

internal void
DrawCircle(i32 x, i32 y, i32 r, particle prt)
{
    for(i32 _x = x - r; _x < x + r; _x++)
    {
        for(i32 _y = y - r; _y < y + r; _y++)
        {
            f32 d = Distance(v2{x, y}, v2{_x, _y});
            if(d >= r * 0.75)
                continue;
            SetParticle(_x, _y, prt);
        }
    }
}

internal inline void
MoveParticle(u32 from, u32 to, particle prt)
{
    WriteData(to, prt);
    WriteData(from, empty_particle);
}

internal void
UpdateWater(i32 x, i32 y)
{
    u32 index = GetIndex(x, y);
    
    
    
    u32 b = GetIndex(x, y + 1);
    u32 br = GetIndex(x + 1, y);
    u32 bl = GetIndex(x - 1, y);
    
    
    particle* prt = GetParticle(b);
    particle* prt_br = GetParticle(br);
    particle* prt_bl = GetParticle(bl);
    
    if(prt->id == air && IsInBounds(x, y + 1))
    {
        MoveParticle(index, b, water_particle);
        return;
    }
    i32 side = rand() % 2;
    side = 0;
    if(side == 0)
    {
        if(prt_bl->id == air && IsInBounds(x - 1, y))
        {
            MoveParticle(index, bl, water_particle);
        }
        else if(prt_br->id == air && IsInBounds(x + 1, y))
        {
            MoveParticle(index, br, water_particle);
        }
    }else{
        if(prt_br->id == air && IsInBounds(x + 1, y))
        {
            MoveParticle(index, br, water_particle);
        } else if(prt_bl->id == air && IsInBounds(x - 1, y))
        {
            MoveParticle(index, bl, water_particle);
        }
        
    }
}

internal void
UpdateSand(i32 x, i32 y)
{
    i32 index = GetIndex(x, y);
    i32 b = GetIndex(x, y + 1);
    i32 br = GetIndex(x + 1, y + 1);
    i32 bl = GetIndex(x - 1, y + 1);
    
    particle* prt = GetParticle(b);
    particle* prt_br = GetParticle(br);
    particle* prt_bl = GetParticle(bl);
    
    if(prt->id == air && IsInBounds(x, y + 1))
    {
        MoveParticle(index, b, sand_particle);
        GetParticle(b)->updated = true;
    } else if(prt_br->id == air && IsInBounds(x + 1, y + 1))
    {
        MoveParticle(index, br, sand_particle);
        GetParticle(br)->updated = true;
    } else if(prt_bl->id == air && IsInBounds(x - 1, y + 1))
    {
        MoveParticle(index, bl, sand_particle);
        GetParticle(bl)->updated = true;
    }
}

int main(int argc, char** argv)
{
    Init();
    return 0;
}