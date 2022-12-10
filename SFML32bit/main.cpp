#include <SFML/Graphics.hpp>
#include<string>
#include<vector>
#include<cmath>
#include<iostream>
using namespace std;
using namespace sf;

#define shootVelocity 9

struct aim {
    CircleShape circle;
    float velY;
    int finishTime;
};
typedef struct aim aim;

void randomWord(string*, int); 
void calc(aim*, Vector2f, int, float, int);

int main() {
    float velocity = -2;     // 0-тэй дөхөх тусам удаан болно, эерэг утга авахгүй
    int interval = 60;       // 1 секунд тутамд шинэ үг гарч ирнэ 
    int wordLengthLimit = 7; // Үг хамгийн ихдээ wordLengthLimit үсэгтэй, хамгийн богинодоо 3 үсэгтэй байна

    int width = 1280, height = 720;
    int currentWord = -1;
    static int idx = 1;      // Үгийг хэд дэх үсэг хүртэл зөв бичсэн байгааг хадгална
    int scene = 1;
    int t = 0;               // timer
    srand(time(0));

    // <background, үг> хадгалах vector бүтэц, сум хадгалах set
    vector<pair<RectangleShape, Text>> container;
    vector<pair<RectangleShape, Text>>::iterator it;
    vector<aim> set;
    vector<aim>::iterator itAim;

    // Үг дэвсгэр
    RectangleShape box(Vector2f(100, 30));
    box.setFillColor(Color::Black);
    box.setOutlineThickness(1);
    box.setOutlineColor(Color::Black);

    // Улаан шугам
    RectangleShape redLine(Vector2f(2, height));
    redLine.setPosition(100, 0);
    redLine.setFillColor(Color::Red);

    // Бичих текстүүд
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

    // Хожигдоход гарах үг
    Text instruction;
    instruction.setFont(font);
    instruction.setCharacterSize(50);
    instruction.setFillColor(Color{101, 179, 122});
    instruction.setString("press SPACE to play again");
    instruction.setPosition(width/2 - instruction.getGlobalBounds().width / 2, height/2 - instruction.getGlobalBounds().height);

    // Текст арилгах 
    RectangleShape block(Vector2f(0, 30));
    box.setFillColor(Color::Black);

    // Сум
    aim shoot;
    shoot.circle.setRadius(5);
    shoot.circle.setFillColor(Color::White);
    shoot.circle.setPosition(Vector2f(50, height / 2));

    // Background texture
    Texture texture;
    if (!texture.loadFromFile("space.png")) {
        cout << "Background texture file unshihad aldaa garlaa\n";
        exit(-1);
    }
    texture.setRepeated(true);
    Sprite sprite;
    sprite.setTexture(texture);
    sprite.setTextureRect(IntRect(0, 0, width, height));

    // Тоглогч
    Texture player;
    if (!player.loadFromFile("spaceship.png")) {
        cout << "Playeriin texture file unshihad aldaa garlaa\n";
        exit(-1);
    }
    Sprite spritePlayer(player);
    spritePlayer.setScale(Vector2f(50/ spritePlayer.getLocalBounds().width, 50 / spritePlayer.getLocalBounds().width));
    spritePlayer.setPosition(0, height/2- spritePlayer.getGlobalBounds().width/2);

    // Цонх нээх
    RenderWindow window(VideoMode(width, height), "Game1"); 
    window.setFramerateLimit(60); // frame rate limit -> 60fps

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (Keyboard::isKeyPressed(Keyboard::Escape))
                window.close();
            if (event.type == Event::Resized){
                width = event.size.width;
                height = event.size.height;
            }
            if (scene == 1) {
                if (event.type == Event::TextEntered) {
                    // Аль ч үгийг оноогүй байгаа бол
                    if (currentWord == -1) {
                        for (int i = 0; i < container.size(); i++) {
                            // Үг бүрийн эхний үсэг мөн эсэхийг шалгах
                            // Ижилхэн үсгээр эхэлсэн үг байвал хамгийн урд байгааг нь авна
                            if (event.text.unicode == container[i].second.getString()[0]) {
                                currentWord = i;
                                container[i].first.setOutlineColor(Color::White);
                                block.setSize(Vector2f(17.5, 30)); // Эхний үсгийг таглах
                                block.setPosition(container[i].first.getPosition());
                                //block.setFillColor(Color::Black);
                                calc(&shoot, container[i].first.getPosition(), t, velocity, height);
                                set.push_back(shoot);
                                break;
                            }
                        }
                    }
                    // Аль нэг үгийг оносон байгаа
                    else {
                        if (event.text.unicode == container[currentWord].second.getString()[idx]) {
                            calc(&shoot, container[currentWord].first.getPosition(), t, velocity, height);
                            set.push_back(shoot);
                            block.setSize(Vector2f(17.5 * (idx + 1), 30)); // idx+1 ширхэг үсгийг таглах
                            idx++;
                            // Бүх үсэг бичиж дуусвал тэр үгийг устгах
                            if (idx == container[currentWord].second.getString().getSize()) {
                                it = container.begin() + currentWord;
                                container.erase(it);
                                currentWord = -1;
                                idx = 1;
                                block.setSize(Vector2f(0, 30));
                            }

                        }
                    }
                }
            }
            else if (scene == 2) {
                if (event.type == Event::KeyPressed)
                    if (event.key.code == Keyboard::Space) {
                        scene = 1;
                        //window.clear();
                    }

            }
        }

        // Тоглож байгаа бол
        if (scene == 1) {
            // Velocity-р шилжих
            for (int i = 0; i < container.size(); i++) {
                container[i].first.setPosition(container[i].first.getPosition().x + velocity, container[i].first.getPosition().y);
                container[i].second.setPosition(container[i].second.getPosition().x + velocity, container[i].second.getPosition().y);
            }
            for (int i = 0; i < set.size(); i++)
                set[i].circle.setPosition(set[i].circle.getPosition().x + (shootVelocity), set[i].circle.getPosition().y + set[i].velY);
            block.setPosition(block.getPosition().x + velocity, block.getPosition().y);

            // interval/fps секунд тутамд шинэ үг гарч ирнэ / fps=60 /
            if (t % interval == 0) {
                int randomHeight = rand() % (height - 30);
                int wordLength = rand() % (wordLengthLimit - 2) + 3; // Үгийн урт 3-10 хооронд
                string tmp;
                randomWord(&tmp, wordLength);
                box.setSize(Vector2f(17.5 * wordLength, 30));
                box.setPosition(width, randomHeight);
                text.setPosition(width + 3, randomHeight);
                text.setString(tmp);
                container.push_back(make_pair(box, text));
            }

            // Улаан шугаманд хүрвэл scene солигдоно
            if (container.size() > 0 && container[0].first.getPosition().x <= 100) {
                scene++;
                currentWord = -1;
                container.clear();
                set.clear();
                block.setSize(Vector2f(0, 30));
                idx = 1;
            }
        }

        if (scene == 1){
            window.clear(Color::Black);
            window.draw(sprite);
            window.draw(spritePlayer);
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
            window.draw(block);
            window.display();
        }

        // Тоглоом зогссон бол
        else if (scene == 2) {
            window.setTitle("Game Over!");
            window.clear(Color{53,53,53});
            window.draw(instruction);
            window.display();
        }

        // Хугацааг нэг frame зурах бүрт нэмэгдүүлэх
        t++;
    }

    return 0;
}

void randomWord(string* ptr, int length) {
    for (int i = 0; i < length; i++)
        *ptr += 'a' + rand() % 26;
}

// Зөв үсэг дарагдахад шинэ сумны хурдны чиг, утга, зурагдах хугацааг тооцож хадгална
void calc(aim* b, Vector2f pos, int t, float velocity, int height) {
    b->finishTime = (pos.x - 50) / ((shootVelocity)+velocity * (-1));
    b->velY = (height / 2 - pos.y) * (-1) / b->finishTime;
    b->finishTime += t;
}