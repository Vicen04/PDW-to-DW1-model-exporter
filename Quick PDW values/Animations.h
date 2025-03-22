#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <format>
#include <numeric> 
#include <string>

struct PDWAnimPart
{
    std::vector<int16_t> posX;
    std::vector<int16_t> posY;
    std::vector<int16_t> posZ;
    std::vector<int16_t> rotX;
    std::vector<int16_t> rotY;
    std::vector<int16_t> rotZ;
    std::vector<int16_t> scaleX;
    std::vector<int16_t> scaleY;
    std::vector<int16_t> scaleZ;
    std::vector <uint8_t> frames;

    // Construtor to set and straight up translate the 
    PDWAnimPart(uint8_t* buffer, int keyFrames)
    {
        int currentOffset = 0;

        for (int i = 0; i < keyFrames; i++)
        {
            posX.push_back(((*reinterpret_cast<int32_t*>(buffer + currentOffset) << 0x9) >> 0x16) << 0x1);
            posY.push_back((*reinterpret_cast<int32_t*>(buffer + currentOffset + 4) << 0x16) >> 0x15);
            posZ.push_back(((*reinterpret_cast<int32_t*>(buffer + currentOffset + 4) << 0xc) >> 0x16) << 0x1);
            rotX.push_back((*reinterpret_cast<int32_t*>(buffer + currentOffset) << 0x13) >> 0x13);
            rotY.push_back((*reinterpret_cast<int32_t*>(buffer + currentOffset + 8) << 0x13) >> 0x13);
            rotZ.push_back((*reinterpret_cast<int32_t*>(buffer + currentOffset + 8) << 0x6) >> 0x13);
            scaleX.push_back((*reinterpret_cast<int32_t*>(buffer + currentOffset + 4) >> 0x14) << 0x2);
            scaleY.push_back((*reinterpret_cast<int32_t*>(buffer + currentOffset + 12) >> 0x6) & 0x3ffc);
            scaleZ.push_back((*reinterpret_cast<int32_t*>(buffer + currentOffset + 12) >> 0x14) << 0x2);
            frames.push_back(*reinterpret_cast<uint8_t*>(buffer + currentOffset + 12) % 0xff);

            currentOffset += 16;
        }
    }

};

struct PDWAnim
{
    std::vector<PDWAnimPart> animationsPart;
    std::vector<short> keyframes;
    std::vector<int16_t> parentNodes;
    std::vector<int16_t> startPosX;
    std::vector<int16_t> startPosY;
    std::vector<int16_t> startPosZ;
    int basePart;
};


struct DW1AnimPart
{
    std::vector<int16_t> PosX;
    std::vector<int16_t> PosY;
    std::vector<int16_t> PosZ;
    std::vector<int16_t> RotX;
    std::vector<int16_t> RotY;
    std::vector<int16_t> RotZ;
    std::vector<int16_t> ScaleX;
    std::vector<int16_t> ScaleY;
    std::vector<int16_t> ScaleZ;
    std::vector<int16_t> frames;
    std::vector<int16_t> handler;
    bool hasScale = false;
    int currentFrame, currentIteration;

