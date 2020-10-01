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
global const f32 gravity = 3.81f;
global f32 radius = 15;
global i32 frames = 0;
global f32 delta_time = 1.0;

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
internal void Update();
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
internal void DrawLine(u32 *data, v2i a,  v2i b, u32 color);

internal void RenderDrawUnfilledCircle(u32* data, i32 centreX, i32 centreY, i32 radius, u32 color);
internal inline bool IsAir(i32 x, i32 y);
internal inline bool IsCompletelySurrounded(i32 x, i32 y);
internal inline  void SetPixel(u32* data, i32 x, i32 y, u32 color);
internal float Clamp(f32 target, f32 min, f32 max);
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
            case SDL_MOUSEWHEEL:
            {
                if(event.wheel.y > 0) 
                {
                    radius += 1.0f;
                }
                else if(event.wheel.y < 0) 
                {
                    radius -= 1.0f;
                }
                radius = Clamp(radius, 5, 25);
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
    f32 dt = 0.0f;
    for(;!interrupted;)
    {
        f32 current = SDL_GetTicks();
        dt = (f32)(current - prev);
        prev = current;
        lag += dt;
        
        //delta_time = (float)((now - last) * 1000 / (float)SDL_GetPerormanceFrequency());
        
        
        
        HandleEvents();
        
        while(lag >= ms_per_update)
        {
            Update();
            delta_time = dt * 0.5;
            lag -= ms_per_update;
        }
        
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        
        SDL_RenderClear(renderer);
        
        Render();
        
        SDL_RenderPresent(renderer);
    }
}

