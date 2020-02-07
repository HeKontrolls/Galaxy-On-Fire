/*
    Procedural Generation: Programming The Universe
    "Here we go again! Year 4 begins now..." - javidx9

    License (OLC-3)
    ~~~~~~~~~~~~~~~

    Copyright 2018-2020 OneLoneCoder.com

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions or derivations of source code must retain the above
    copyright notice, this list of conditions and the following disclaimer.

    2. Redistributions or derivative works in binary form must reproduce
    the above copyright notice. This list of conditions and the following
    disclaimer must be reproduced in the documentation and/or other
    materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Relevant Video: https://youtu.be/ZZY9YE7rZJw

    Links
    ~~~~~
    YouTube:	https://www.youtube.com/javidx9
                https://www.youtube.com/javidx9extra
    Discord:	https://discord.gg/WhwHUMV
    Twitter:	https://www.twitter.com/javidx9
    Twitch:		https://www.twitch.tv/javidx9
    GitHub:		https://www.github.com/onelonecoder
    Patreon:	https://www.patreon.com/javidx9
    Homepage:	https://www.onelonecoder.com

    Author
    ~~~~~~
    David Barr, aka javidx9, �OneLoneCoder 2018, 2019, 2020
*/



#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <random>
#include <iostream>
using namespace std;

constexpr uint32_t g_starColours[8] =
{
        0xFFFFFFFF, 0xFFD9FFFF, 0xFFA3FFFF, 0xFFFFC8C8,
        0xFFFFCB9D, 0xFF9F9FFF, 0xFF415EFF, 0xFF28199D
};

constexpr uint32_t g_planetColours[5] =
{
    // TODO: The color expects a hex containing alpha, blue, green and red, in this especific order.
//       It's inverveted thanks to the `union` behavior in olc::Pixel class.
//
//       take a look:
//       https://github.com/OneLoneCoder/olcPixelGameEngine/blob/75cc6d902ef3331540d0d951792a53c6f251e6fe/olcPixelGameEngine.h#L286
//
//	 The `union` will fill each channel and `n` method automatically when you set the color,
//       but it does not respect the order of the members assined to the struct (r, g, b, a) inverting
//       the sequense to `a`, `b`, `g`, `r`.
//
//       It means 0xFF001122 is equivalent to rgba(0x22,0x11,0x00,0xFF)
//
//       It's a David's design decision that could be "fixed" inverting the union struct, something like:
//
                //       struct Pixel
//       {
//           union {
//               uint32_t n;
//               struct
//               {
//                   uint8_t a;
//                   uint8_t b;
//                   uint8_t g;
//                   uint8_t r;
//               }
//           }
//           ...
//       }
//
//
                0xFFFF0000, 0xFFFF9000, 0xFFCC0000, 0xFF004EFF,
                0xFF00CC00
};

struct sPlanet
{
    double distance = 0.0;
    double diameter = 0.0;
    double foliage = 0.0;
    double minerals = 0.0;
    double water = 0.0;
    double gases = 0.0;
    double temperature = 0.0;
    double population = 0.0;
    olc::Pixel planetColour = olc::WHITE;
    bool ring = true;
    std::vector<double> vMoons;
};

