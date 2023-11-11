#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <chrono>
#include <random>

std::mt19937 ran(time(0));

int irand(int a, int b) {
    return a + (ran() % (b - a + 1));
}

struct wall
{
    sf::Vector2f first;
    sf::Vector2f second;
    sf::VertexArray line;
    float charge;
};

struct table
{
    sf::Vector2f position;
    float charge;
    sf::Vector2f forceVector;
    sf::Vector2f Size;
    sf::RectangleShape DrawShape;
};

void initWalls(std::vector<wall>& walls, std::string x1, std::string y1, std::string x2, std::string y2)
{
    wall twall;
    twall.first.x = std::stoi(x1);
    twall.first.y = std::stoi(y1);
    twall.second.x = std::stoi(x2);
    twall.second.y = std::stoi(y2);
    twall.line.setPrimitiveType(sf::Lines);
    twall.line.resize(2);
    twall.line[0].position = sf::Vector2f(std::stoi(x1) + 280, std::stoi(y1) + 100);
    twall.line[1].position = sf::Vector2f(std::stoi(x2) + 280, std::stoi(y2) + 100);
    twall.line[0].color = sf::Color::Black;
    twall.line[1].color = sf::Color::Black;
    walls.push_back(twall);

}

//--------------------------------------------------------------------------------
// algoruthm
float GetLength(sf::Vector2f input) {
    return sqrt(input.x * input.x + input.y * input.y);
}

sf::Vector2f GetDirection(sf::Vector2f input) {
    float len = GetLength(input);
    input.x = input.x / len;
    input.y = input.y / len;
    return input;
}

sf::Vector2f GetForceVector(sf::Vector2f pos1/*current table*/, sf::Vector2f pos2/*from where force is*/, float charge1, float charge2) { // pos1 is current processing table
    sf::Vector2f direction;												// pos2 is an object, which influence we
    direction.x = pos1.x - pos2.x;									// are calculating
    direction.y = pos1.y - pos2.y;
    float distance = GetLength(direction);
    direction = GetDirection(direction);
    float force = charge1 * charge2 / (distance * distance);
    sf::Vector2f forceVector;
    forceVector.x = direction.x * force / 3;
    forceVector.y = direction.y * force / 3;
    return forceVector;
}

sf::Vector2f GetForceFromWalls(table Table, std::vector<wall>& walls) {
    sf::Vector2f upper, lower, left, right;
    float miny = 720, maxy = 0, minx = 1280, maxx = 0;
    for (int i = 0; i < walls.size(); i++) {
        if ((Table.position.x > walls[i].first.x && Table.position.x < walls[i].second.x) || (Table.position.x < walls[i].first.x && Table.position.x > walls[i].second.x)) {
            float Yw, Yv, Xw, Xv;
            if (walls[i].first.y >= walls[i].second.y) {
                Yw = walls[i].first.y;
                Yv = walls[i].second.y;
                Xw = walls[i].first.x;
                Xv = walls[i].second.x;
            }
            else {
                Yv = walls[i].first.y;
                Yw = walls[i].second.y;
                Xv = walls[i].first.x;
                Xw = walls[i].second.x;
            }
            float X = Table.position.x, Y = Table.position.y;
            float XwXv = fabs(Xw - Xv);
            float YwYv = Yw - Yv;
            float XvX = fabs(Xv - X);
            float YvY0 = (YwYv * XvX) / XwXv;
            float Y0 = Yv + YvY0;
            if (Y0 > Y) {
                if (Y0 < miny) {
                    miny = Y0;
                }
            }
            else {
                if (Y0 > maxy) {
                    maxy = Y0;
                }
            }
        }
        if ((Table.position.y > walls[i].first.y && Table.position.y < walls[i].second.y) || (Table.position.y < walls[i].first.y && Table.position.y > walls[i].second.y)) {
            float Yw, Yv, Xw, Xv;
            if (walls[i].first.x >= walls[i].second.x) {
                Yw = walls[i].first.y;
                Yv = walls[i].second.y;
                Xw = walls[i].first.x;
                Xv = walls[i].second.x;
            }
            else {
                Yv = walls[i].first.y;
                Yw = walls[i].second.y;
                Xv = walls[i].first.x;
                Xw = walls[i].second.x;
            }
            float X = Table.position.x, Y = Table.position.y;
            float YwYv = fabs(Yw - Yv);
            float XwXv = Xw - Xv;
            float YvY = fabs(Yv - Y);
            float XvX0 = (XwXv * YvY) / YwYv;
            float X0 = Xv + XvX0;
            if (X0 > X) {
                if (X0 < minx) {
                    minx = X0;
                }
            }
            else {
                if (X0 > maxx) {
                    maxx = X0;
                }
            }
        }
    }
    upper.x = Table.position.x;
    upper.y = maxy;
    lower.x = Table.position.x;
    lower.y = miny;
    left.x = maxx;
    left.y = Table.position.y;
    right.x = minx;
    right.y = Table.position.y;
    upper = GetForceVector(Table.position, upper, Table.charge, 10);
    lower = GetForceVector(Table.position, lower, Table.charge, 10);
    left = GetForceVector(Table.position, left, Table.charge, 10);
    right = GetForceVector(Table.position, right, Table.charge, 10);
    sf::Vector2f forceVector;
    forceVector.x = upper.x + lower.x + left.x + right.x;
    forceVector.y = upper.y + lower.y + left.y + right.y;
    return forceVector;
}