internal void 
Update()
{
    //printf("%f\n", delta_time);
    if(mouse.left)
    {
        DrawCircle((u32)mouse.pos.x, (u32)mouse.pos.y, radius, wood_particle);
    }
    if(mouse.right)
    {
        DrawCircle((u32)mouse.pos.x, (u32)mouse.pos.y, radius, sand_particle);
    }
    if(mouse.middle)
    {
        EraseCirlce((u32)mouse.pos.x, (u32)mouse.pos.y, radius);
    }
    
    frames++;
    
    i32 ran = frames % 2;
    //ran = 0;
    if(frames > 10)
        frames -= 10;
    
    for(i32 y = 1 ? height - 2 : 0; 1 ? y >= 0 : y < height - 2; 1 ?  y-- : y++)
    {
        for(i32 x = ran ? width - 1 : 0; ran ? x >= 0 : x < width; ran ? x-- : x++)
        {
            u32 index = GetIndex(x, y);
            particle* prt = GetParticle(index); 
            
            
            if(prt == NULL)
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

internal inline  void SetPixel(u32* data, i32 x, i32 y, u32 color)
{
    if(!IsInBounds(x, y))
        return;
    *(data + x + y * width) = color;
}

internal void RenderDrawUnfilledCircle(u32* data, i32 centreX, i32 centreY, i32 radius, u32 color)
{
    const i32 diameter = ((radius * 2));
    
    i32 x = (radius - 1);
    i32 y = 0;
    i32 tx = 1;
    i32 ty = 1;
    i32 error = (tx - diameter);
    
    while (x >= y)
    {
        SetPixel(data, (centreX + x), (centreY - y), color);
        SetPixel(data, (centreX + x), (centreY + y), color);
        SetPixel(data, (centreX - x), (centreY - y), color);
        SetPixel(data, (centreX - x), (centreY + y), color);
        SetPixel(data, (centreX + y), (centreY - x), color);
        SetPixel(data, (centreX + y), (centreY + x), color);
        SetPixel(data, (centreX - y), (centreY - x), color);
        SetPixel(data, (centreX - y), (centreY + x), color);
        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }
        
        if (error > 0)
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

internal void DrawLine(u32 *data, v2i a,  v2i b, u32 color)
{
    i32 dx, dy, p, x, y;
    
	dx = b.x - a.x;
	dy = b.y - a.y;
    
	x = a.x;
	y = a.y;
    
	p = 2 * dy-dx;
    
	while( x < b.x )
	{
		if(p>=0)
		{
            SetPixel(data, x, y, color);
			y=y+1;
			p=p+2*dy-2*dx;
		}
		else
		{
            SetPixel(data, x, y, color);
			p=p+2*dy;
		}
		x=x+1;
	}
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
    
    RenderDrawUnfilledCircle(&data[0], mouse.pos.x, mouse.pos.y, radius * 0.85, 0x2F50F6FF);
    
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
    //world_data[index].updated = true;
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
    //GetParticle(index)->updated = true;
    
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
            i32 s = rand() % 10;
            if(d >= r * 0.75)
                continue;
            if(s % 9 == 0)
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

internal inline bool IsAir(i32 x, i32 y)
{
    return IsInBounds(x, y) &&  GetParticle(GetIndex(x, y))->id == air;
}

internal float Clamp(f32 target, f32 min, f32 max)
{
    if(min > target)
        return min;
    if(max < target)
        return max;
    return target;
}

internal inline bool
IsCompletelySurrounded(i32 x, i32 y)
{
    return  
        IsAir(x + 1, y + 1)
        && IsAir(x + 1, y - 1)
        && IsAir(x - 1, y + 1)
        && IsAir(x - 1, y - 1)
        && IsAir(x + 1, y)
        && IsAir(x - 1, y)
        && IsAir(x, y + 1)
        && IsAir(x, y - 1);
}

internal void
UpdateWater(i32 x, i32 y)
{
    i32 index = GetIndex(x, y);
    
    //if(IsCompletelySurrounded(x, y))
    //return;
    
    i32 b = GetIndex(x, y + 1);
    i32 br = GetIndex(x + 1, y + 1);
    i32 bl = GetIndex(x - 1, y + 1);
    i32 l = GetIndex(x - 1, y);
    i32 r = GetIndex(x + 1, y);
    
    
    
    particle origin = *GetParticle(index);
    
    GetParticle(index)->updated = true;
    
    particle prt = *GetParticle(b);
    particle prt_br = *GetParticle(br);
    particle prt_bl = *GetParticle(bl);
    
    particle prt_l = *GetParticle(l);
    particle prt_r = *GetParticle(r);
    
    if(prt.id == air && IsInBounds(x, y + 1))
    {
        WriteData(b, origin);
        WriteData(index, empty_particle);
    } else if(prt_br.id == air && IsInBounds(x + 1, y + 1))
    {
        WriteData(br, origin);
        WriteData(index, empty_particle);
    }else if(prt_bl.id == air && IsInBounds(x - 1, y + 1))
    {
        WriteData(bl, origin);
        WriteData(index, empty_particle);
    }else if(prt_l.id == air && IsInBounds(x - 1, y))
    {
        WriteData(l, origin);
        WriteData(index, empty_particle);
    }else if(prt_r.id == air && IsInBounds(x + 1, y))
    {
        WriteData(r, origin);
        WriteData(index, empty_particle);
    }
}

#if 1


internal void
UpdateSand(i32 x, i32 y)
{
    
    i32 index = GetIndex(x, y);
    
    
    
    particle* p = GetParticle(index);
    particle tmp_a  = *GetParticle(index);
    
    p->velocity.y = Clamp(p->velocity.y + (gravity), -8, 8);
    i32 vi_x = p->velocity.x + x;
    i32 vi_y = p->velocity.y + y;
    i32 dir_y = Sign(vi_y);
    
    i32 pos_y = y;
    
    
    while(pos_y != vi_y)
    {
        pos_y += dir_y;
        
        if(!IsInBounds(x, pos_y))
            break;
        if(GetParticle(GetIndex(x, pos_y))->updated)
            break;
        
        i32 b = GetIndex(x, pos_y);
        i32 br = GetIndex(x + 1, pos_y);
        i32 bl = GetIndex(x - 1, pos_y);
        particle prt = *GetParticle(b);
        particle prt_br = *GetParticle(br);
        particle prt_bl = *GetParticle(bl);
        
        //if(GetParticle(GetIndex(vi_x, vi_y))->updated)
        //break;
        
        if(IsAir(vi_x, vi_y))
        {
            particle tmp_b = *GetParticle(GetIndex(vi_x, vi_y));
            WriteData(GetIndex(vi_x, vi_y), tmp_a);
            WriteData(index, tmp_b);
        }else
            
            if((prt.id == air) && IsInBounds(x, pos_y))
        {
            particle tmp_b = *GetParticle(b);
            p->velocity.y = 0;
            WriteData(b, *p);
            WriteData(index, tmp_b);
            break;
        } else if((prt_br.id == air) && IsInBounds(x + 1, pos_y))
        {
            particle tmp_b = *GetParticle(br);
            p->velocity.y = 0;
            WriteData(br, *p);
            WriteData(index, tmp_b);
        }else if((prt_bl.id == air) && IsInBounds(x - 1, pos_y))
        {
            particle tmp_b = *GetParticle(bl);
            p->velocity.y = 0;
            WriteData(bl, *p);
            WriteData(index, tmp_b);
        }
    }
}

#else

internal void
UpdateSand(i32 x, i32 y)
{
    i32 index = GetIndex(x, y);
    i32 b = GetIndex(x, y + 1);
    i32 br = GetIndex(x + 1, y + 1);
    i32 bl = GetIndex(x - 1, y + 1);
    
    
    particle* p = GetParticle(index);
    particle tmp_a  = *GetParticle(index);
    
    
    
    particle prt = *GetParticle(b);
    particle prt_br = *GetParticle(br);
    particle prt_bl = *GetParticle(bl);
    
    
    if((prt.id == air) && IsInBounds(x, y + 1))
    {
        particle tmp_b = *GetParticle(b);
        WriteData(b, *p);
        WriteData(index, tmp_b);
    } else if((prt_br.id == air) && IsInBounds(x + 1, y + 1))
    {
        particle tmp_b = *GetParticle(br);
        WriteData(br, *p);
        WriteData(index, tmp_b);
    }else if((prt_bl.id == air) && IsInBounds(x - 1, y + 1))
    {
        particle tmp_b = *GetParticle(bl);
        WriteData(bl, *p);
        WriteData(index, tmp_b);
    }
}

#endif


int main(int argc, char** argv)
{
    Init();
    return 0;
}