#include "Animations.h"


//unused, originally for a line read directly from the console
/*
std::vector<std::string> Animations::split(std::string s) {
    size_t pos_start = 0, pos_end = 8, tempStart = 0, tempEnd = 2;
    std::string token, temp;
    std::vector<std::string> res, tempVec;

    for (int i = 0; i < 4; i++)
    {
        if (pos_end > s.size())
            break;

        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end;
        pos_end = pos_end + 8;
        tempStart = 0;
        tempEnd = 2;
        for (int i = 0; i < 4; i++)
        {
            if (tempEnd > token.size())
                break;

            temp = token.substr(tempStart, tempEnd - tempStart);
            tempStart = tempEnd;
            tempEnd = tempEnd + 2;
            tempVec.push_back(temp);
        }
        token.clear();
        if (tempVec.size() == 4)
        {
            token = tempVec[3] + tempVec[2] + tempVec[1] + tempVec[0];
            res.push_back(token);
        }
        tempVec.clear();
    }
    tempVec.clear();
    tempVec.~vector();
    return res;
}*/

//Sets all the PDW animations
void Animations::HandleAnim(int32_t offset, uint8_t* buffer)
{
    short framesTotal = 0, totalNodes = 0, keyframes = 0;

    PDWAnim animation;
    uint32_t animOffset;

    uint8_t* currentBuffer = buffer + offset;

    totalNodes = *reinterpret_cast<uint16_t*>(currentBuffer + 4);
    animation.basePart = *reinterpret_cast<uint16_t*>(currentBuffer + 8);
    framesTotal = *reinterpret_cast<uint16_t*>(currentBuffer + 12);

    uint8_t* animBuffer = currentBuffer + 16;

    for (int i = 0; i < totalNodes; i++)
    {
        keyframes = *reinterpret_cast<uint16_t*>(animBuffer);
        animation.startPosX.push_back(*reinterpret_cast<int16_t*>(animBuffer + 4));
        animation.startPosY.push_back(*reinterpret_cast<int16_t*>(animBuffer + 6));
        animation.startPosZ.push_back(*reinterpret_cast<int16_t*>(animBuffer + 8));
        animation.parentNodes.push_back(*reinterpret_cast<uint16_t*>(animBuffer + 10));
        animOffset = *reinterpret_cast<uint32_t*>(animBuffer + 12);
        animation.keyframes.push_back(keyframes);
        animation.animationsPart.push_back(PDWAnimPart((currentBuffer + animOffset), keyframes));

        animBuffer = animBuffer + 16;
    }

    currentBuffer = nullptr;
    animBuffer = nullptr;

    finalAnimations.push_back(DW1Anim(animation, framesTotal));
}

int Animations::ReadFile(std::string directory)
{
    std::ifstream input;
    input.open(directory, std::ios::binary);
    if (input.good())
    {
        uint32_t* animCount = 0;

        std::streamoff animFileSize = std::filesystem::file_size(directory);

        std::vector<uint8_t> buffer;

        buffer.resize(animFileSize);

        input.read(reinterpret_cast<char*>(buffer.data()), animFileSize);

        uint8_t* offset1 = buffer.data() + 4;

        animCount = reinterpret_cast<uint32_t*>(offset1);

        int offset = 0x24;

        for (int i = 0; i < *animCount; i++)
        {
            HandleAnim(*reinterpret_cast<uint32_t*>(buffer.data() + offset), buffer.data());
            offset = offset + 0x1c;
        }
        input.close();

        animCount = nullptr;
        buffer.clear();
    }
    else
    {
        std::cout << "Unable to read the file" << std::endl;

        std::cin;

        return -1;
    }
}

