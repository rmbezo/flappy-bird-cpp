#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

constexpr unsigned int WIDTH = 800;
constexpr unsigned int HEIGHT = 600;

constexpr float BIRD_X = 220.f;
constexpr float READY_BIRD_Y = 260.f;
constexpr float GRAVITY = 1100.f;
constexpr float JUMP_STRENGTH = -380.f;
constexpr float PIPE_GAP = 175.f;
constexpr float PIPE_SPEED = 240.f;
constexpr float PIPE_SPAWN_INTERVAL = 1.45f;
constexpr float MIN_GAP_CENTER_Y = 175.f;
constexpr float MAX_GAP_CENTER_Y = HEIGHT - 175.f;

enum class GameState {
    Ready,
    Playing,
    GameOver
};

struct PipePair {
    sf::Sprite top;
    sf::Sprite bottom;
    bool scored = false;
};

float randomRange(float min, float max) {
    const float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return min + t * (max - min);
}

float clamp(float value, float min, float max) {
    return std::max(min, std::min(value, max));
}

void resetBird(sf::Sprite& bird, float& birdVelocity) {
    const auto bounds = bird.getGlobalBounds();
    bird.setPosition({BIRD_X, READY_BIRD_Y - bounds.size.y / 2.f});
    bird.setRotation(sf::degrees(0.f));
    birdVelocity = 0.f;
}

void resetGame(std::vector<PipePair>& pipes,
               sf::Sprite& bird,
               float& birdVelocity,
               int& score,
               GameState& gameState,
               sf::Clock& pipeSpawnClock,
               sf::Clock& animationClock) {
    pipes.clear();
    resetBird(bird, birdVelocity);
    score = 0;
    gameState = GameState::Ready;
    pipeSpawnClock.restart();
    animationClock.restart();
}

void spawnPipe(std::vector<PipePair>& pipes, const sf::Texture& pipeTexture) {
  if (pipeSpawnClock.getElapsedTime().asSeconds() >= PIPE_SPAWN_INTERVAL) {
    pipeSpawnClock.restart();

    PipePair pair{sf::Sprite(pipeTexture), sf::Sprite(pipeTexture)};

    const float minGapCenter = 180.f;
    const float maxGapCenter = static_cast<float>(HEIGHT) - 180.f;
    const float random01 = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    const float gapCenterY = minGapCenter + random01 * (maxGapCenter - minGapCenter);
    const float pipeX = static_cast<float>(WIDTH);

    const float gapTop = gapCenterY - PIPE_GAP / 2.f;
    const float gapBottom = gapCenterY + PIPE_GAP / 2.f;

    pair.top.setScale({1.f, -1.f});
    pair.top.setPosition({pipeX, gapTop});

    pair.bottom.setPosition({pipeX, gapBottom});

    pipes.push_back(pair);
  }
}
    PipePair pipePair{sf::Sprite(pipeTexture), sf::Sprite(pipeTexture), false};

    const float gapCenterY = randomRange(MIN_GAP_CENTER_Y, MAX_GAP_CENTER_Y);
    const float pipeX = static_cast<float>(WIDTH) + 40.f;

    const float gapTop = gapCenterY - PIPE_GAP / 2.f;
    const float gapBottom = gapCenterY + PIPE_GAP / 2.f;

    pipePair.top.setScale({1.f, -1.f});
    pipePair.top.setPosition({pipeX, gapTop});

    pipePair.bottom.setPosition({pipeX, gapBottom});

    pipes.push_back(pipePair);
}

bool isCollidingWithPipes(const sf::Sprite& bird, const std::vector<PipePair>& pipes) {
    const auto birdBounds = bird.getGlobalBounds();

    for (const auto& pipe : pipes) {
        if (birdBounds.findIntersection(pipe.top.getGlobalBounds()) ||
            birdBounds.findIntersection(pipe.bottom.getGlobalBounds())) {
            return true;
        }
    }

    return false;
}

bool isOutOfBounds(const sf::Sprite& bird) {
    const auto bounds = bird.getGlobalBounds();
    return bounds.position.y < 0.f ||
           bounds.position.y + bounds.size.y > static_cast<float>(HEIGHT);
}

