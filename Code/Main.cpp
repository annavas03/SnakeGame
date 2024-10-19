#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream> 
#include <iostream>

using namespace std;
using namespace sf;

const int CELL_SIZE = 20;
const int WIDTH = 1000;
const int HEIGHT = 800;

enum Direction { None, Up, Down, Left, Right };

class Snake {
public:
    Snake() {
        body.push_back(Vector2i(WIDTH / 2 / CELL_SIZE, HEIGHT / 2 / CELL_SIZE));
        direction = Right;
        isAlive = true;
    }

    void update() {
        if (!isAlive) return;

        Vector2i newHead = body.front();
        if (direction == Up) newHead.y--;
        if (direction == Down) newHead.y++;
        if (direction == Left) newHead.x--;
        if (direction == Right) newHead.x++;

        
        if (newHead.x < 0 || newHead.x >= WIDTH / CELL_SIZE || newHead.y < 0 || newHead.y >= HEIGHT / CELL_SIZE || isSegment(newHead)) {
            isAlive = false;
            return;
        }

        body.insert(body.begin(), newHead);
        body.pop_back();
    }

    void grow() {
        body.push_back(body.back());
    }

    void setDirection(Direction newDirection) {
        if ((direction == Up && newDirection != Down) ||
            (direction == Down && newDirection != Up) ||
            (direction == Left && newDirection != Right) ||
            (direction == Right && newDirection != Left)) {
            direction = newDirection;
        }
    }

    const vector<Vector2i>& getBody() const { return body; }
    bool isAliveState() const { return isAlive; }
    void reset() {
        body.clear();
        body.push_back(Vector2i(WIDTH / 2 / CELL_SIZE, HEIGHT / 2 / CELL_SIZE));
        direction = Right;
        isAlive = true;
    }

private:
    vector<Vector2i> body;
    Direction direction;
    bool isAlive;

    bool isSegment(const Vector2i& segment) const {
        for (const auto& bodyPart : body) {
            if (bodyPart == segment) return true;
        }
        return false;
    }
};

class Food {
public:
    Food() {
       /* if (!texture.loadFromFile("Resources/images/orange.png")) {
            cerr << "Error loading food texture!" << std::endl;
        }
        sprite.setTexture(texture);*/ // передача текстури
        respawn();
    }

    void respawn() {
        position.x = rand() % (WIDTH / CELL_SIZE);
        position.y = rand() % (HEIGHT / CELL_SIZE);
        /*sprite.setPosition(position.x * CELL_SIZE, position.y * CELL_SIZE);*/
    }

    /*void draw(RenderWindow& window) {
        window.draw(sprite);
     }*/

    void draw(RenderWindow& window) {
        RectangleShape rect(Vector2f(CELL_SIZE, CELL_SIZE));
        rect.setPosition(position.x * CELL_SIZE, position.y * CELL_SIZE);
        rect.setFillColor(Color::Red); 
        window.draw(rect);
    }

    Vector2i getPosition() const { return position; }

private:
    Texture texture;
    Sprite sprite;
    Vector2i position;
};

void displayGameOver(RenderWindow& window, int score) {
    Font font;
    font.loadFromFile("Resources/Font/AmaticSC-Regular.ttf");

    Text gameOverText("Game Over", font, 50);
    gameOverText.setFillColor(Color::White);
    gameOverText.setPosition(WIDTH / 2 - gameOverText.getGlobalBounds().width / 2, HEIGHT / 2 - 100);

    Text scoreText("Score: " + to_string(score), font, 30);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(WIDTH / 2 - scoreText.getGlobalBounds().width / 2, HEIGHT / 2);

    Text restartText("Press R to Restart", font, 30);
    restartText.setFillColor(Color::White);
    restartText.setPosition(WIDTH / 2 - restartText.getGlobalBounds().width / 2, HEIGHT / 2 + 50);

    window.clear(Color::Black);
    window.draw(gameOverText);
    window.draw(scoreText);
    window.draw(restartText);
    window.display();
}

enum GameState { Menu, Playing, GameOver };


