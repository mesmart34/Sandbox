/* date = September 18th 2020 11:14 am */

#ifndef WORLD_H
#define WORLD_H

#include <SDL2/SDL.h>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <ctime>
#include <cstring>
#include <iostream>

#define AIR 0
#define SAND 1
#define WATER 2

class World
{
    public:
    World() = default;
    World(int width, int height) : w(width), h(height) {
        type = new std::uint8_t[w * h]();
        vel_x = new float[w * h]();
        vel_y = new float[w * h]();
        acc_x = new float[w * h]();
        acc_y = new float[w * h]();
        scanned = new std::uint8_t[w * h]();
        //FillRandomly();
    }
    
    void FillRandomly();
    void Update();
    void Render(SDL_Renderer* renderer);
    void Draw(int x, int y);
    void DrawCircle(int x, int y);
    
    private:
    
    void MoveCell(std::uint32_t x, std::uint32_t y);
    void Collide(std::uint32_t index_a, std::uint32_t index_b);
    void SetCell(std::uint32_t last_index, std::uint32_t index);
    void ZeroCell(std::uint32_t index);
    
    float bounce = 1.0f;
    float G = 3.1f;
    int w, h;
    std::uint8_t* type;
    std::uint8_t* scanned;
    float* vel_x;
    float* vel_y;
    float* acc_x;
    float* acc_y;
};

#endif //WORLD_H
