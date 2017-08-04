#include <SFML/Graphics.hpp>
#include <iostream>
#include "chip8.h"
typedef uint8_t u8;
typedef uint16_t u16;
using namespace sf;
using namespace std;

Keyboard::Key key[] = {
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
    cout << "Enter file name: \n";
    string file;
    cin >> file;

	RenderWindow window(VideoMode(640, 320), "Chip8");

	Chip8 chip8;
    chip8.init(file);

    //u8 color[64 * 32 * 4] = {0};
    Texture gfx;
    gfx.create(64, 32);
    Sprite display(gfx);
    display.setScale(10.0, 10.0);

	while(window.isOpen()){
        Event e;
        chip8.exec();

        while(window.pollEvent(e)){
            if(e.type == Event::Closed) window.close();
            else if(e.type == Event::KeyPressed){
                for(int i = 0; i < 16; i++){
                    if(e.key.code == key[i]) chip8.keys[i] = 1;
                }
            }
            else if(e.type == Event::KeyReleased){
                for(int i = 0; i < 16; i++){
                    if(e.key.code == key[i]) chip8.keys[i] = 0;
                }
            }
        }

        if(chip8.drawFlag){
            chip8.drawFlag = false;

            Uint8* color = new Uint8[64 * 32 * 4];
            for(int i = 0; i < 64 * 32; i++){
                int j = i * 4;
                u8 pixel = chip8.gfx[i];
                color[j] = pixel * 255;
                color[j + 1] = color[j + 2] = pixel * 255;
                color[j + 3] = 255;
            }
            gfx.update(color);
            delete[] color;
        }

        window.clear();
        window.draw(display);
        window.display();

        sleep(microseconds(1200));
	}

	return 0;
}