    //Check if the movement is the exact same movement in both keyframes
    bool CheckIfOptimisable()
    {
        bool posX, posY, posZ, rotX, rotY, rotZ, scaleX, scaleY, scaleZ;

        posX = (*PosX.rbegin() * *frames.rbegin()) == (*(PosX.rbegin() + 1) * *(frames.rbegin() + 1));
        posY = (*PosY.rbegin() * *frames.rbegin()) == (*(PosY.rbegin() + 1) * *(frames.rbegin() + 1));
        posZ = (*PosZ.rbegin() * *frames.rbegin()) == (*(PosZ.rbegin() + 1) * *(frames.rbegin() + 1));
        rotX = (*RotX.rbegin() * *frames.rbegin()) == (*(RotX.rbegin() + 1) * *(frames.rbegin() + 1));
        rotY = (*RotY.rbegin() * *frames.rbegin()) == (*(RotY.rbegin() + 1) * *(frames.rbegin() + 1));
        rotZ = (*RotZ.rbegin() * *frames.rbegin()) == (*(RotZ.rbegin() + 1) * *(frames.rbegin() + 1));
        scaleX = (*ScaleX.rbegin() * *frames.rbegin()) == (*(ScaleX.rbegin() + 1) * *(frames.rbegin() + 1));
        scaleY = (*ScaleY.rbegin() * *frames.rbegin()) == (*(ScaleY.rbegin() + 1) * *(frames.rbegin() + 1));
        scaleZ = (*ScaleZ.rbegin() * *frames.rbegin()) == (*(ScaleZ.rbegin() + 1) * *(frames.rbegin() + 1));

        return (posX == posY) && (posY == posZ) && (posZ == rotX) && (rotX == rotY) && (rotY == rotZ) &&
            (rotZ == scaleX) && (scaleX == scaleY) && (scaleY == scaleZ) && (scaleZ == true);
    }

