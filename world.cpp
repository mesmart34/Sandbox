#include "world.h"

static int sign(float val)
{
    if(val < 0)
        return -1;
    return 1;
}

static float distance(float x1, float y1, float x2, float y2)
{
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void World::FillRandomly()
{
    srand(time(0));
    for(auto i = 0; i < w * h; i++)
    {
        type[i] = rand() % 2;
        vel_y[i] = rand() % 3;
    }
}

void World::DrawCircle(int x, int y)
{
    const float r = 10;
    for(auto _x = -r; _x <= r; _x++)
    {
        for(auto _y = -r; _y <= r; _y++)
        {
            /*float d = distance(x, x + _x, y, y + _y);
            if(d > 15)
                continue;*/
            if(_x + x >= 0 && _x + x < w && _y + y >= 0 && _y + y < h)
                Draw(_x + x, _y + y);
        }
    }
}

void World::Draw(int x, int y)
{
    auto index = x + y * w;
    if(type[index] != 0)
        return;
    type[index] = 1;
    vel_x[index] = 0;
    vel_y[index] = 0;
}

void World::SetCell(std::uint32_t last_index, std::uint32_t index)
{
    vel_x[index] = vel_x[last_index];
    vel_y[index] = vel_y[last_index];
    type[index] = type[last_index];
    type[index] = type[last_index];
}

void World::ZeroCell(std::uint32_t index)
{
    vel_x[index] = 0;
    vel_y[index] = 0;
    type[index] = 0;
    type[index] = 0;
}

void World::MoveCell(std::uint32_t x, std::uint32_t y)
{
    auto index = x + y * w;
    vel_y[index] += G;
    auto steps = (int)vel_y[index];
    auto dir = sign(vel_y[index]);
    auto i = 0;
    auto pos_y = y;
    while(i < steps)
    {
        pos_y += dir;
        if(pos_y == h)
            break;
        if(type[x + pos_y * w] != 0)
        {
            auto collision_index = x + pos_y * w;
            Collide(index, collision_index);
            break;
        }
        
        SetCell(index, x + pos_y * w);
        ZeroCell(index);
        scanned[x + pos_y * w] = 1;
        i++;
        
    }
}

void World::Collide(std::uint32_t index_a, std::uint32_t index_b)
{
    //X
    auto vel_x_a = vel_x[index_a];
    auto vel_x_b = vel_x[index_b];
    vel_x[index_a] = (vel_x_a + vel_x_b) / 2 * bounce;
    vel_x[index_b] = (vel_x_a + vel_x_b) / 2 * bounce;
    
    //Y
    auto vel_y_a = vel_y[index_a];
    auto vel_y_b = vel_y[index_b];
    vel_y[index_a] = (vel_y_a + vel_y_b) / 2 * bounce;
    vel_y[index_b] = (vel_y_a + vel_y_b) / 2 * bounce;
    
}

void World::Update()
{
    std::memset(scanned, 0, w * h);
    for(auto y = 0; y < h; y++)
    {
        for(auto x = 0; x < w; x++)
        {
            auto index = x + y * w;
            
            if(scanned[index] != 0)
                continue;
            if(type[index] == 0)
                continue;
            
            MoveCell(x, y);
            
            
        }
    }
}

void World::Render(SDL_Renderer* renderer)
{
    for(auto y = 0; y < h; y++)
    {
        for(auto x = 0; x < w; x++)
        {
            int scale = type[x + y * w] * 255;
            SDL_SetRenderDrawColor(renderer, scale, scale, scale, 255);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}