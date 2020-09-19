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
#include <algorithm>

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
    void Draw(int x, int y, std::uint8_t _type);
    void DrawCircle(int x, int y, float r, std::uint8_t type);
    void Erase(int x, int y);
    void EraseCircle(int x, int y, int r);
    
    private:
    
    std::uint8_t GetType(std::int32_t ind);
    void ProcessCell(std::int32_t x, std::int32_t y);
    void MoveCell(std::int32_t from, std::int32_t to);
    //void Collide(std::int32_t index_a, std::int32_t index_b);
    void SetCell(std::int32_t last_index, std::int32_t index);
    void ZeroCell(std::int32_t index);
    bool IsInBounds(std::int32_t x, std::int32_t y);
    void SwapCells(std::int32_t a, std::int32_t b);
    
    //std::uint32_t GetColorFromSDLColor(SDL_Color color);
    
    void UpdateSand(std::int32_t x, std::int32_t y);
    void UpdateWater(std::int32_t x, std::int32_t y);
    
    
    float bounce = 1.0f;
    float G = 9.81f;
    int w, h;
    std::uint8_t* type;
    std::uint8_t* scanned;
    float* vel_x;
    float* vel_y;
    float* acc_x;
    float* acc_y;
};

#endif //WORLD_H