    DW1AnimPart(PDWAnimPart animation, short keyFrames, int currentNode)
    {
        //textfile version
        //int currenthandler = 0x80;

        //Bin version
        int currenthandler = 0x8000;
        currentFrame = 0;
        currentIteration = 0;

        for (int i = 0; i < keyFrames - 1; i++)
        {
            PosX.push_back(animation.posX[i + 1] - animation.posX[i]);
            PosY.push_back(animation.posY[i + 1] - animation.posY[i]);
            PosZ.push_back(animation.posZ[i + 1] - animation.posZ[i]);
            RotX.push_back(animation.rotX[i + 1] - animation.rotX[i]);
            RotY.push_back(animation.rotY[i + 1] - animation.rotY[i]);
            RotZ.push_back(animation.rotZ[i + 1] - animation.rotZ[i]);
            ScaleX.push_back(animation.scaleX[i + 1] - animation.scaleX[i]);
            ScaleY.push_back(animation.scaleY[i + 1] - animation.scaleY[i]);
            ScaleZ.push_back(animation.scaleZ[i + 1] - animation.scaleZ[i]);
            frames.push_back(animation.frames[i + 1] - animation.frames[i]);

            //Enable to render a text file
            /*
            if (i == 0)
            {
                currenthandler += (PosX[i] != 0) ? 0x1 : 0x0;
                currenthandler += (PosY[i] != 0) ? 0x8000 : 0x0;
                currenthandler += (PosZ[i] != 0) ? 0x4000 : 0x0;
                currenthandler += (RotX[i] != 0) ? 0x8 : 0x0;
                currenthandler += (RotY[i] != 0) ? 0x4 : 0x0;
                currenthandler += (RotZ[i] != 0) ? 0x2 : 0x0;
                currenthandler += (ScaleX[i] != 0) ? 0x40 : 0x0;
                currenthandler += (ScaleY[i] != 0) ? 0x20 : 0x0;
                currenthandler += (ScaleZ[i] != 0) ? 0x10 : 0x0;
            }
            else
            {
                //checks if there has been any updates, including setting the value to 0
                currenthandler += (*PosX.rbegin() == 0 && *(PosX.rbegin() + 1) == 0) ? 0 : 0x1;
                currenthandler += (*PosY.rbegin() == 0 && *(PosY.rbegin() + 1) == 0) ? 0 : 0x8000;
                currenthandler += (*PosZ.rbegin() == 0 && *(PosZ.rbegin() + 1) == 0) ? 0 : 0x4000;
                currenthandler += (*RotX.rbegin() == 0 && *(RotX.rbegin() + 1) == 0) ? 0 : 0x8;
                currenthandler += (*RotY.rbegin() == 0 && *(RotY.rbegin() + 1) == 0) ? 0 : 0x4;
                currenthandler += (*RotZ.rbegin() == 0 && *(RotZ.rbegin() + 1) == 0) ? 0 : 0x2;
                currenthandler += (*ScaleX.rbegin() == 0 && *(ScaleX.rbegin() + 1) == 0) ? 0 : 0x40;
                currenthandler += (*ScaleY.rbegin() == 0 && *(ScaleY.rbegin() + 1) == 0) ? 0 : 0x20;
                currenthandler += (*ScaleZ.rbegin() == 0 && *(ScaleZ.rbegin() + 1) == 0) ? 0 : 0x10;

                //Remove any empty movement
                if (currenthandler == 0x80 && *handler.rbegin() == (short)(0x80 + currentNode * 0x100))
                {
                    PosX.pop_back();
                    PosY.pop_back();
                    PosZ.pop_back();
                    RotX.pop_back();
                    RotY.pop_back();
                    RotZ.pop_back();
                    ScaleX.pop_back();
                    ScaleY.pop_back();
                    ScaleZ.pop_back();
                    frames.pop_back();

                    *frames.rbegin() = *frames.rbegin() + animation.frames[i + 1] - animation.frames[i];
                }
                //if it is the same movement, remove this keyframe and add the current movement to the previous keyframe
                else if ((short)(currenthandler + currentNode * 0x100) == *handler.rbegin() && CheckIfOptimisable())
                {
                    PosX.pop_back();
                    PosY.pop_back();
                    PosZ.pop_back();
                    RotX.pop_back();
                    RotY.pop_back();
                    RotZ.pop_back();
                    ScaleX.pop_back();
                    ScaleY.pop_back();
                    ScaleZ.pop_back();
                    frames.pop_back();

                    *PosX.rbegin() = *PosX.rbegin() + animation.posX[i + 1] - animation.posX[i];
                    *PosY.rbegin() = *PosY.rbegin() + animation.posY[i + 1] - animation.posY[i];
                    *PosZ.rbegin() = *PosZ.rbegin() + animation.posZ[i + 1] - animation.posZ[i];
                    *RotX.rbegin() = *RotX.rbegin() + animation.rotX[i + 1] - animation.rotX[i];
                    *RotY.rbegin() = *RotY.rbegin() + animation.rotY[i + 1] - animation.rotY[i];
                    *RotZ.rbegin() = *RotZ.rbegin() + animation.rotZ[i + 1] - animation.rotZ[i];
                    *ScaleX.rbegin() = *ScaleX.rbegin() + animation.scaleX[i + 1] - animation.scaleX[i];
                    *ScaleY.rbegin() = *ScaleY.rbegin() + animation.scaleY[i + 1] - animation.scaleY[i];
                    *ScaleZ.rbegin() = *ScaleZ.rbegin() + animation.scaleZ[i + 1] - animation.scaleZ[i];
                    *frames.rbegin() = *frames.rbegin() + animation.frames[i + 1] - animation.frames[i];

                    currenthandler = 0x80;
                    continue;
                }
            }*/

            //disable for a text file
            
            if (i == 0)
            {
                currenthandler += (PosX[i] != 0) ? 0x100 : 0x0;
                currenthandler += (PosY[i] != 0) ? 0x80 : 0x0;
                currenthandler += (PosZ[i] != 0) ? 0x40 : 0x0;
                currenthandler += (RotX[i] != 0) ? 0x800 : 0x0;
                currenthandler += (RotY[i] != 0) ? 0x400 : 0x0;
                currenthandler += (RotZ[i] != 0) ? 0x200 : 0x0;
                currenthandler += (ScaleX[i] != 0) ? 0x4000 : 0x0;
                currenthandler += (ScaleY[i] != 0) ? 0x2000 : 0x0;
                currenthandler += (ScaleZ[i] != 0) ? 0x1000 : 0x0;
            }
           else
            {
                //checks if there has been any updates, including setting the value to 0
                currenthandler += (*PosX.rbegin() == 0 && *(PosX.rbegin() + 1) == 0) ? 0 : 0x100;
                currenthandler += (*PosY.rbegin() == 0 && *(PosY.rbegin() + 1) == 0) ? 0 : 0x80;
                currenthandler += (*PosZ.rbegin() == 0 && *(PosZ.rbegin() + 1) == 0) ? 0 : 0x40;
                currenthandler += (*RotX.rbegin() == 0 && *(RotX.rbegin() + 1) == 0) ? 0 : 0x800;
                currenthandler += (*RotY.rbegin() == 0 && *(RotY.rbegin() + 1) == 0) ? 0 : 0x400;
                currenthandler += (*RotZ.rbegin() == 0 && *(RotZ.rbegin() + 1) == 0) ? 0 : 0x200;
                currenthandler += (*ScaleX.rbegin() == 0 && *(ScaleX.rbegin() + 1) == 0) ? 0 : 0x4000;
                currenthandler += (*ScaleY.rbegin() == 0 && *(ScaleY.rbegin() + 1) == 0) ? 0 : 0x2000;
                currenthandler += (*ScaleZ.rbegin() == 0 && *(ScaleZ.rbegin() + 1) == 0) ? 0 : 0x1000;

                //Remove any empty movement
                if (currenthandler == 0x8000 && *handler.rbegin() == (short)(0x8000 + currentNode))
                {
                    PosX.pop_back();
                    PosY.pop_back();
                    PosZ.pop_back();
                    RotX.pop_back();
                    RotY.pop_back();
                    RotZ.pop_back();
                    ScaleX.pop_back();
                    ScaleY.pop_back();
                    ScaleZ.pop_back();
                    frames.pop_back();

                    *frames.rbegin() = *frames.rbegin() + animation.frames[i + 1] - animation.frames[i];
                }
                //if it is the same movement, remove this keyframe and add the current values to the previous keyframe
                else if ((short)(currenthandler + currentNode) == *handler.rbegin() && CheckIfOptimisable())
                {
                    PosX.pop_back();
                    PosY.pop_back();
                    PosZ.pop_back();
                    RotX.pop_back();
                    RotY.pop_back();
                    RotZ.pop_back();
                    ScaleX.pop_back();
                    ScaleY.pop_back();
                    ScaleZ.pop_back();
                    frames.pop_back();

                    *PosX.rbegin() = *PosX.rbegin() + animation.posX[i + 1] - animation.posX[i];
                    *PosY.rbegin() = *PosY.rbegin() + animation.posY[i + 1] - animation.posY[i];
                    *PosZ.rbegin() = *PosZ.rbegin() + animation.posZ[i + 1] - animation.posZ[i];
                    *RotX.rbegin() = *RotX.rbegin() + animation.rotX[i + 1] - animation.rotX[i];
                    *RotY.rbegin() = *RotY.rbegin() + animation.rotY[i + 1] - animation.rotY[i];
                    *RotZ.rbegin() = *RotZ.rbegin() + animation.rotZ[i + 1] - animation.rotZ[i];
                    *ScaleX.rbegin() = *ScaleX.rbegin() + animation.scaleX[i + 1] - animation.scaleX[i];
                    *ScaleY.rbegin() = *ScaleY.rbegin() + animation.scaleY[i + 1] - animation.scaleY[i];
                    *ScaleZ.rbegin() = *ScaleZ.rbegin() + animation.scaleZ[i + 1] - animation.scaleZ[i];
                    *frames.rbegin() = *frames.rbegin() + animation.frames[i + 1] - animation.frames[i];

                    currenthandler = 0x8000;
                    continue;
                }
            }

            //textfile version
            //handler.push_back(currenthandler + currentNode * 0x100);
            //currenthandler = 0x80;
            //(currenthandler & 0x70) ? hasScale = true : hasScale = false;

            //Bin version
            handler.push_back(currenthandler + currentNode);
            currenthandler = 0x8000;
            (currenthandler & 0x7000) ? hasScale = true : hasScale = false;
        }
    }
};