void updateWindowTitle(sf::RenderWindow& window, GameState gameState, int score, int bestScore) {
    std::string title = "Flappy Bird";

    if (gameState == GameState::Ready) {
        title += " | Press Space to start";
    } else if (gameState == GameState::Playing) {
        title += " | Score: " + std::to_string(score) + " | Best: " + std::to_string(bestScore);
    } else {
        title += " | Game Over | Score: " + std::to_string(score) +
                 " | Best: " + std::to_string(bestScore) +
                 " | Press Space to restart";
    }

    window.setTitle(title);
}

int main() {
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Flappy Bird");
    window.setFramerateLimit(60);

    sf::Texture birdTexture;
    sf::Texture pipeTexture;
    sf::Texture backgroundTexture;

    if (!birdTexture.loadFromFile("assets/bird.png")) {
        std::cerr << "Failed to load assets/bird.png\n";
        return 1;
    }

    if (!pipeTexture.loadFromFile("assets/pipe.png")) {
        std::cerr << "Failed to load assets/pipe.png\n";
        return 1;
    }

    if (!backgroundTexture.loadFromFile("assets/background.png")) {
        std::cerr << "Failed to load assets/background.png\n";
        return 1;
    }

    sf::Sprite background(backgroundTexture);
    background.setPosition({0.f, 0.f});
    background.setScale({
        static_cast<float>(WIDTH) / static_cast<float>(backgroundTexture.getSize().x),
        static_cast<float>(HEIGHT) / static_cast<float>(backgroundTexture.getSize().y)
    });

    sf::Sprite bird(birdTexture);

    float birdVelocity = 0.f;
    int score = 0;
    int bestScore = 0;
    GameState gameState = GameState::Ready;

    std::vector<PipePair> pipes;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    sf::Clock deltaClock;
    sf::Clock pipeSpawnClock;
    sf::Clock animationClock;

    resetGame(pipes, bird, birdVelocity, score, gameState, pipeSpawnClock, animationClock);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }

                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    if (gameState == GameState::Ready) {
                        gameState = GameState::Playing;
                        birdVelocity = JUMP_STRENGTH;
                        pipeSpawnClock.restart();
                    } else if (gameState == GameState::Playing) {
                        birdVelocity = JUMP_STRENGTH;
                    } else if (gameState == GameState::GameOver) {
                        resetGame(pipes, bird, birdVelocity, score, gameState, pipeSpawnClock, animationClock);
                    }
                }
            }
        }

        const float dt = std::min(deltaClock.restart().asSeconds(), 0.02f);

        if (gameState == GameState::Ready) {
            const float t = animationClock.getElapsedTime().asSeconds();
            const auto bounds = bird.getGlobalBounds();
            const float bobOffset = std::sin(t * 3.0f) * 10.f;
            bird.setPosition({BIRD_X, READY_BIRD_Y - bounds.size.y / 2.f + bobOffset});
            bird.setRotation(sf::degrees(-8.f));
        } else if (gameState == GameState::Playing) {
            birdVelocity += GRAVITY * dt;
            bird.move({0.f, birdVelocity * dt});

            const float rotation = clamp(-25.f + (birdVelocity / 500.f) * 55.f, -30.f, 85.f);
            bird.setRotation(sf::degrees(rotation));

            if (pipeSpawnClock.getElapsedTime().asSeconds() >= PIPE_SPAWN_INTERVAL) {
                spawnPipe(pipes, pipeTexture);
                pipeSpawnClock.restart();
            }

            for (std::size_t i = 0; i < pipes.size();) {
                pipes[i].top.move({-PIPE_SPEED * dt, 0.f});
                pipes[i].bottom.move({-PIPE_SPEED * dt, 0.f});

                const float pipeRightEdge = pipes[i].bottom.getPosition().x + pipes[i].bottom.getGlobalBounds().size.x;
                if (!pipes[i].scored && pipeRightEdge < bird.getPosition().x) {
                    pipes[i].scored = true;
                    ++score;
                    bestScore = std::max(bestScore, score);
                }

                if (pipeRightEdge < -5.f) {
                    pipes.erase(pipes.begin() + static_cast<std::ptrdiff_t>(i));
                } else {
                    ++i;
                }
            }

            if (isCollidingWithPipes(bird, pipes) || isOutOfBounds(bird)) {
                gameState = GameState::GameOver;
                bestScore = std::max(bestScore, score);
                bird.setRotation(sf::degrees(85.f));
            }
        }

        updateWindowTitle(window, gameState, score, bestScore);

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