void Animations::WriteTextFile()
{
    std::cout << "Set the location of the output (default is C)" << std::endl;

    std::string outputDir;

    std::getline(std::cin, outputDir);

    if (std::cin.fail())
    {
        std::cout << "Set default location." << std::endl;

        outputDir = "C:/";
    }

    std::ofstream output(outputDir + "\\anim.txt", std::ios::binary);

    std::string currentAnim;

    bool hasData = false;

    if (output.is_open())
    {
        output << "Nodes: " << std::endl;
        for (int i = 0; i < finalAnimations[0].parentNodes.size(); i++)
        {
            output << std::hex << finalAnimations[0].parentNodes[i] << std::endl;
        }
        output << std::endl;

        for (int i = 0; i < finalAnimations.size(); i++)
        {
            output << "Animation " << std::endl << std::endl;
            output << "Frames:" << std::endl;
            output << std::hex << finalAnimations[i].framesTotal + 1 << std::endl << std::endl;
            output << "Positions:" << std::endl;

            for (int j = 0; j < finalAnimations[i].startPosX.size(); j++)
            {
                if (finalAnimations[i].hasScale)
                {
                    output << std::hex << finalAnimations[i].startScaleX[j] << " ";
                    output << std::hex << finalAnimations[i].startScaleY[j] << " ";
                    output << std::hex << finalAnimations[i].startScaleZ[j] << " ";
                }
                output << std::hex << finalAnimations[i].startRotX[j] << " ";
                output << std::hex << finalAnimations[i].startRotY[j] << " ";
                output << std::hex << finalAnimations[i].startRotZ[j] << " ";
                output << std::hex << finalAnimations[i].startPosX[j] << " ";
                output << std::hex << finalAnimations[i].startPosY[j] << " ";
                output << std::hex << finalAnimations[i].startPosZ[j] << std::endl << std::endl;
            }

            output << "Animations errors:" << std::endl << std::endl;

            for (int j = 0; j < finalAnimations[i].animationsPart.size(); j++)
            {
                output << "Part " << j << std::endl;
                if (finalAnimations[i].hasScale)
                {
                    output << "ScaleX: " << std::hex << std::accumulate(finalAnimations[i].animationsPart[j].ScaleX.begin(), finalAnimations[i].animationsPart[j].ScaleX.end(), 0) << " ";
                    output << "ScaleY: " << std::hex << std::accumulate(finalAnimations[i].animationsPart[j].ScaleY.begin(), finalAnimations[i].animationsPart[j].ScaleY.end(), 0) << " ";
                    output << "ScaleZ: " << std::hex << std::accumulate(finalAnimations[i].animationsPart[j].ScaleZ.begin(), finalAnimations[i].animationsPart[j].ScaleZ.end(), 0) << " ";
                }
                output << "RotX: " << std::hex << std::accumulate(finalAnimations[i].animationsPart[j].RotX.begin(), finalAnimations[i].animationsPart[j].RotX.end(), 0) << " ";
                output << "RotY: " << std::hex << std::accumulate(finalAnimations[i].animationsPart[j].RotY.begin(), finalAnimations[i].animationsPart[j].RotY.end(), 0) << " ";
                output << "RotZ: " << std::hex << std::accumulate(finalAnimations[i].animationsPart[j].RotZ.begin(), finalAnimations[i].animationsPart[j].RotZ.end(), 0) << " ";
                output << "PosX: " << std::hex << std::accumulate(finalAnimations[i].animationsPart[j].PosX.begin(), finalAnimations[i].animationsPart[j].PosX.end(), 0) << " ";
                output << "PosY: " << std::hex << std::accumulate(finalAnimations[i].animationsPart[j].PosY.begin(), finalAnimations[i].animationsPart[j].PosY.end(), 0) << " ";
                output << "PosZ: " << std::hex << std::accumulate(finalAnimations[i].animationsPart[j].PosZ.begin(), finalAnimations[i].animationsPart[j].PosZ.end(), 0);
                output << std::endl;

            }

            output << "Animations:" << std::endl;

            int iteration;
            for (int h = 0; h < finalAnimations[i].framesTotal; h++)
            {
                output << h + 1 << " 00" << std::endl;

                hasData = false;

                for (int j = 0; j < finalAnimations[i].animationsPart.size(); j++)
                {
                    if (finalAnimations[i].animationsPart[j].currentFrame == h)
                    {
                        iteration = finalAnimations[i].animationsPart[j].currentIteration;

                        output << std::hex << finalAnimations[i].animationsPart[j].handler[iteration] << " ";
                        output << std::hex << finalAnimations[i].animationsPart[j].frames[iteration] << "  ";

                        if (finalAnimations[i].hasScale)
                        {
                            if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x40) != 0) output << std::hex << finalAnimations[i].animationsPart[j].ScaleX[iteration] << " ";
                            if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x20) != 0) output << std::hex << finalAnimations[i].animationsPart[j].ScaleY[iteration] << " ";
                            if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x10) != 0) output << std::hex << finalAnimations[i].animationsPart[j].ScaleZ[iteration] << " ";
                        }
                        if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x8) != 0) output << std::hex << finalAnimations[i].animationsPart[j].RotX[iteration] << " ";
                        if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x4) != 0) output << std::hex << finalAnimations[i].animationsPart[j].RotY[iteration] << " ";
                        if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x2) != 0) output << std::hex << finalAnimations[i].animationsPart[j].RotZ[iteration] << " ";
                        if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x1) != 0) output << std::hex << finalAnimations[i].animationsPart[j].PosX[iteration] << " ";
                        if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x8000) != 0) output << std::hex << finalAnimations[i].animationsPart[j].PosY[iteration] << " ";
                        if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x4000) != 0) output << std::hex << finalAnimations[i].animationsPart[j].PosZ[iteration];
                        output << std::endl;

                        finalAnimations[i].animationsPart[j].currentFrame += finalAnimations[i].animationsPart[j].frames[iteration];
                        finalAnimations[i].animationsPart[j].currentIteration++;
                        hasData = true;
                    }
                }
                if (!hasData)
                {
                    bool check = (h + 1) > 0xf;
                    long place = output.tellp();
                    if (check)
                        output.seekp(place - 6);
                    else
                        output.seekp(place - 7);
                }
            }

            output << "00 00" << std::endl << std::endl << std::endl;
        }
        output.close();
    }
    else
        std::cout << "Failed to write to file." << std::endl;
}

