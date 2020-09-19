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

void World::SwapCells(std::int32_t a, std::int32_t b)
{
    std::swap(type[a], type[b]);
    std::swap(vel_x[a], vel_x[b]);
    std::swap(vel_y[a], vel_y[b]);
}

void World::FillRandomly()
{
    srand(time(0));
    for(auto i = (int)(w * h * 0.5); i < w * h; i++)
    {
        type[i] = WATER;
        vel_y[i] = rand() % 3;
    }
}

void World::EraseCircle(int x, int y, int r)
{
    for(auto _x = -r; _x <= r; _x++)
    {
        for(auto _y = -r; _y <= r; _y++)
        {
            auto new_x = _x + x;
            auto new_y = _y + y;
            float d = distance(x, y, new_x, new_y);
            if(d > r * 0.75)
                continue;
            Erase(new_x, new_y);
        }
    }
}

void World::Erase(int x, int y)
{
    if(x >= 0 && x < w && y >= 0 && y < h)
        ZeroCell(x + y * w);
}

void World::DrawCircle(int x, int y, float r, std::uint8_t type)
{
    for(auto _x = -r; _x <= r; _x++)
    {
        for(auto _y = -r; _y <= r; _y++)
        {
            auto new_x = _x + x;
            auto new_y = _y + y;
            float d = distance(x, y, new_x, new_y);
            if(d > r * 0.75)
                continue;
            Draw(new_x, new_y, type);
        }
    }
}

void World::Draw(int x, int y, std::uint8_t _type)
{
    if(!(x >= 0 && x < w && y >= 0 && y < h))
        return;
    auto index = x + y * w;
    if(type[index] != AIR)
        return;
    type[index] = _type;
    vel_x[index] = 0;
    vel_y[index] = 0;
}

void World::SetCell(std::int32_t last_index, std::int32_t index)
{
    vel_x[index] = vel_x[last_index];
    vel_y[index] = vel_y[last_index];
    type[index] = type[last_index];
    type[index] = type[last_index];
}

void World::ZeroCell(std::int32_t index)
{
    vel_x[index] = 0;
    vel_y[index] = 0;
    type[index] = 0;
    type[index] = 0;
}

void World::ProcessCell(std::int32_t x, std::int32_t y)
{
    auto index = x + y * w;
    acc_x[index] = 0;
    acc_y[index] = G;
    vel_x[index] += acc_x[index];
    vel_y[index] += acc_y[index];
    auto steps_x = (int)vel_x[index];
    auto steps_y = (int)vel_y[index];
    auto dir_x = sign(vel_x[index]);
    auto dir_y = sign(vel_y[index]);
    auto i = 0;
    auto pos_y = y;
    while(i < steps_y)
    {
        pos_y += dir_y;
        if(pos_y == h || pos_y == 0)
        {
            vel_y[index] = 0;
            //acc_y[index] = 0;
            break;
        }
        
        auto next = x + pos_y * w;
        
        switch(type[index])
        {
            case SAND: { //UpdateSand(x, pos_y);
            } break;
            case WATER: { UpdateWater(x, pos_y); } break;
            i = steps_y;
        };
        
        i++;
    }
    scanned[x + pos_y * w] = 1;
}

void World::UpdateSand(std::int32_t x, std::int32_t y)
{
    //std::cout << vel_y[x +(y - 1) * w] << std::endl;
    auto bottom = x + y * w;           //BOTTOM
    auto bottom_left = x - 1 + y * w;  //BOTTOM LEFT
    auto bottom_right = x + 1 + y * w; //BOTTOM RIGHT
    if(GetType(bottom) == AIR)
    {
        MoveCell(x + (y - 1) * w, bottom);
    } else if(GetType(bottom_left) == AIR && x - 1 >= 0)
    {
        MoveCell(x + (y - 1) * w, bottom_left);
    } else if(GetType(bottom_right) == AIR && x + 1 < w)
    {
        MoveCell(x + (y - 1) * w, bottom_right);
    }
}

void World::UpdateWater(std::int32_t x, std::int32_t y)
{
    auto start = x + (y - 1) * w;
    auto bottom = x + y * w;           //BOTTOM
    auto bottom_left = x - 1 + y * w;  //BOTTOM LEFT
    auto bottom_right = x + 1 + y * w; //BOTTOM RIGHT
    auto left = x - 1 + (y - 1) * w; //LEFT
    auto right = x + 1 + (y - 1) * w; //RIGHT
    if(GetType(bottom) == AIR)
    {
        MoveCell(start, bottom);
    } else if(GetType(bottom_left) == AIR && x - 1 >= 0)
    {
        MoveCell(start, bottom_left);
    } else if(GetType(bottom_right) == AIR && x + 1 < w)
    {
        MoveCell(start, bottom_right);
    }/* else if(GetType(start - 1) == AIR && x - 1 >= 0)
    {
        MoveCell(start, start - 1);
    } else if(GetType(start + 1) == AIR && x + 1 < w)
    {
        MoveCell(start, start + 1);
    }*/
}

std::uint8_t World::GetType(std::int32_t ind)
{
    return type[ind];
}

bool World::IsInBounds(std::int32_t x, std::int32_t y)
{
    return x < 0 || x >= w || y < 0 || y >= h;
}

void World::MoveCell(std::int32_t from, std::int32_t to)
{
    SetCell(from, to);
    ZeroCell(from);
}
/*
void World::Collide(std::int32_t index_a, std::int32_t index_b)
{
    //X
    auto vel_x_a = vel_x[index_a];
    auto vel_x_b = vel_x[index_b];
    vel_x[index_a] = (vel_x_a + vel_x_b) / 2 * bounce;
    vel_x[index_b] = (vel_x_a + vel_x_b) / 2 * bounce;
    
    if(std::fabs((int)vel_y[index_a] + (int)vel_y[index_b]) < 2)
        return;
    
    //Y
    auto vel_y_a = vel_y[index_a];
    auto vel_y_b = vel_y[index_b];
    vel_y[index_a] = (vel_y_a + vel_y_b) / 2 * bounce;
    vel_y[index_b] = (vel_y_a + vel_y_b) / 2 * bounce;
    
}*/

void World::Update()
{
    std::memset(scanned, 0, w * h);
    for(auto y = h; y >= 0; y--)
    {
        for(auto x = 0; x < w; x++)
        {
            auto index = x + y * w;
            
            if(scanned[index] != 0)
                continue;
            if(type[index] == AIR)
                continue;
            
            ProcessCell(x, y);
            
            
        }
    }
}

void World::Render(SDL_Renderer* renderer)
{
    auto SetColor = [&](SDL_Color color) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    };
    for(auto y = 0; y < h; y++)
    {
        for(auto x = 0; x < w; x++)
        {
            auto type = GetType(x + y * w);
            auto color = SDL_Color{0,0,0,0};
            switch(type)
            {
                case (AIR): {   } break;
                case (SAND): { color = SDL_Color{252, 186, 3, 0}; } break;
                case (WATER): { color = SDL_Color{3, 136, 252, 0}; } break;
            };
            SetColor(color);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}