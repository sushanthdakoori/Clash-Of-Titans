#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;

struct Bullet {
    sf::RectangleShape shape;
};

struct Bomb {
    sf::CircleShape shape;
};

struct Plane {
    sf::RectangleShape shape;
    bool alive;
    int lastBomb;
};

int main() {
    srand(time(nullptr));

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Clash of Titans - SFML");
    window.setFramerateLimit(60);

    // Player gun
    sf::RectangleShape gun(sf::Vector2f(60, 10));
    gun.setFillColor(sf::Color::White);
    gun.setPosition(WIDTH / 2, HEIGHT - 40);

    // Score
    int score = 0;
    bool paused = false;

    sf::Font font;
    font.loadFromFile("arial.ttf");

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(18);
    scoreText.setFillColor(sf::Color::White);

    // Planes
    vector<Plane> planes;
    for (int i = 0; i < 8; i++) {
        Plane p;
        p.shape.setSize(sf::Vector2f(50, 15));
        p.shape.setFillColor(sf::Color::Red);
        p.shape.setPosition(i * 100, 50);
        p.alive = true;
        p.lastBomb = 0;
        planes.push_back(p);
    }

    vector<Bullet> bullets;
    vector<Bomb> bombs;

    sf::Clock clock;
    int tick = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P)
                paused = !paused;
        }

        if (paused) continue;

        // INPUT
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && gun.getPosition().x > 0)
            gun.move(-6, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
            gun.getPosition().x + gun.getSize().x < WIDTH)
            gun.move(6, 0);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            Bullet b;
            b.shape.setSize(sf::Vector2f(4, 10));
            b.shape.setFillColor(sf::Color::Green);
            b.shape.setPosition(gun.getPosition().x + 28, gun.getPosition().y);
            bullets.push_back(b);
        }

        // UPDATE PLANES
        for (auto &p : planes) {
            if (!p.alive) continue;

            p.shape.move(2, 0);
            if (p.shape.getPosition().x > WIDTH) {
                p.shape.setPosition(-50, 50);
                p.alive = true;
            }

            if (tick - p.lastBomb > 60 && rand() % 80 == 0) {
                Bomb bo;
                bo.shape.setRadius(5);
                bo.shape.setFillColor(sf::Color::Yellow);
                bo.shape.setPosition(
                    p.shape.getPosition().x + 20,
                    p.shape.getPosition().y + 15
                );
                bombs.push_back(bo);
                p.lastBomb = tick;
            }
        }

        // UPDATE BULLETS
        for (auto &b : bullets)
            b.shape.move(0, -8);

        bullets.erase(remove_if(bullets.begin(), bullets.end(),
            [](Bullet &b) {
                return b.shape.getPosition().y < 0;
            }), bullets.end());

        // UPDATE BOMBS
        for (auto &bo : bombs)
            bo.shape.move(0, 5);

        bombs.erase(remove_if(bombs.begin(), bombs.end(),
            [](Bomb &bo) {
                return bo.shape.getPosition().y > HEIGHT;
            }), bombs.end());

        // COLLISIONS
        for (auto &b : bullets) {
            for (auto &p : planes) {
                if (p.alive && b.shape.getGlobalBounds().intersects(p.shape.getGlobalBounds())) {
                    p.alive = false;
                    score++;
                }
            }
        }

        for (auto &bo : bombs) {
            if (bo.shape.getGlobalBounds().intersects(gun.getGlobalBounds())) {
                window.close(); // game over
            }
        }

        // DRAW
        window.clear(sf::Color::Black);

        for (auto &p : planes)
            if (p.alive) window.draw(p.shape);

        for (auto &b : bullets)
            window.draw(b.shape);

        for (auto &bo : bombs)
            window.draw(bo.shape);

        window.draw(gun);

        scoreText.setString("Score: " + to_string(score));
        scoreText.setPosition(10, 10);
        window.draw(scoreText);

        window.display();
        tick++;
    }

    return 0;
}
