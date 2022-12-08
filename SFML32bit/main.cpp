#include <SFML/Graphics.hpp>
#include<string>
#include<vector>
#include<cmath>
#include<iostream>
using namespace std;
using namespace sf;

float velocity = -2;     // 0-тэй дөхөх тусам удаан болно, эерэг утга авахгүй
float shootVelocity = 9; // X
int t = 0;               // timer
int width = 1280, height = 720;

struct aim {
    CircleShape circle;
    float velY;
    int finishTime;
};
typedef struct aim aim;

void randomWord(string *ptr, int length) {
    for (int i = 0; i < length; i++)
        *ptr += 'a' + rand() % 26;
}

// Зөв үсэг дарагдахад шинэ сумны хурдны чиг, утга, зурагдах хугацааг тооцож хадгална 
void calc(aim* b, Vector2f pos) {
    b->finishTime = (pos.x - 50) / (shootVelocity + velocity * (-1));
    b->velY = (height/2 - pos.y)*(-1) / b->finishTime;
    b->finishTime += t;
}

int main(){
    int interval = 60;       // 1 секунд тутамд шинэ үг гарч ирнэ 
    int wordLengthLimit = 7; // Үг хамгийн ихдээ wordLengthLimit үсэгтэй, хамгийн богинодоо 3 үсэгтэй байна
    srand(time(0));
    int currentWord = -1;
    // box.height -> 30;

    // <background, үг> хадгалах vector бүтэц
    vector<pair<RectangleShape, Text>> container;
    vector<pair<RectangleShape, Text>>::iterator it;
    vector<aim> set;
    vector<aim>::iterator itAim;

    // Тоглогч
    CircleShape triangle(50, 3);
    Vector2f trianglePosition(50, height/2);
    triangle.setPosition(trianglePosition);
    triangle.setFillColor(Color::Yellow);
    triangle.rotate(90);

    // Үг дэвсгэр
    RectangleShape box(Vector2f(100, 30));
    box.setFillColor(Color::Black);
    box.setOutlineThickness(1);
    box.setOutlineColor(Color::Black);

    // Улаан шугам
    RectangleShape redLine(Vector2f(2, height));
    redLine.setPosition(100, 0);
    redLine.setFillColor(Color::Red);

    // Текст
    Font font;
    if (!font.loadFromFile("Livemonodemo.otf")) {
        cout << "font file unshihad aldaa garlaa\n";
        exit(-1);
    }
    Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(Color{ 0, 143, 17 }); // #008F11
    text.setStyle(Text::Bold);

    // Текст арилгах 
    RectangleShape tmp(Vector2f(0, 30));
    box.setFillColor(Color::Black);

    // Арын texture
    Texture texture;
    if (!texture.loadFromFile("texture.png")) {
        cout << "texture file unshihad aldaa garlaa\n";
        exit(-1);
    }
    texture.setRepeated(true);
    Sprite sprite;
    sprite.setTexture(texture);
    sprite.setTextureRect(IntRect(0,0,width,height));

    // Сум
    aim shoot;
    shoot.circle.setRadius(5);
    shoot.circle.setFillColor(Color::White);
    shoot.circle.setPosition(Vector2f(50, height/2));

    // Цонх нээх
    RenderWindow window(VideoMode(width, height), "Game1");
    // frame rate limit -> 60fps
    window.setFramerateLimit(60);

    while (window.isOpen()){
        Event event;
        // Event шалгах
        while (window.pollEvent(event)){
            if (event.type == Event::Closed) 
                window.close();
            if (Keyboard::isKeyPressed(Keyboard::Escape)) 
                window.close();
            //if (event.type == Event::Resized)
            //{
            //    event.size.width;
            //    event.size.height;
            //}

            if (event.type == Event::TextEntered){
                // Аль ч үгийг оноогүй байгаа бол
                if (currentWord == -1) {
                    for (int i = 0; i < container.size(); i++) {
                        // Үг бүрийн эхний үсэг мөн эсэхийг шалгах
                        // Ижилхэн үсгээр эхэлсэн үг байвал хамгийн урд байгааг нь авна
                        if (event.text.unicode == container[i].second.getString()[0]) {
                            currentWord = i;
                            container[i].first.setOutlineColor(Color::White);
                            tmp.setSize(Vector2f(17.5, 30)); // Эхний үсгийг таглах
                            tmp.setPosition(container[i].first.getPosition());
                            //tmp.setFillColor(Color::Black);
                            calc(&shoot, container[i].first.getPosition());
                            set.push_back(shoot);
                            break;
                        }
                    }
                }
                // Аль нэг үгийг оносон байгаа
                else {
                    static int idx = 1; // Үгийг хэд дэх үсэг хүртэл зөв бичсэн байгааг хадгална
                    if (event.text.unicode == container[currentWord].second.getString()[idx]) {
                        calc(&shoot, container[currentWord].first.getPosition());
                        set.push_back(shoot);
                        tmp.setSize(Vector2f(17.5*(idx+1), 30)); // idx+1 ширхэг үсгийг таглах
                        idx++;
                        // Бүх үсэг бичиж дуусвал тэр үгийг устгах
                        if (idx == container[currentWord].second.getString().getSize()) {
                            it = container.begin() + currentWord;
                            container.erase(it);
                            currentWord = -1;
                            idx = 1;
                            tmp.setSize(Vector2f(0, 30));
                        }

                    }
                }
            }
        }

        // Улаан шугаманд хүрвэл хожигдоно
        if (container.size() > 0 && container[0].first.getPosition().x <= 100) {
            exit(-1);
        }

        // Velocity-р шилжих
        for (int i = 0; i < container.size(); i++) {
            container[i].first.setPosition(container[i].first.getPosition().x + velocity, container[i].first.getPosition().y);
            container[i].second.setPosition(container[i].second.getPosition().x + velocity, container[i].second.getPosition().y);
        }
        for (int i = 0; i < set.size(); i++)
            set[i].circle.setPosition(set[i].circle.getPosition().x + shootVelocity, set[i].circle.getPosition().y + set[i].velY);
        tmp.setPosition(tmp.getPosition().x + velocity, tmp.getPosition().y);

        // interval/fps секунд тутамд шинэ үг гарч ирнэ / fps=60 /
        if (t % interval == 0) {
            int randomHeight = rand() % (height - 30);
            int wordLength = rand() % (wordLengthLimit-2)+3; // Үгийн урт 3-10 хооронд
            string tmp;
            randomWord(&tmp, wordLength);
            box.setSize(Vector2f(17.5*wordLength,30));
            box.setPosition(width, randomHeight);
            text.setPosition(width+3, randomHeight);
            text.setString(tmp);
            container.push_back(make_pair(box,text));
        }

        window.clear(Color::Black);
        window.draw(sprite);

        window.draw(triangle);
        window.draw(redLine);
        for (int i = 0; i < set.size(); i++) {
            // Сум тооцоолсон хугацаанд хүрсэн бол устгана
            if (set[i].finishTime <= t) {
                itAim = set.begin() + i;
                set.erase(itAim);
            }
            // Үгүй бол зурна
            else
                window.draw(set[i].circle);
        }
        // Эхэлж background box-г нь зурна!
        for (int i = 0; i < container.size(); i++) {
            window.draw(container[i].first);
            window.draw(container[i].second);
        }
        window.draw(tmp);
        window.display();

        // Хугацааг нэг frame зурах бүрт нэмэгдүүлэх
        t++;
    }

    return 0;
}