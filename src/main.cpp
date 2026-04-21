#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

constexpr unsigned int WIDTH = 800;
constexpr unsigned int HEIGHT = 600;

struct PipePair {
    sf::Sprite top;
    sf::Sprite bottom;
};

int main() {
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Flappy Bird");
    window.setFramerateLimit(60);

    sf::Texture birdTexture;
    sf::Texture pipeTexture;
    sf::Texture bgTexture;

    if (!birdTexture.loadFromFile("assets/bird.png")) {
        std::cerr << "Failed to load assets/bird.png\n";
        return 1;
    }

    if (!pipeTexture.loadFromFile("assets/pipe.png")) {
        std::cerr << "Failed to load assets/pipe.png\n";
        return 1;
    }

    if (!bgTexture.loadFromFile("assets/background.png")) {
        std::cerr << "Failed to load assets/background.png\n";
        return 1;
    }

    sf::Sprite background(bgTexture);
    background.setPosition({0.f, 0.f});
    background.setScale({
        static_cast<float>(WIDTH) / static_cast<float>(bgTexture.getSize().x),
        static_cast<float>(HEIGHT) / static_cast<float>(bgTexture.getSize().y)
    });

    sf::Sprite bird(birdTexture);
    const auto birdBounds = bird.getGlobalBounds();
    bird.setPosition({
        static_cast<float>(WIDTH) / 2.f - birdBounds.size.x / 2.f,
        static_cast<float>(HEIGHT) / 2.f - birdBounds.size.y / 2.f
    });

    float birdVelocity = 0.f;
    constexpr float GRAVITY = 900.f;
    constexpr float JUMP_STRENGTH = -350.f;
    constexpr float PIPE_GAP = 170.f;
    constexpr float PIPE_SPEED = 220.f;
    constexpr float PIPE_SPAWN_INTERVAL = 1.5f;

    std::vector<PipePair> pipes;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    sf::Clock deltaClock;
    sf::Clock pipeSpawnClock;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    birdVelocity = JUMP_STRENGTH;
                }
            }
        }

        const float dt = deltaClock.restart().asSeconds();

        birdVelocity += GRAVITY * dt;
        bird.move({0.f, birdVelocity * dt});

        if (pipeSpawnClock.getElapsedTime().asSeconds() >= PIPE_SPAWN_INTERVAL) {
            pipeSpawnClock.restart();

            PipePair pair{sf::Sprite(pipeTexture), sf::Sprite(pipeTexture)};

            const float minGapCenter = 180.f;
            const float maxGapCenter = static_cast<float>(HEIGHT) - 180.f;
            const float random01 = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
            const float gapCenterY = minGapCenter + random01 * (maxGapCenter - minGapCenter);
            const float pipeX = static_cast<float>(WIDTH);

            pair.top.setOrigin({0.f, static_cast<float>(pipeTexture.getSize().y)});
            pair.top.setScale({1.f, -1.f});
            pair.top.setPosition({pipeX, gapCenterY - PIPE_GAP / 2.f});

            pair.bottom.setPosition({pipeX, gapCenterY + PIPE_GAP / 2.f});

            pipes.push_back(pair);
        }

        for (std::size_t i = 0; i < pipes.size();) {
            pipes[i].top.move({-PIPE_SPEED * dt, 0.f});
            pipes[i].bottom.move({-PIPE_SPEED * dt, 0.f});

            if (bird.getGlobalBounds().findIntersection(pipes[i].top.getGlobalBounds()) ||
                bird.getGlobalBounds().findIntersection(pipes[i].bottom.getGlobalBounds())) {
                std::cout << "Game Over!\n";
                window.close();
                break;
            }

            if (pipes[i].top.getPosition().x + pipes[i].top.getGlobalBounds().size.x < 0.f) {
                pipes.erase(pipes.begin() + static_cast<std::ptrdiff_t>(i));
            } else {
                ++i;
            }
        }

        const auto currentBirdBounds = bird.getGlobalBounds();
        if (bird.getPosition().y < 0.f ||
            bird.getPosition().y + currentBirdBounds.size.y > static_cast<float>(HEIGHT)) {
            std::cout << "Game Over!\n";
            window.close();
        }

        window.clear(sf::Color::White);
        window.draw(background);

        for (const auto& pipe : pipes) {
            window.draw(pipe.top);
            window.draw(pipe.bottom);
        }

        window.draw(bird);
        window.display();
    }

    return 0;
}