void AddTable(std::vector<wall>& walls, std::vector<table>& tables, int tableNum, int tableWidth, int tableLength, bool& stable) {
    if (tables.size() < tableNum) {
        table Da;
        Da.charge = 30;
        Da.Size.x = tableWidth;
        Da.Size.y = tableLength;
        float x = 0, y = 0;
        for (int i = 0; i < walls.size(); i++) {
            x += walls[i].first.x;
            y += walls[i].first.y;
            x += walls[i].second.x;
            y += walls[i].second.y;
        }
        x = x / walls.size() / 2;
        y = y / walls.size() / 2;
        Da.position.x = x + irand(-10, 10);
        Da.position.y = y + irand(-10, 10);
        tables.push_back(Da);
        stable = false;
    }
}
//-----------------------------------------------------------------------------

void menu1(sf::RenderWindow& window, std::vector<wall>& walls) {
    sf::RectangleShape button1(sf::Vector2f(200, 100));
    button1.setPosition(50, 120);
    button1.setFillColor(sf::Color::White);
    button1.setOutlineThickness(5.f);
    button1.setOutlineColor(sf::Color::Black);


    sf::RectangleShape button2(sf::Vector2f(200, 100));
    button2.setPosition(1000, 120);
    button2.setFillColor(sf::Color::White);
    button2.setOutlineThickness(5.f);
    button2.setOutlineColor(sf::Color::Black);

    sf::RectangleShape rect1(sf::Vector2f(200, 50));
    rect1.setPosition(50, 255);
    rect1.setFillColor(sf::Color::White);
    rect1.setOutlineColor(sf::Color::Black);
    rect1.setOutlineThickness(2);

    sf::RectangleShape rect2(sf::Vector2f(200, 50));
    rect2.setPosition(50, 310);
    rect2.setFillColor(sf::Color::White);
    rect2.setOutlineColor(sf::Color::Black);
    rect2.setOutlineThickness(2);

    sf::RectangleShape rect3(sf::Vector2f(200, 50));
    rect3.setPosition(50, 365);
    rect3.setFillColor(sf::Color::White);
    rect3.setOutlineColor(sf::Color::Black);
    rect3.setOutlineThickness(2);

    sf::RectangleShape rect4(sf::Vector2f(200, 50));
    rect4.setPosition(50, 420);
    rect4.setFillColor(sf::Color::White);
    rect4.setOutlineColor(sf::Color::Black);
    rect4.setOutlineThickness(2);

    sf::Texture textureBg;
    textureBg.loadFromFile("Texture/‘он.png");
    sf::RectangleShape Bg(sf::Vector2f(1280, 720));
    Bg.setTexture(&textureBg);

    sf::RectangleShape colorZone(sf::Vector2f(700, 550));
    colorZone.setPosition(280, 100);
    sf::Color color(0, 0, 0, 0);
    colorZone.setFillColor(sf::Color::White);
    colorZone.setOutlineColor(sf::Color::Black);
    colorZone.setOutlineThickness(4);

    sf::Font font;
    if (!font.loadFromFile("Font/Futurafuturiscondextraboldc (1).otf")) {
    }

    sf::Text textButtonOne("Add", font, 30);
    textButtonOne.setFillColor(sf::Color::Black);
    sf::FloatRect textButtonOneRect = textButtonOne.getLocalBounds();
    textButtonOne.setOrigin(textButtonOneRect.left + textButtonOneRect.width / 2, textButtonOneRect.top + textButtonOneRect.height / 2);
    textButtonOne.setPosition(button1.getPosition().x + button1.getSize().x / 2, button1.getPosition().y + button1.getSize().y / 2);

    sf::Text textButtonTwo("Continue", font, 30);
    textButtonTwo.setFillColor(sf::Color::Black);
    sf::FloatRect textButtonTwoRect = textButtonTwo.getLocalBounds();
    textButtonTwo.setOrigin(textButtonTwoRect.left + textButtonTwoRect.width / 2, textButtonTwoRect.top + textButtonTwoRect.height / 2);
    textButtonTwo.setPosition(button2.getPosition().x + button2.getSize().x / 2, button2.getPosition().y + button2.getSize().y / 2);


    sf::Text text1("", font, 20);
    text1.setPosition(rect1.getPosition().x + 40, rect1.getPosition().y + 10);
    text1.setFillColor(sf::Color::Black);

    sf::Text text2("", font, 20);
    text2.setPosition(rect2.getPosition().x + 40, rect2.getPosition().y + 10);
    text2.setFillColor(sf::Color::Black);

    sf::Text text3("", font, 20);
    text3.setPosition(rect3.getPosition().x + 40, rect3.getPosition().y + 10);
    text3.setFillColor(sf::Color::Black);

    sf::Text text4("", font, 20);
    text4.setPosition(rect4.getPosition().x + 40, rect4.getPosition().y + 10);
    text4.setFillColor(sf::Color::Black);


    sf::Text textv1("X1:", font, 20);
    textv1.setPosition(rect1.getPosition().x + 10, rect1.getPosition().y + 10);
    textv1.setFillColor(sf::Color::Black);

    sf::Text textv2("Y1:", font, 20);
    textv2.setPosition(rect2.getPosition().x + 10, rect2.getPosition().y + 10);
    textv2.setFillColor(sf::Color::Black);

    sf::Text textv3("X2:", font, 20);
    textv3.setPosition(rect3.getPosition().x + 10, rect3.getPosition().y + 10);
    textv3.setFillColor(sf::Color::Black);

    sf::Text textv4("Y2:", font, 20);
    textv4.setPosition(rect4.getPosition().x + 10, rect4.getPosition().y + 10);
    textv4.setFillColor(sf::Color::Black);

    bool isRect1Active = false;
    bool isRect2Active = false;
    bool isRect3Active = false;
    bool isRect4Active = false;

    sf::Clock clock1;
    sf::Clock clock2;

    bool isPressed1 = false;
    bool isPressed2 = false;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePos2 = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                    if (button1.getGlobalBounds().contains(mousePos2))
                    {
                        isPressed1 = true;
                        button1.setOutlineColor(sf::Color::Red);
                        clock1.restart();
                        initWalls(walls, text1.getString(), text2.getString(), text3.getString(), text4.getString());
                    }

                    if (button2.getGlobalBounds().contains(mousePos2))
                    {
                        isPressed2 = true;
                        button2.setOutlineColor(sf::Color::Red);
                        clock2.restart();
                        return;
                    }
                    if (rect1.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        isRect1Active = true;
                        isRect2Active = false;
                        isRect3Active = false;
                        isRect4Active = false;
                    }
                    else if (rect2.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        isRect2Active = true;
                        isRect1Active = false;
                        isRect3Active = false;
                        isRect4Active = false;
                    }
                    else if (rect3.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        isRect2Active = false;
                        isRect1Active = false;
                        isRect3Active = true;
                        isRect4Active = false;
                    }
                    else if (rect4.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        isRect1Active = false;
                        isRect2Active = false;
                        isRect3Active = false;
                        isRect4Active = true;
                    }
                    else {
                        isRect1Active = false;
                        isRect2Active = false;
                        isRect3Active = false;
                        isRect4Active = false;
                    }
                }
            }

            if (event.type == sf::Event::TextEntered) {
                if (isRect1Active) {
                    if (event.text.unicode == '\b' && text1.getString().getSize() > 0) {
                        std::string str = text1.getString();
                        str.erase(str.size() - 1, 1);
                        text1.setString(str);
                    }
                    else if (event.text.unicode < 128) {
                        text1.setString(text1.getString() + static_cast<char>(event.text.unicode));
                    }
                }

                if (isRect2Active) {
                    if (event.text.unicode == '\b' && text2.getString().getSize() > 0) {
                        std::string str = text2.getString();
                        str.erase(str.size() - 1, 1);
                        text2.setString(str);
                    }
                    else if (event.text.unicode < 128) {
                        text2.setString(text2.getString() + static_cast<char>(event.text.unicode));
                    }
                }

                if (isRect3Active) {
                    if (event.text.unicode == '\b' && text3.getString().getSize() > 0) {
                        std::string str = text3.getString();
                        str.erase(str.size() - 1, 1);
                        text3.setString(str);
                    }
                    else if (event.text.unicode < 128) {
                        text3.setString(text3.getString() + static_cast<char>(event.text.unicode));
                    }
                }
                if (isRect4Active) {
                    if (event.text.unicode == '\b' && text4.getString().getSize() > 0) {
                        std::string str = text4.getString();
                        str.erase(str.size() - 1, 1);
                        text4.setString(str);
                    }
                    else if (event.text.unicode < 128) {
                        text4.setString(text4.getString() + static_cast<char>(event.text.unicode));
                    }
                }
            }
        }

        window.clear();

        if (isRect1Active) {
            rect1.setOutlineColor(sf::Color::Red);
        }
        else {
            rect1.setOutlineColor(sf::Color::Black);
        }

        if (isRect2Active) {
            rect2.setOutlineColor(sf::Color::Red);
        }
        else {
            rect2.setOutlineColor(sf::Color::Black);
        }

        if (isRect3Active) {
            rect3.setOutlineColor(sf::Color::Red);
        }
        else {
            rect3.setOutlineColor(sf::Color::Black);
        }

        if (isRect4Active) {
            rect4.setOutlineColor(sf::Color::Red);
        }
        else {
            rect4.setOutlineColor(sf::Color::Black);
        }
        if (isPressed1 && clock1.getElapsedTime().asSeconds() >= 0.5f)
        {
            isPressed1 = false;
            button1.setOutlineColor(sf::Color::Black);
        }
        if (isPressed2 && clock2.getElapsedTime().asSeconds() >= 0.5f)
        {
            isPressed2 = false;
            button2.setOutlineColor(sf::Color::Black);
        }


        window.draw(Bg);
        window.draw(colorZone);
        window.draw(button1);
        window.draw(button2);
        window.draw(textButtonOne);
        window.draw(textButtonTwo);
        window.draw(textv1);
        window.draw(textv2);
        window.draw(textv3);
        window.draw(textv4);
        window.draw(rect1);
        window.draw(rect2);
        window.draw(rect3);
        window.draw(rect4);
        window.draw(text1);
        window.draw(text2);
        window.draw(text3);
        window.draw(text4);
        window.draw(textv1);
        window.draw(textv2);
        window.draw(textv3);
        window.draw(textv4);
        for (int i = 0; i < walls.size(); i++) {
            window.draw(walls[i].line);
        }
        window.display();
    }
}

