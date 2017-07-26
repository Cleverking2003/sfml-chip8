#include <SFML/Graphics.hpp>
#include "chip8.h"
using namespace sf;

Keyboard::Key keys[] = {
    Keyboard::X,
    Keyboard::Num1,
    Keyboard::Num2,
    Keyboard::Num3,
    Keyboard::Q,
    Keyboard::W,
    Keyboard::E,
    Keyboard::A,
    Keyboard::S,
    Keyboard::D,
    Keyboard::Z,
    Keyboard::C,
    Keyboard::Num4,
    Keyboard::R,
    Keyboard::F,
    Keyboard::V
};

int main(){
    //string fileName;
    //cin >> fileName;

    RenderWindow window(VideoMode(640, 320), "Chip 8", Style::Titlebar | Style::Close);
    Chip8 chip8;
    uint8_t* color = new uint8_t[2048 * 4];
    chip8.init("C:\\Users\\Egor\\Downloads\\c8games\\PONG");
    Texture gfx;
    gfx.create(64, 32);
    Sprite display(gfx);
    display.setScale(Vector2f(10, 10));
    Clock clock;
    int time;

    while(window.isOpen()){
        Event e;

        time = clock.getElapsedTime().asMilliseconds();
        if(time % (int)(1000.0f / 60.0f) == 0)
        chip8.cycle();

        while(window.pollEvent(e)){
            if(e.type == Event::Closed) window.close();
            else if(e.type == Event::KeyPressed){
                for(int i = 0; i < 16; i++){
                    if(e.key.code == keys[i]) chip8.key[i] = 1;
                }
            }
            else if(e.type == Event::KeyReleased){
                for(int i = 0; i < 16; i++){
                    if(e.key.code == keys[i]) chip8.key[i] = 0;
                }
            }
        }

        if(chip8.drawFlag){
            for(int i = 0; i < 2048; i++){
                int j = i * 4;
                color[j] = color[j + 1] = color[j + 2] = chip8.vmem[i] * 0xFF;
                color[j + 3] = 0xFF;
            }
            gfx.update(color, 64, 32, 0, 0);
            chip8.drawFlag = false;
        }

        window.clear();
        window.draw(display);
        window.display();
    }

    return 0;
}
