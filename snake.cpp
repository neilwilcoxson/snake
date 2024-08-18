#include <array>
#include <deque>
#include <iostream>
#include <random>
#include <SDL.h>

const int UPDATE_TIME_MS = 1000 / 5;

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;

const int BOARD_WIDTH_TILES = 30;
const int BOARD_HEIGHT_TILES = 30;

const int TILE_WIDTH_PIXELS = SCREEN_WIDTH / BOARD_WIDTH_TILES;
const int TILE_HEIGHT_PIXELS = SCREEN_HEIGHT / BOARD_HEIGHT_TILES;

struct GridPosition
{
    int row;
    int col;
};

bool operator==(const GridPosition& a, const GridPosition& b)
{
    return a.row == b.row && a.col == b.col;
}

enum class Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};
const std::array<int, 4> ROW_INC {-1, 1, 0, 0};
const std::array<int, 4> COL_INC {0, 0, -1, 1};

struct Snake
{
    std::deque<GridPosition> locations;
    Direction facingDirection;
};

struct Fruit
{
    GridPosition location;
};

void drawState(SDL_Renderer* const renderer, const Snake& snake, const Fruit& fruit);
void moveSnake(Snake& snake, Fruit& fruit);

struct SdlManager
{
    SDL_Window* window;
    SDL_Surface* screenSurface;
    SDL_Renderer* renderer;
    SdlManager();
    ~SdlManager();
};

int main(int argc, char** argv)
{
    SdlManager sdlManager;

    uint64_t nextFrameTicks = SDL_GetTicks64() + UPDATE_TIME_MS;
    Snake snake {{{0, 0}}, Direction::RIGHT};
    Fruit fruit {BOARD_HEIGHT_TILES / 2, BOARD_WIDTH_TILES / 2};

    SDL_Event e;
    while(true)
    {
        if(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
            case SDL_QUIT:
                return 0;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym)
                {
                case SDLK_UP:
                    snake.facingDirection = Direction::UP;
                    break;
                case SDLK_DOWN:
                    snake.facingDirection = Direction::DOWN;
                    break;
                case SDLK_LEFT:
                    snake.facingDirection = Direction::LEFT;
                    break;
                case SDLK_RIGHT:
                    snake.facingDirection = Direction::RIGHT;
                    break;
                }
            default:
                break;
            }
        }

        uint64_t currentTicks = SDL_GetTicks64();

        if(currentTicks >= nextFrameTicks)
        {
            moveSnake(snake, fruit);
            SDL_SetRenderDrawColor(sdlManager.renderer, 0, 0, 0, 0xff);
            SDL_RenderClear(sdlManager.renderer);
            drawState(sdlManager.renderer, snake, fruit);
            SDL_RenderPresent(sdlManager.renderer);
            nextFrameTicks += UPDATE_TIME_MS;
        }
    }

    return 0;
}

void moveSnake(Snake& snake, Fruit& fruit)
{
    const auto [row, col] = snake.locations.front();
    const int newRow = row + ROW_INC[(size_t)snake.facingDirection];
    const int newCol = col + COL_INC[(size_t)snake.facingDirection];

    if(newRow < 0 || newRow >= BOARD_HEIGHT_TILES || newCol < 0 || newCol >= BOARD_WIDTH_TILES)
    {
        // hit a wall, no additional movement
        return;
    }

    const GridPosition newLocation {newRow, newCol};
    if(std::find(snake.locations.begin(), snake.locations.end(), newLocation) != snake.locations.end())
    {
        // no doubling back
        return;
    }

    snake.locations.push_front(newLocation);

    if(newLocation == fruit.location)
    {
        std::default_random_engine generator;
        do
        {
            fruit.location = {generator() % BOARD_HEIGHT_TILES, generator() % BOARD_WIDTH_TILES};
        } while(std::find(snake.locations.begin(), snake.locations.end(), fruit.location) != snake.locations.end());
    }
    else
    {
        snake.locations.pop_back();
    }
}

void drawState(SDL_Renderer* const renderer, const Snake& snake, const Fruit& fruit)
{
    static const SDL_Color SNAKE_COLOR {0xff, 0x00, 0x00, 0xff};
    SDL_SetRenderDrawColor(renderer, SNAKE_COLOR.r, SNAKE_COLOR.g, SNAKE_COLOR.b, SNAKE_COLOR.a);
    for(const auto& [row, col] : snake.locations)
    {
        const int x = col * TILE_WIDTH_PIXELS;
        const int y = row * TILE_HEIGHT_PIXELS;
        const SDL_Rect snakeRect {x, y, TILE_WIDTH_PIXELS, TILE_HEIGHT_PIXELS};
        SDL_RenderFillRect(renderer, &snakeRect);
    }

    static const SDL_Color FRUIT_COLOR {0x00, 0xff, 0x00, 0xff};
    SDL_SetRenderDrawColor(renderer, FRUIT_COLOR.r, FRUIT_COLOR.g, FRUIT_COLOR.b, FRUIT_COLOR.a);
    const auto& [row, col] = fruit.location;
    const int x = col * TILE_WIDTH_PIXELS;
    const int y = row * TILE_HEIGHT_PIXELS;
    const SDL_Rect fruitRect {x, y, TILE_WIDTH_PIXELS, TILE_HEIGHT_PIXELS};
    SDL_RenderFillRect(renderer, &fruitRect);
}

SdlManager::SdlManager()
{
    int sdlInitResult = SDL_Init(SDL_INIT_EVERYTHING);
    if(sdlInitResult != 0)
    {
        std::cerr << "Error starting SDL" << std::endl;
        exit(-1);
    }
    window = SDL_CreateWindow(
        "Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(window);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

SdlManager::~SdlManager()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}