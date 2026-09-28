#include <iostream>
#include <array>
#include <string>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <random>

std::wstring tetromino[7];
const int fWidth {12};
const int fHeight {24};

const int screenWidth {12};
const int screenHeight {30};

void setTerminalSize(int height, int width){
    std::cout << "\033[8;" << height << ";" << width << "t";
}

void terminalSetup(bool toggle){
    static struct termios oldT, newT;
    if(toggle){
        //std::cout << "\033[?1049h"; // Switch buffer -> alt
        //std::cout << "\033[?25l";   // Hide cursor
        tcgetattr(STDIN_FILENO, &oldT);
        newT = oldT;

        newT.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newT);

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }
    else {
        //std::cout << "\033[?25h";   // Show cursor
        //std::cout << "\033[?1049l"; // Switch buffer -> main
        tcsetattr(STDIN_FILENO, TCSANOW, &oldT);
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
    }
    std::cout << std::flush;
}

int rotate(int px, int py, int r){
    switch(r % 4){
        case 0:
            //0 degrees
            return py * 4 + px;
        case 1:
            //90 degrees
            return 12 + py - (px * 4);
        case 2:
            //180 degrees
            return 15 - (py * 4) - px;
        case 3:
            //270 degrees
            return 3 - py + (px * 4);
    }
    return 0;
}

char getInput(){
    char ch {0};
    if(read(STDIN_FILENO, &ch, 1) < 0){
        return 0;
    }
    return ch;
}

int main(){

    terminalSetup(true);
    //setTerminalSize(screenHeight, screenWidth);
    bool running {true};
    bool falling {false};

    std::wstring t;
    int tx {4}; //Screen midpoint centered
    int ty {0};

    //Create assets
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"..X.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".X..");
    tetromino[1].append(L"....");
    
    tetromino[2].append(L".X..");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"..X.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"....");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L"..X.");

    tetromino[5].append(L"....");
    tetromino[5].append(L"..XX");
    tetromino[5].append(L"..X.");
    tetromino[5].append(L"..X.");

    tetromino[6].append(L"..X.");
    tetromino[6].append(L".XX.");
    tetromino[6].append(L"..X.");
    tetromino[6].append(L"....");

    std::array<int, fWidth * fHeight> pField {};
    for (int x {0}; x < fWidth; x++){
        for (int y {0}; y < fHeight; y++){
            pField[y * fWidth + x] = (x == 0 || x == fWidth - 1 || y == fHeight - 1) ? 9 : 0;
        }
    }

    std::array<char, 10> pChars {{' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', '=', '#'}}; // Array of visual assets
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distr(0,6);

    while(running){
        if (!falling){
            t = tetromino[static_cast<int>(distr(gen))];
        }

        // Drop tetromino
        ty++;

        // Player input
        char key {getInput()};
        switch(key){
            case 'a':
                tx--;
            case 'd':
                tx++;
            case 's':
                ty++;
            //case ' ':
            //  rotate
            case 'q':
                running = false;
            
        }
        // Handle collision

        // Update field

        // Draw field
        std::string frame = "\033[H"; //Move cursor to (0,0)

        for (int y {0}; y < fHeight; y++){
            for (int x {0}; x < fWidth; x++){
                frame += pChars[pField[(y * fWidth + x)]];
            }
            frame += '\n';
        }

        std::cout << frame << std::flush;

        // Sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    terminalSetup(false);
    return 0;
}