void menu2(sf::RenderWindow& window, int& stableH, int& stableW, int& kTabl)
{
    sf::RectangleShape button1(sf::Vector2f(200, 100));
    button1.setPosition(550, 120);
    button1.setFillColor(sf::Color::White);
    button1.setOutlineThickness(5.f);
    button1.setOutlineColor(sf::Color::Black);

    sf::RectangleShape rect1(sf::Vector2f(200, 50));
    rect1.setPosition(550, 255);
    rect1.setFillColor(sf::Color::White);
    rect1.setOutlineColor(sf::Color::Black);
    rect1.setOutlineThickness(2);

    sf::RectangleShape rect2(sf::Vector2f(200, 50));
    rect2.setPosition(550, 310);
    rect2.setFillColor(sf::Color::White);
    rect2.setOutlineColor(sf::Color::Black);
    rect2.setOutlineThickness(2);

    sf::RectangleShape rect3(sf::Vector2f(200, 50));
    rect3.setPosition(550, 500);
    rect3.setFillColor(sf::Color::White);
    rect3.setOutlineColor(sf::Color::Black);
    rect3.setOutlineThickness(2);

    sf::RectangleShape rect4(sf::Vector2f(200, 60));
    rect4.setPosition(550, 450);
    rect4.setFillColor(sf::Color::White);
    rect4.setOutlineColor(sf::Color::Black);
    rect4.setOutlineThickness(2);

    sf::Texture textureBg;
    textureBg.loadFromFile("Texture/‘он.png");
    sf::RectangleShape Bg(sf::Vector2f(1280, 720));
    Bg.setTexture(&textureBg);

    sf::Font font;
    if (!font.loadFromFile("Font/Futurafuturiscondextraboldc (1).otf")) {
    }

    sf::Text textButtonOne("Continue", font, 30);
    textButtonOne.setFillColor(sf::Color::Black);
    sf::FloatRect textButtonOneRect = textButtonOne.getLocalBounds();
    textButtonOne.setOrigin(textButtonOneRect.left + textButtonOneRect.width / 2, textButtonOneRect.top + textButtonOneRect.height / 2);
    textButtonOne.setPosition(button1.getPosition().x + button1.getSize().x / 2, button1.getPosition().y + button1.getSize().y / 2);


    sf::Text text1("", font, 20);
    text1.setPosition(rect1.getPosition().x + 30, rect1.getPosition().y + 10);
    text1.setFillColor(sf::Color::Black);

    sf::Text text2("", font, 20);
    text2.setPosition(rect2.getPosition().x + 30, rect2.getPosition().y + 10);
    text2.setFillColor(sf::Color::Black);

    sf::Text text3("", font, 20);
    text3.setPosition(rect3.getPosition().x + 10, rect3.getPosition().y + 10);
    text3.setFillColor(sf::Color::Black);


    sf::Text textv1("X:", font, 20);
    textv1.setPosition(rect1.getPosition().x + 10, rect1.getPosition().y + 10);
    textv1.setFillColor(sf::Color::Black);

    sf::Text textv2("Y:", font, 20);
    textv2.setPosition(rect2.getPosition().x + 10, rect2.getPosition().y + 10);
    textv2.setFillColor(sf::Color::Black);

    sf::Text textv3("Tables number", font, 30);
    textv3.setFillColor(sf::Color::Black);
    sf::FloatRect textv3Rect = textv3.getLocalBounds();
    textv3.setOrigin(textv3Rect.left + textv3Rect.width / 2, textv3Rect.top + textv3Rect.height / 2);
    textv3.setPosition(rect4.getPosition().x + rect4.getSize().x / 2, rect4.getPosition().y + rect4.getSize().y / 2);


    bool isRect1Active = false;
    bool isRect2Active = false;
    bool isRect3Active = false;

    sf::Clock clock1;

    bool isPressed1 = false;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePos2 = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                    if (button1.getGlobalBounds().contains(mousePos2))
                    {
                        isPressed1 = true;
                        button1.setOutlineColor(sf::Color::Red);
                        clock1.restart();
                        std::string sH = text1.getString();
                        std::string sW = text2.getString();
                        std::string kT = text3.getString();
                        stableH = std::stoi(sH);
                        stableW = std::stoi(sW);
                        kTabl = std::stoi(kT);
                        return;
                    }

                    if (rect1.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        isRect1Active = true;
                        isRect2Active = false;
                        isRect3Active = false;
                    }
                    else if (rect2.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        isRect2Active = true;
                        isRect1Active = false;
                        isRect3Active = false;
                    }
                    else if (rect3.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        isRect2Active = false;
                        isRect1Active = false;
                        isRect3Active = true;
                    }
                    else {
                        isRect1Active = false;
                        isRect2Active = false;
                        isRect3Active = false;
                    }
                }
            }

            if (event.type == sf::Event::TextEntered) {
                if (isRect1Active) {
                    if (event.text.unicode == '\b' && text1.getString().getSize() > 0) {
                        std::string str = text1.getString();
                        str.erase(str.size() - 1, 1);
                        text1.setString(str);
                    }
                    else if (event.text.unicode < 128) {
                        text1.setString(text1.getString() + static_cast<char>(event.text.unicode));
                    }
                }

                if (isRect2Active) {
                    if (event.text.unicode == '\b' && text2.getString().getSize() > 0) {
                        std::string str = text2.getString();
                        str.erase(str.size() - 1, 1);
                        text2.setString(str);
                    }
                    else if (event.text.unicode < 128) {
                        text2.setString(text2.getString() + static_cast<char>(event.text.unicode));
                    }
                }

                if (isRect3Active) {
                    if (event.text.unicode == '\b' && text3.getString().getSize() > 0) {
                        std::string str = text3.getString();
                        str.erase(str.size() - 1, 1);
                        text3.setString(str);
                    }
                    else if (event.text.unicode < 128) {
                        text3.setString(text3.getString() + static_cast<char>(event.text.unicode));
                    }
                }

            }
        }

        window.clear();

        if (isRect1Active) {
            rect1.setOutlineColor(sf::Color::Red);
        }
        else {
            rect1.setOutlineColor(sf::Color::Black);
        }

        if (isRect2Active) {
            rect2.setOutlineColor(sf::Color::Red);
        }
        else {
            rect2.setOutlineColor(sf::Color::Black);
        }

        if (isRect3Active) {
            rect3.setOutlineColor(sf::Color::Red);
        }
        else {
            rect3.setOutlineColor(sf::Color::Black);
        }

        if (isPressed1 && clock1.getElapsedTime().asSeconds() >= 0.5f)
        {
            isPressed1 = false;
            button1.setOutlineColor(sf::Color::Black);
        }

        window.draw(Bg);
        window.draw(button1);
        window.draw(textButtonOne);
        window.draw(rect1);
        window.draw(rect2);
        window.draw(rect3);
        window.draw(rect4);
        window.draw(text1);
        window.draw(text2);
        window.draw(text3);
        window.draw(textv1);
        window.draw(textv2);
        window.draw(textv3);

        window.display();
    }

}

