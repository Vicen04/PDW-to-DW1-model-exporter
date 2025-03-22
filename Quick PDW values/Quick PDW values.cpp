// Quick PDW values.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "Animations.h"

std::string input;

int main()
{
    /*
    std::cout << "Paste the bytes here (no spaces)" << std::endl;

    int32_t integer1 = 0, integer2 = 0, integer3 = 0, integer4 = 0, posX = 0, posY = 0, posZ = 0, rotX = 0, rotY = 0, rotZ = 0, scaleX = 0, scaleY = 0, scaleZ = 0;

    while (true)
    {
        std::cin >> input;

        if (std::cin.fail())
        {
            std::cout << "unexpected error." << std::endl;
            return -1;
        }
        else if (input == "exit")
            return -1;

        std::vector<std::string> temp = split(input);

        if (temp.size() == 4)
        {

            integer1 = std::stoul(temp[0], nullptr, 16);
            integer2 = std::stoul(temp[1], nullptr, 16);
            integer3 = std::stoul(temp[2], nullptr, 16);
            integer4 = std::stoul(temp[3], nullptr, 16);

            std::cout <<  std::hex << integer1 << std::endl;
            std::cout <<  std::hex << integer2 << std::endl;
            std::cout << std::hex << integer3 << std::endl;
            std::cout <<  std::hex << integer4 << std::endl;

            temp.clear();
            temp.~vector();

            posX = ((integer1 << 0x9) >> 0x16) << 0x1;
            posY = (integer2 << 0x16) >> 0x15;
            posZ = ((integer2 << 0xc) >> 0x16) << 0x1;
            rotX = (integer1 << 0x13) >> 0x13;
            rotY = (integer3 << 0x13) >> 0x13;
            rotZ = (integer3 << 0x6) >> 0x13;
            scaleX = (integer2 >> 0x14) << 0x2;
            scaleY = (integer4 >> 0x6) & 0x3ffc;
            scaleZ = (integer4 >> 0x14) << 0x2;

            std::cout << "Position X: " << std::hex << posX << std::endl;
            std::cout << "Position Y: " << std::hex << posY << std::endl;
            std::cout << "Position Z: " << std::hex << posZ << std::endl;
            std::cout << "Rotation X: " << std::hex << rotX << std::endl;
            std::cout << "Rotation Y: " << std::hex << rotY << std::endl;
            std::cout << "Rotation Z: " << std::hex << rotZ << std::endl;
            std::cout << "Scale X: " << std::hex << scaleX << std::endl;
            std::cout << "Scale Y: " << std::hex << scaleY << std::endl;
            std::cout << "Scale Z: " << std::hex << scaleZ << std::endl;
        }
        else
            std::cout << "unexpected error." << std::endl;


    }
    */

    Animations anim;

    std::cout << "Paste the location of the MTD data file" << std::endl;

    std::getline(std::cin, input);

    if (std::cin.fail())
    {
        std::cout << "unexpected error." << std::endl;
        return -1;
    }
    else if (input == "exit")
        return -1;

    if ( anim.ReadFile(input) != -1)
    {
        //anim.WriteTextFile();
        anim.WriteBINFile();

        std::cout << "Animation file created" << std::endl;

        std::cin;

        return -1;
    }
    else
    return -1;
    

}




// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
