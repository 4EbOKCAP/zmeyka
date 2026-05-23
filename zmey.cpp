#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

const int WIDTH = 30;       // Ширина
const int HEIGHT = 20;      // Высота
const int INIT_LEN = 2;     // длина змейки

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

// Глобальные переменные
vector<Point> snake;        // Змейка
Direction dir;              // Текущее направление
Point food;                 // Позиция еды
int score;                  // Счёт
bool gameOver;              // Флаг окончания игры

HANDLE hConsole;            // Хэндл консоли для работы с курсором

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hConsole, coord);
}

void HideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// Отрисовка границы поля
void DrawBorder() {
    // Верхняя граница
    for (int i = 0; i < WIDTH; ++i) {
        gotoxy(i, 0);
        cout << "#";
    }
    // Нижняя граница
    for (int i = 0; i < WIDTH; ++i) {
        gotoxy(i, HEIGHT - 1);
        cout << "#";
    }
    // Левая граница
    for (int i = 0; i < HEIGHT; ++i) {
        gotoxy(0, i);
        cout << "#";
    }
    // Правая граница
    for (int i = 0; i < HEIGHT; ++i) {
        gotoxy(WIDTH - 1, i);
        cout << "#";
    }
}

// Отрисовка змейки
void DrawSnake() {
    for (size_t i = 0; i < snake.size(); ++i) {
        gotoxy(snake[i].x, snake[i].y);
        if (i == 0)
            cout << "Z";    // Голова
        else
            cout << "O";    // Тело
    }
}

void DrawFood() {
    gotoxy(food.x, food.y);
    cout << "F";
}

void GenerateFood() {
    bool freeCellFound = false;
    while (!freeCellFound) {
        int fx = rand() % (WIDTH - 2) + 1;
        int fy = rand() % (HEIGHT - 2) + 1;
        freeCellFound = true;
        for (const auto& segment : snake) {            // проверка не занята ли клетка змейкой
            if (segment.x == fx && segment.y == fy) {
                freeCellFound = false;
                break;
            }
        }
        if (freeCellFound) {
            food.x = fx;
            food.y = fy;
        }
    }
}

bool CheckCollision() {
    Point head = snake[0];
    if (head.x <= 0 || head.x >= WIDTH - 1 || head.y <= 0 || head.y >= HEIGHT - 1)          // Столкновение границей
        return true;
    for (size_t i = 1; i < snake.size(); ++i) {                 // Столкновение с собственным телом
        if (snake[i].x == head.x && snake[i].y == head.y)
            return true;
    }
    return false;
}

void ProcessInput() {
    if (_kbhit()) {
        int key = _getch();
        // Стрелки: первый символ 224 или 0
        if (key == 224) {
            key = _getch();
            switch (key) {
            case 72: if (dir != DOWN)  dir = UP;    break;
            case 80: if (dir != UP)    dir = DOWN;  break;
            case 75: if (dir != RIGHT) dir = LEFT;  break;
            case 77: if (dir != LEFT)  dir = RIGHT; break;
            }
        }
        // WASD
        else {
            switch (tolower(key)) {
            case 'w': if (dir != DOWN)  dir = UP;    break;
            case 's': if (dir != UP)    dir = DOWN;  break;
            case 'a': if (dir != RIGHT) dir = LEFT;  break;
            case 'd': if (dir != LEFT)  dir = RIGHT; break;
            case 27: gameOver = true;                break; // ESC - выход
            }
        }
    }
}

void UpdateGame() {
    if (dir == STOP) return;

    // Новая голова
    Point newHead = snake[0];
    switch (dir) {
    case UP:    newHead.y--; break;
    case DOWN:  newHead.y++; break;
    case LEFT:  newHead.x--; break;
    case RIGHT: newHead.x++; break;
    }

    if (newHead.x == food.x && newHead.y == food.y) {
        snake.insert(snake.begin(), newHead);           //добавляем новую голову
        score++;
        GenerateFood(); 
        if (snake.size() == (WIDTH - 2) * (HEIGHT - 2)) {
            gameOver = true;
            cout << "\n\nТЫ ПОБЕДИЛ!";
        }
    }
    else {
        snake.insert(snake.begin(), newHead);
        snake.pop_back();
    }

    // Проверка столкновений после обновления
    if (CheckCollision())
        gameOver = true;
}

void Render() {
    DrawBorder();
    DrawSnake();
    DrawFood();
    gotoxy(1, 0);
    cout << "Счёт: " << score;
    gotoxy(WIDTH - 10, 0);
    cout << "ESC - Выход";
}

// запуск игры
void InitGame() {
    snake.clear();
    int startX = WIDTH / 2;
    int startY = HEIGHT / 2;
    for (int i = 0; i < INIT_LEN; ++i) {
        snake.push_back(Point(startX - i, startY));
    }
    dir = RIGHT;                // Начальное направление – вправо
    score = 0;
    gameOver = false;

    GenerateFood();
}

int main() {
    setlocale(LC_ALL, "Russian");
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HideCursor();

    srand(static_cast<unsigned>(time(nullptr)));    // генератора случайных чисел

    InitGame();
    Render();

    while (!gameOver) {
        ProcessInput();     // Обработка ввода
        UpdateGame();       // Обновление логики
        Render();           // Отрисовка
        Sleep(100);         // Задержка для регулировки скорости
    }

    // Игра окончена – вывод финального сообщения
    gotoxy(WIDTH / 2 - 5, HEIGHT / 2);
    cout << "ПРОИГРЫШ!";
    gotoxy(WIDTH / 2 - 8, HEIGHT / 2 + 1);
    cout << "ВАШ СЧЁТ: " << score;
    gotoxy(0, HEIGHT);
    return 0;
}