void Animations::WriteBINFile()
{
    std::cout << "Set the location of the output (default is C)" << std::endl;

    std::string outputDir;

    std::getline(std::cin, outputDir);

    if (std::cin.fail())
    {
        std::cout << "Set default location." << std::endl;

        outputDir = "C:/";
    }

    std::ofstream output(outputDir + "\\anim.BIN", std::ios::binary);

    std::string currentAnim;

    const char end[2] = { 0, 0 };
    bool hasData = false;

    if (output.is_open())
    {
        for (int i = 0; i < finalAnimations.size(); i++)
        {
            output.write(CastToChar(finalAnimations[i].framesTotal + 1), 2);

            for (int j = 0; j < finalAnimations[i].startPosX.size(); j++)
            {
                if (finalAnimations[i].hasScale)
                {
                    output.write(CastToChar(finalAnimations[i].startScaleX[j]), 2);
                    output.write(CastToChar(finalAnimations[i].startScaleY[j]), 2);
                    output.write(CastToChar(finalAnimations[i].startScaleZ[j]), 2);
                }
                output.write(CastToChar(finalAnimations[i].startRotX[j]), 2);
                output.write(CastToChar(finalAnimations[i].startRotY[j]), 2);
                output.write(CastToChar(finalAnimations[i].startRotZ[j]), 2);
                output.write(CastToChar(finalAnimations[i].startPosX[j]), 2);
                output.write(CastToChar(finalAnimations[i].startPosY[j]), 2);
                output.write(CastToChar(finalAnimations[i].startPosZ[j]), 2);
            }

            int iteration;
            for (int16_t h = 0; h < finalAnimations[i].framesTotal; h++)
            {
                output.write(CastToChar(h + 1), 2);
                hasData = false;

                for (int j = 0; j < finalAnimations[i].animationsPart.size(); j++)
                {
                    if (finalAnimations[i].animationsPart[j].currentFrame == h)
                    {
                        iteration = finalAnimations[i].animationsPart[j].currentIteration;

                        output.write(CastToChar(finalAnimations[i].animationsPart[j].handler[iteration]), 2);
                        output.write(CastToChar(finalAnimations[i].animationsPart[j].frames[iteration]), 2);

                        if (finalAnimations[i].hasScale)
                        {
                            if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x4000) != 0) output.write(CastToChar(finalAnimations[i].animationsPart[j].ScaleX[iteration]), 2);
                            if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x2000) != 0) output.write(CastToChar(finalAnimations[i].animationsPart[j].ScaleY[iteration]), 2);
                            if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x1000) != 0) output.write(CastToChar(finalAnimations[i].animationsPart[j].ScaleZ[iteration]), 2);
                        }
                        if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x800) != 0) output.write(CastToChar(finalAnimations[i].animationsPart[j].RotX[iteration]), 2);
                        if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x400) != 0) output.write(CastToChar(finalAnimations[i].animationsPart[j].RotY[iteration]), 2);
                        if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x200) != 0) output.write(CastToChar(finalAnimations[i].animationsPart[j].RotZ[iteration]), 2);
                        if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x100) != 0) output.write(CastToChar(finalAnimations[i].animationsPart[j].PosX[iteration]), 2);
                        if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x80) != 0) output.write(CastToChar(finalAnimations[i].animationsPart[j].PosY[iteration]), 2);
                        if ((finalAnimations[i].animationsPart[j].handler[iteration] & 0x40) != 0) output.write(CastToChar(finalAnimations[i].animationsPart[j].PosZ[iteration]), 2);


                        finalAnimations[i].animationsPart[j].currentFrame += finalAnimations[i].animationsPart[j].frames[iteration];
                        finalAnimations[i].animationsPart[j].currentIteration++;
                        hasData = true;
                    }
                }
                if (!hasData)
                {
                    long place = output.tellp();
                    output.seekp(place - 2);
                }

            }
            output.write(end, 2);
        }
        output.close();
    }
    else
        std::cout << "Failed to write to file." << std::endl;
}