#include <SFML/Graphics.hpp> // sfml 4.2
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <math.h>

class Chars{
    public:
    sf::Font fontello;
    sf::Text character;
    std::vector <sf::Text> fade;
    int record_fall;
    int max_lenght;
    int const_max_lenght;
    float light;

    Chars(int x=0){
        fontello.loadFromFile("fonts/lucon.ttf");
        character.setFont(fontello);
        character.setCharacterSize(22);
        character.setColor(sf::Color(0,63,0));
        character.setStyle(sf::Text::Bold);
        character.setPosition(x, 0);
        record_fall = 0;
        max_lenght = 32;
        const_max_lenght = 32;
        light = 1.05;

        fade.push_back(character);
        fade[0].setString(char(33 + rand()%93));
    }
    void randString(){
        fade[0].setString(char(33 + rand()%93));
    }
    void updateFade(){
        int limit = fade.size(); // if fade.push_back() then don't fall last char
        if(fade.size() < max_lenght){
            fade.push_back(fade[limit - 1]);
            //fade[limit].setColor(sf::Color(int(fade[limit - 1].getColor().r/(limit*1.3)), rand()%200 ,int(fade[limit - 1].getColor().b/(limit*1.3))));
            fade[limit].setColor(sf::Color(int(fade[limit - 1].getColor().b/(limit*1.3)), int(fade[limit - 1].getColor().g/light) ,int(fade[limit - 1].getColor().b/(limit*1.3))));
        }
        for(int i=limit - 1; i>0; i--){
            fade[i].setPosition(fade[i].getPosition().x, fade[i].getPosition().y+16);
            fade[i].setString(fade[i-1].getString());
        }
        fade[0].setPosition(fade[0].getPosition().x, fade[0].getPosition().y+16);
    }
    ~Chars(){}
};

int main()
{
    //Sleep(5000);

    int button_x = sf::Mouse::getPosition().x;
    int button_y = sf::Mouse::getPosition().y;

    sf::RenderWindow window(sf::VideoMode().getDesktopMode(), "Typing Game", sf::Style::Titlebar | sf::Style::Close /*,sf::Style::Fullscreen*/);
    window.setFramerateLimit(30);
    srand(time(NULL));

    sf::Music music;
    if (!music.openFromFile("sounds/cubbed_to_death.ogg")) return -1;
    music.setLoop(true);
    music.setVolume(100);
    music.play();

    Chars char_template;
    std::vector <Chars> matrix;

    int record_spinn = 0;
    int record_spawn = 0;
    int record_light = 0;
    int spawn_speed = 0;
    bool moving_mouse = false;
    bool returning_mouse = false;

    // ====================================================[ \ / ]===========[ \ / ]====================================================
    // ====================================================|  |  | GAME LOOP |  |  |====================================================
    // ====================================================[ / \ ]===========[ / \ ]====================================================
    while(window.isOpen()){

        int record = clock();

        sf::Event event;
        if(window.pollEvent(event)){
            if(event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
            if(event.type == sf::Event::KeyPressed){
                char key = char(event.text.unicode + 97);
                if(key == -93){
                    moving_mouse = !moving_mouse;
                    returning_mouse = false;
                } else if(key == -97){
                    returning_mouse = moving_mouse = !returning_mouse;
                }
            }
        }
        if(moving_mouse && record - record_spinn > 5000){
            int x = sf::Mouse::getPosition().x;
            int y = sf::Mouse::getPosition().y;
            sf::Mouse::setPosition(sf::Vector2i(button_x,button_y));

            if(returning_mouse){
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                sf::Mouse::setPosition(sf::Vector2i(x,y));
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            }
            record_spinn = record;
        }

        if(record - record_spawn > spawn_speed){
            if(matrix.size() < 512)
            {
                char_template.fade[0].setPosition(16*(rand()%105), 0);
                char_template.randString();
                matrix.push_back(char_template);
            }
            int a = rand()%matrix.size();
            if(matrix[a].fade[0].getColor().g + 32 < 255){
                matrix[a].fade[0].setColor(sf::Color(matrix[a].fade[0].getColor().r + 16, matrix[a].fade[0].getColor().g + 32, matrix[a].fade[0].getColor().b + 16));
                matrix[a].const_max_lenght ++;
                matrix[a].light -= 0.001;
            }
            spawn_speed = 100 + rand()%300;
            record_spawn = record;
        }

        // ====================================================[ | ]==============[ | ]====================================================
        // ====================================================| | |     WINDOW CLEAR       | | |====================================================
        // ====================================================[ | ]==============[ | ]====================================================
        window.clear(sf::Color(0,0,0));

        for(int i=0; i<matrix.size(); i++){ // fall
            if(record-matrix[i].record_fall > 200/(1+sqrt(sqrt(sqrt(matrix.size()))))){
                matrix[i].updateFade();
                matrix[i].randString();

                if(matrix[i].fade[0].getPosition().y > 1000 + 16*matrix[i].max_lenght){
                    matrix[i].max_lenght = matrix[i].const_max_lenght;
                    matrix[i].fade[0].setPosition(16*(rand()%105), 0);
                    int a = matrix[i].fade.size();
                    for(int j=1; j<a; j++){
                        matrix[i].fade.pop_back();
                    }
                    for(int k=0; k < matrix.size() - 1; k++){
                        if(k!=i && matrix[k].fade[0].getPosition().x == matrix[i].fade[0].getPosition().x){
                            //if(matrix[k].fade.size() > 1) matrix[k].fade.pop_back();
                            matrix[k].max_lenght = matrix[k].fade.size();
                        }
                    }
                    //for(int k=0; k<matrix.size(); k++){
                    //    if(k!=i && matrix[k].fade[0].getPosition().x == matrix[i].fade[0].getPosition().x){
                    //        matrix[k].max_lenght = matrix[k].fade.size();
                    //    }
                    //}
                } // if
                matrix[i].record_fall = record;
            }
            for(int j=0; j<matrix[i].fade.size(); j++){
                window.draw(matrix[i].fade[j]);
            }
        } // for
        window.display();
    } // gameLoop

    return 0;
}