int main() {
    srand(static_cast<unsigned>(time(0)));
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Snake Game");
    Snake snake;
    Food food;

    Clock clock;
    float timer = 0.0f;
    float delay = 0.1f;
    int score = 0;
    int highScore = 0; 
    GameState state = Menu;

   
    std::ifstream inputFile("BestScore.txt");
    if (inputFile.is_open()) {
        inputFile >> highScore;
        inputFile.close();
    }

    
    Font font;
    font.loadFromFile("Resources/Font/AmaticSC-Regular.ttf");

    Text title("Snake Game", font, 60);
    title.setFillColor(Color::White);
    title.setPosition(WIDTH / 2 - title.getGlobalBounds().width / 2, HEIGHT / 2 - 100);

    Text startText("Press Enter to Start", font, 40);
    startText.setFillColor(Color::White);
    startText.setPosition(WIDTH / 2 - startText.getGlobalBounds().width / 2, HEIGHT / 2);

    Text gameOverText("", font, 60);
    gameOverText.setFillColor(Color::White);
    gameOverText.setPosition(WIDTH / 2 - 150, HEIGHT / 2 - 50);

    Text restartText("Press R to Restart", font, 40);
    restartText.setFillColor(Color::White);
    restartText.setPosition(WIDTH / 2 - restartText.getGlobalBounds().width / 2, HEIGHT / 2 + 50);

    Text highScoreText("High Score: " + to_string(highScore), font, 40);
    highScoreText.setFillColor(Color::White);
    highScoreText.setPosition(WIDTH / 2 - highScoreText.getGlobalBounds().width / 2, HEIGHT / 2 + 100);

    Text exitText("Press Esc to Exit", font, 40);
    exitText.setFillColor(Color::White);
    exitText.setPosition(WIDTH / 2 - exitText.getGlobalBounds().width / 2, HEIGHT / 2 + 50); 

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed) {
                if (state == Menu) {
                    if (event.key.code == Keyboard::Enter) {
                        state = Playing;
                        snake.reset();
                        food.respawn();
                        score = 0;
                    }
                    if (event.key.code == Keyboard::Escape) {
                        window.close(); 
                    }
                }
                else if (state == Playing) {
                    if (event.key.code == Keyboard::Up) snake.setDirection(Up);
                    if (event.key.code == Keyboard::Down) snake.setDirection(Down);
                    if (event.key.code == Keyboard::Left) snake.setDirection(Left);
                    if (event.key.code == Keyboard::Right) snake.setDirection(Right);
                }
                else if (state == GameOver) {
                    if (event.key.code == Keyboard::R) {
                        state = Playing;
                        snake.reset();
                        food.respawn();
                        score = 0;
                    }
                    if (event.key.code == Keyboard::Escape) {
                        window.close(); 
                    }
                }
            }
        }

        if (state == Playing) {
            timer += clock.restart().asSeconds();
            if (timer >= delay) {
                snake.update();

                
                if (snake.getBody().front() == food.getPosition()) {
                    snake.grow();
                    food.respawn();
                    score++;
                }

                
                if (!snake.isAliveState()) {
                    state = GameOver;

                    
                    if (score > highScore) {
                        highScore = score;

                        
                        std::ofstream outputFile("BestScore.txt");
                        if (outputFile.is_open()) {
                            outputFile << highScore;
                            outputFile.close();
                        }

                        
                        highScoreText.setString("High Score: " + to_string(highScore));
                    }
                }

                timer = 0.0f;
            }
        }

        window.clear(Color::Black); 

        if (state == Menu) {
            
            window.draw(title);
            window.draw(startText);
            window.draw(highScoreText); 
            window.draw(exitText);
        }
        else if (state == Playing) {
            
            for (const auto& segment : snake.getBody()) {
                RectangleShape rect(Vector2f(CELL_SIZE, CELL_SIZE));
                rect.setPosition(segment.x * CELL_SIZE, segment.y * CELL_SIZE);
                rect.setFillColor(Color::Green);
                window.draw(rect);
            }

            
            food.draw(window); 
        }
        else if (state == GameOver) {
            
            gameOverText.setString("Game Over! Score: " + to_string(score));
            window.draw(gameOverText);
            window.draw(restartText); 
            window.draw(highScoreText); 
        }

        window.display(); 
    }

    return 0;
}
