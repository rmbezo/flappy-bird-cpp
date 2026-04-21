# Flappy Bird C++

Мой первый Flappy Bird на C++ с использованием SFML.

Сейчас в проекте уже есть:
- окно игры
- птица
- трубы
- фон
- базовая физика прыжка
- столкновения

## Стек

- C++
- SFML
- CMake

## Структура проекта

```text
.
├── assets
│   ├── background.png
│   ├── bird.png
│   └── pipe.png
├── CMakeLists.txt
└── src
    └── main.cpp
```
## Управление 
- Space - прыжок
- Закрытие окна - выход из игры


## Сборка и запуск
```
cmake -S . -B build
cmake --build build
./build/flappy-bird
```