class cStarSystem
{
public:
    cStarSystem(uint32_t x, uint32_t y, bool bGenerateFullSystem = false)
    {
        // Set seed based on location of star system
        nProcGen = (x & 0xFFFF) << 16 | (y & 0xFFFF);

        // Not all locations contain a system
        starExists = (rndInt(0, 20) == 1);
        if (!starExists) return;

        // Generate Star
        starDiameter = rndDouble(20.0, 40.0);
        starColour = g_starColours[rndInt(0, 8)];

        if (starTemeprature)

            // When viewing the galaxy map, we only care about the star
            // so abort early
            if (!bGenerateFullSystem) return;

        // If we are viewing the system map, we need to generate the
        // full system

        // Generate Planets
        double dDistanceFromStar = rndDouble(120.0, 200.0);
        int nPlanets = rndInt(0, 10);
        //planetcolour = g_planetColours[rndInt(0, 8)];
        for (int i = 0; i < nPlanets; i++)
        {

            sPlanet p;

            p.distance = dDistanceFromStar;
            dDistanceFromStar += rndDouble(90.0, 200.0);
            p.diameter = rndDouble(4.0, 20.0);

            // Could make temeprature a function of distance from star
            p.temperature = rndDouble(-200.0, 300.0);

            // Composition of planet
            p.foliage = rndDouble(0.0, 1.0);
            p.minerals = rndDouble(0.0, 1.0);
            p.gases = rndDouble(0.0, 1.0);
            p.water = rndDouble(0.0, 1.0);

            // Normalise to 100%
            double dSum = 1.0 / (p.foliage + p.minerals + p.gases + p.water);
            p.foliage *= dSum;
            p.minerals *= dSum;
            p.gases *= dSum;
            p.water *= dSum;

            // Population could be a function of other habitat encouraging
            // properties, such as temperature and water
            p.population = std::max(rndInt(-5000000, 20000000), 0);

            // 10% of planets have a ring
            p.ring = rndInt(0, 45) == 1;

            //p.planetcolor = g_planetColours[0, 5];

            // Satellites (Moons)
            int nMoons = std::max(rndInt(-5, 5), 0);
            for (int n = 0; n < nMoons; n++)
            {
                // A moon is just a diameter for now, but it could be
                // whatever you want!
                p.vMoons.push_back(rndDouble(1.0, 5.0));
            }

            // Add planet to vector
            vPlanets.push_back(p);
        }
    }

    ~cStarSystem()
    {

    }

public:
    std::vector<sPlanet> vPlanets;

public:
    bool		starExists = false;
    double		starDiameter = 0.0f;
    double		starTemeprature = 0.0;
    olc::Pixel	starColour = olc::WHITE;
    int solarStarDiameter = 50;

private:
    uint32_t nProcGen = 0;

    double rndDouble(double min, double max)
    {
        return ((double)rnd() / (double)(0x7FFFFFFF)) * (max - min) + min;
    }

    int rndInt(int min, int max)
    {
        return (rnd() % (max - min)) + min;
    }

    uint32_t rnd()
    {
        nProcGen += 0xe120fc15;
        uint64_t tmp;
        tmp = (uint64_t)nProcGen * 0x4a39b70d;
        uint32_t m1 = (tmp >> 32) ^ tmp;
        tmp = (uint64_t)m1 * 0x12fad5c9;
        uint32_t m2 = (tmp >> 32) ^ tmp;
        return m2;
    }

};

class olcGalaxy : public olc::PixelGameEngine
{
public:
    olcGalaxy()
    {
        sAppName = "Galaxy On Fire";
    }

public:
    bool OnUserCreate() override
    {
        return true;
    }

    olc::vf2d vGalaxyOffset = { 0,0 };
    bool bStarSelected = false;
    uint32_t nSelectedStarSeed1 = 0;
    uint32_t nSelectedStarSeed2 = 0;

    // TODO:
    // we need it accessible to the class without to restart it
    double angle = 0.f;