struct DW1Anim
{
    std::vector<DW1AnimPart> animationsPart;
    std::vector<int16_t> parentNodes;
    std::vector<int16_t> startPosX;
    std::vector<int16_t> startPosY;
    std::vector<int16_t> startPosZ;
    std::vector<int16_t> startRotX;
    std::vector<int16_t> startRotY;
    std::vector<int16_t> startRotZ;
    std::vector<int16_t> startScaleX;
    std::vector<int16_t> startScaleY;
    std::vector<int16_t> startScaleZ;
    short framesTotal;
    bool hasScale = false;

    DW1Anim(PDWAnim anim, int totalF)
    {
        parentNodes.push_back(-1);
        framesTotal = totalF;

        for (int i = 0; i < anim.animationsPart.size(); i++)
        {
            int parentPart;
            if (i != anim.basePart)
            {
                //nodes only are shown in the txt version
                if (anim.parentNodes[i] != anim.basePart)
                {
                    parentPart = anim.parentNodes[i];
                    parentNodes.push_back(i * 0x100 + ((parentPart + 2)));
                }
                else
                    parentNodes.push_back(i * 0x100 + 1);

                //unsure if the start position is necessary
                startPosX.push_back(anim.animationsPart[i].posX[0] + anim.startPosX[i]);
                startPosY.push_back(anim.animationsPart[i].posY[0] + anim.startPosY[i]);
                startPosZ.push_back(anim.animationsPart[i].posZ[0] + anim.startPosZ[i]);
                startRotX.push_back(anim.animationsPart[i].rotX[0]);
                startRotY.push_back(anim.animationsPart[i].rotY[0]);
                startRotZ.push_back(anim.animationsPart[i].rotZ[0]);
                startScaleX.push_back(anim.animationsPart[i].scaleX[0]);
                startScaleY.push_back(anim.animationsPart[i].scaleY[0]);
                startScaleZ.push_back(anim.animationsPart[i].scaleZ[0]);
            }
            else
            {
                //unsure if the start position is necessary
                parentNodes.insert(parentNodes.begin() + 1, (0xFF00));
                startPosX.insert(startPosX.begin(), anim.animationsPart[i].posX[0] + anim.startPosX[i]);
                startPosY.insert(startPosY.begin(), anim.animationsPart[i].posY[0] + anim.startPosY[i]);
                startPosZ.insert(startPosZ.begin(), anim.animationsPart[i].posZ[0] + anim.startPosZ[i]);
                startRotX.insert(startRotX.begin(), anim.animationsPart[i].rotX[0]);
                startRotY.insert(startRotY.begin(), anim.animationsPart[i].rotY[0]);
                startRotZ.insert(startRotZ.begin(), anim.animationsPart[i].rotZ[0]);
                startScaleX.insert(startScaleX.begin(), anim.animationsPart[i].scaleX[0]);
                startScaleY.insert(startScaleY.begin(), anim.animationsPart[i].scaleY[0]);
                startScaleZ.insert(startScaleZ.begin(), anim.animationsPart[i].scaleZ[0]);
            }

            if (i != anim.basePart)
            {
                animationsPart.push_back(DW1AnimPart(anim.animationsPart[i], anim.keyframes[i], i + 2));
                if (!hasScale) hasScale = animationsPart[i].hasScale;
            }
            else
            {
                animationsPart.insert(animationsPart.begin(), DW1AnimPart(anim.animationsPart[i], anim.keyframes[i], 1));
                if (!hasScale) hasScale = animationsPart[0].hasScale;
            }
        }

    }
};

class Animations
{
protected:    

    char values[2];
    char* CastToChar(int16_t value)
    {
        values[0] = value & 0xFF;
        values[1] = value >> 8;

        return &values[0];
    }

    std::vector<std::string> split(std::string s);
    void HandleAnim(int32_t offset, uint8_t* buffer);
    std::vector <DW1Anim> finalAnimations;

public:
    int ReadFile(std::string directory);
    void WriteTextFile();
    void WriteBINFile();
};