void menu3(sf::RenderWindow& window, std::vector<wall>& walls, std::vector<table>& tables, int tableWidth, int tableLength, int tableNum)
{
    sf::Font font;
    if (!font.loadFromFile("Font/Futurafuturiscondextraboldc (1).otf")) {
    }

    sf::Text textButtonOne("Loading...", font, 30);
    textButtonOne.setFillColor(sf::Color::White);
    sf::FloatRect textButtonOneRect = textButtonOne.getLocalBounds();
    textButtonOne.setOrigin(textButtonOneRect.left + textButtonOneRect.width / 2, textButtonOneRect.top + textButtonOneRect.height / 2);
    textButtonOne.setPosition(640, 320);

    bool stable = true;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        AddTable(walls, tables, tableNum, tableWidth, tableLength, stable);
        while (!stable) {
            //------------------------------------------------------------------------------
            //calculating forces
            //------------------------------------------------------------------------------
            for (int i = 0; i < tables.size(); i++) {
                sf::Vector2f forceVector = GetForceFromWalls(tables[i], walls);
                for (int g = 0; g < tables.size(); g++) {
                    if (g != i) {
                        sf::Vector2f v = GetForceVector(tables[i].position, tables[g].position, tables[i].charge, tables[g].charge);
                        forceVector.x += v.x;
                        forceVector.y += v.y;
                    }
                }
                tables[i].forceVector = forceVector;
            }
            //------------------------------------------------------------------------------
            //checking if the sistem is stable
            //------------------------------------------------------------------------------
            bool flag = true;
            for (int i = 0; i < tables.size(); i++) {
                if (GetLength(tables[i].forceVector) > 0.005) {
                    flag = false;
                    break;
                }
            }
            //------------------------------------------------------------------------------
            //if the sistem is unstable, move objects, else end cycle
            //------------------------------------------------------------------------------
            if (flag) {
                stable = true;
                break;
            }
            else {
                for (int i = 0; i < tables.size(); i++) {
                    tables[i].position.x += tables[i].forceVector.x;
                    tables[i].position.y += tables[i].forceVector.y;
                    tables[i].forceVector.x = 0;
                    tables[i].forceVector.y = 0;
                }
            }
        }

        if (tables.size() == tableNum) { // вот здес€ надо объ€вл€ть параметры классов пр€моугольников(столов)
            for (int i = 0; i < tables.size(); i++) {
                tables[i].DrawShape.setSize(tables[i].Size);
                tables[i].DrawShape.setPosition(sf::Vector2f(tables[i].position.x - tables[i].Size.x / 2, tables[i].position.y - tables[i].Size.y / 2));
                tables[i].DrawShape.setFillColor(sf::Color::Yellow);

            }
            return;// вот здес€ надо ретюрн; поставить штоб после окончани€ работы алгоритма окно загрузки переключилось на следующее окно
        }

        window.clear();
        window.draw(textButtonOne);
        window.display();
    }

}

void menu4(sf::RenderWindow& window, std::vector<wall>& walls, std::vector<table>& tables)
{
    sf::Font font;
    if (!font.loadFromFile("Font/Futurafuturiscondextraboldc (1).otf")) {
    }

    sf::Texture textureBg;
    textureBg.loadFromFile("Texture/‘он.png");
    sf::RectangleShape Bg(sf::Vector2f(1280, 720));
    Bg.setFillColor(sf::Color::White);

    for (int i = 0; i < walls.size(); i++) {
        walls[i].line[0].position.x -= 280;
        walls[i].line[0].position.y -= 100;
        walls[i].line[1].position.x -= 280;
        walls[i].line[1].position.y -= 100;
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(Bg);
        for (int i = 0; i < walls.size(); i++) {
            window.draw(walls[i].line);
        }
        for (int i=0;i<tables.size();i++)
        {
            window.draw(tables[i].DrawShape);
        }
        window.display();
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Stol stul");
    std::vector<wall> walls;
    std::vector<table> tables;
    int stableH;
    int stableW;
    int kTabl;
    menu1(window, walls);
    menu2(window, stableH, stableW, kTabl);
    menu3(window, walls, tables, stableH, stableW, kTabl);
    menu4(window, walls, tables);
}