    bool OnUserUpdate(float fElapsedTime) override
    {
        if (fElapsedTime <= 0.0001f) return true;
        Clear(olc::BLACK);

        if (GetKey(olc::W).bHeld) vGalaxyOffset.y -= 50.0f * fElapsedTime;
        if (GetKey(olc::S).bHeld) vGalaxyOffset.y += 50.0f * fElapsedTime;
        if (GetKey(olc::A).bHeld) vGalaxyOffset.x -= 50.0f * fElapsedTime;
        if (GetKey(olc::D).bHeld) vGalaxyOffset.x += 50.0f * fElapsedTime;

        int nSectorsX = ScreenWidth() / 16;
        int nSectorsY = ScreenHeight() / 16;

        olc::vi2d mouse = { GetMouseX() / 16, GetMouseY() / 16 };
        olc::vi2d galaxy_mouse = mouse + vGalaxyOffset;
        olc::vi2d screen_sector = { 0,0 };

        for (screen_sector.x = 0; screen_sector.x < nSectorsX; screen_sector.x++)
            for (screen_sector.y = 0; screen_sector.y < nSectorsY; screen_sector.y++)
            {
                uint32_t seed1 = (uint32_t)vGalaxyOffset.x + (uint32_t)screen_sector.x;
                uint32_t seed2 = (uint32_t)vGalaxyOffset.y + (uint32_t)screen_sector.y;

                cStarSystem star(seed1, seed2);
                if (star.starExists)
                {
                    FillCircle(screen_sector.x * 16 + 8, screen_sector.y * 16 + 8,
                        (int)star.starDiameter / 8, star.starColour);

                    // For convenience highlight hovered star
                    if (mouse.x == screen_sector.x && mouse.y == screen_sector.y)
                    {

                        DrawCircle(screen_sector.x * 16 + 8, screen_sector.y * 16 + 8, 12, olc::GREEN);
                    }
                }
            }

        // Handle Mouse Click
        if (GetMouse(0).bPressed)
        {
            uint32_t seed1 = (uint32_t)vGalaxyOffset.x + (uint32_t)mouse.x;
            uint32_t seed2 = (uint32_t)vGalaxyOffset.y + (uint32_t)mouse.y;

            cStarSystem star(seed1, seed2);
            if (star.starExists)
            {
                bStarSelected = true;
                nSelectedStarSeed1 = seed1;
                nSelectedStarSeed2 = seed2;
            }
            else
                bStarSelected = false;
        }

        // Draw Details of selected star system
        if (bStarSelected)
        {
            // Generate full star system

            cStarSystem star(nSelectedStarSeed1, nSelectedStarSeed2, true);

            // Draw Window
            FillRect(8, 240, 545, 232, olc::VERY_DARK_GREY);
            DrawRect(8, 240, 545, 232, olc::WHITE);

            // Draw star name & create randomly generated characters for the star names
            srand(nSelectedStarSeed1 << nSelectedStarSeed2);
            std::string StarName = "";
            for (int i = 0; i < 3; i++)
                StarName += 'A' + rand() % 26;
            StarName += ":";
            for (int i = 0; i < 4; i++)
                StarName += '1' + rand() % 9;

            // Display star name on system information.
            DrawString(350, 250, "Star Name: " + std::string(StarName), olc::WHITE);

            // Draw Star
            olc::vi2d vBody = { 14, 356 };
            vBody.x += star.starDiameter * 1.375;

            olc::vi2d sBody = { 8, 356 };
            sBody.x = star.starDiameter / 2;
            FillCircle(vBody, (int)(star.starDiameter * 1.375), star.starColour);
            //Draws the star name in the star.
            /*TODO:: Make it so that the star name fits in the center and changes font size with the Star size.
                Will need to make a if statement that handles this

                if starsize is less then a certain amount font size is ex:8 else font size is normal 10 or 11.
            */
            //DrawString(sBody, (std::string(StarName)), olc::MAGENTA);
            DrawCircle(vBody, (int)(star.starDiameter * 1.375) + 3, olc::DARK_YELLOW);

            vBody.x += (star.starDiameter * 1.375) + 8;

            // Planet count
            int planetcount = 0;

            // Moon count
            int mooncount = 0;

            // Draw Planets
            for (auto& planet : star.vPlanets)
            {
                // really janky way to get planet count.
                planetcount++;

                if (vBody.x + planet.diameter >= 496) break;

                vBody.x += planet.diameter;

                // If planet has ring and is bigger than 15
                if (planet.ring == true && planet.diameter >= 15)
                {
                    //Ellipse(0, 200, 200, 100, 100);
                    DrawCircle(vBody, (int)(planet.diameter * 1) + 1, olc::GREY);
                    DrawCircle(vBody, (int)(planet.diameter * 1) + 2, olc::VERY_DARK_GREY);
                }

                // TODO: You should use the "planet" reference created by the for loop (above) and not a instance of a new sPlanet as commented bellow,
                //       otherwise the temperature will be a constant (zero) forcing the same color to be used. It was creating the problem.
                // sPlanet p;

                int planetColourIndex;

                // TODO: Note it's `planet.temperature` and not `p.temperature`.
                if (planet.temperature >= -200 && planet.temperature < -100)
                {
                    planetColourIndex = 0;
                }
                else if (planet.temperature >= -100 && planet.temperature < 0)
                {
                    planetColourIndex = 1;
                }
                else if (planet.temperature >= 0 && planet.temperature < 100)
                {
                    planetColourIndex = 2;
                }
                else if (planet.temperature >= 100 && planet.temperature < 200)
                {
                    planetColourIndex = 3;
                }
                else
                {
                    // TODO: The array has 5 colors, but it starts counting from zero, so you can
                    //       assign a value greater than 4.
                    //
                    // number of colors:                    1   2   3   4   5
                    // array index to access each color:    0   1   2   3   4
                    //
                    planetColourIndex = 4;
                }

                // TODO: I've tried to avoid redundant code calling it just once,
                //       but it has the same logic.
                planet.planetColour.n = g_planetColours[planetColourIndex];

                FillCircle(vBody, (int)(planet.diameter * 1.0), planet.planetColour);

                olc::vi2d vMoon = vBody;
                vMoon.y += planet.diameter + 10;

                // Draw Moons
                for (auto& moon : planet.vMoons)
                {
                    // really janky way to get moon count.
                    mooncount++;
                    vMoon.y += moon;
                    FillCircle(vMoon, (int)(moon * 1.0), olc::RED);
                    vMoon.y += moon + 10;
                }

                vBody.x += planet.diameter + 8;
            }

            // that janky way for planet count
            char buffer[20];
            snprintf(buffer, 20, "Planets: %d", planetcount);
            DrawString(350, 260, buffer, olc::WHITE);

            // that janky way for moon count
            char buffer1[20];
            snprintf(buffer1, 20, "Moon: %d", mooncount);
            DrawString(350, 270, buffer1, olc::WHITE);

            // Draw Solar System
            olc::vi2d vsBody = { 240, 225 };

            vsBody.x += star.solarStarDiameter;

            //olc::vi2d solarBody = { 545 / 2, 232 / 2 };
            //solarBody.x = star.solarStarDiameter;
               
            // If I is held down it will draw the solar system window
            if (GetKey(olc::I).bHeld)
            {
                //PW, PH, W, H
                FillRect(8, 5, 545, 468, olc::BLACK);
                DrawRect(8, 5, 545, 468, olc::WHITE);
                DrawString(10, 8, std::string(StarName) + " Solar System ", olc::WHITE);

                FillCircle(vsBody, (int)(star.starDiameter / 10), star.starColour);

                // TODO:
                // We added the time of each frame to the angle to make it to expand infinitely
                angle += fElapsedTime;

                // TODO:
                // We get the vector size and add one to it to avoid illegal access of -1 index.
                // It'll be used to determine the velocity of each planet around the sun
                // the last one must be the slower and the first one the fastest, so 
                // we need to invert the order
                int planetVelocity = star.vPlanets.size() + 1;
                for (auto p = 0; p < star.vPlanets.size(); ++p)
                {
                    // You will need check the star diameter to get the correct star position
                    // because initial orbit position starts from it.
                    int start_position = star.starDiameter * 0.3;

                    // the position of each orbit depends on loop iteration number
                    // to sum the same distance to each one.
                    // 20 is just the real space between them, you can change this number.
                    auto space_between_orbits = p * 20;

                    // you'll need to access the planet color member, using the array index
                    // to get the current planet struct
                    auto planetColour = star.vPlanets[p].planetColour;
                     
                    // Note we are sum the space_between_orbits to the initial value (the sum diameter)
                    // otherwise the first orbit will start inside the sun
                    DrawCircle(vsBody, start_position + space_between_orbits, olc::VERY_DARK_GREY);

                    // TODO:
                    // we are calculating the sin and cos of the angle. It will calc the correct coordinate to each
                    // planet and we are adapting the velocity of each one.
                    // As far the planet is from the sun, more slow it is (thanks planetVelocity varible)
                    int planetY = (start_position + space_between_orbits) * std::sin(angle * planetVelocity * 3.141592 / 180);
                    int planetX = (start_position + space_between_orbits) * std::cos(angle * planetVelocity * 3.141592 / 180);

                    // TODO:
                    // Now, we use the planetX and planetY to push the planet to the orbit of the  sun
                    FillCircle(vsBody.x + planetX, vsBody.y + planetY, 4, planetColour);
                    
                    
                    // TODO:
                    // We need to decrement the variable to
                    planetVelocity--;
                }
            }
        }

        return true;
    }
};

int main()
{

    olcGalaxy demo;
    if (demo.Construct(560, 480, 2, 2, false, false))
        demo.Start();
    return 0;


}