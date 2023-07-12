#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "object.h"
#include "text.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <iostream>
#include <sstream>

const glm::vec2 PLAYER_SIZE(50.0f, 50.0f);
const float PLAYER_VELOCITY(500.0f);
const float Acc(-9.8f);
int delay = 0;
int backoff = 0;
int level = 0;
int space = 0;
float prev = 800.0f;
float prev_coin = 800.0f;
float height = 0;
unsigned int score = 0;

GameObject *Player;
// GameObject *zapper;
std::vector<GameObject> zapper;
std::vector<GameObject> coins;
SpriteRenderer *renderer;
TextRenderer *Text;

bool CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}  

void Game::CollectCoins()
{
    for (int i = 0; i < 30000; i++)
    {
        if (!coins[i].Destroyed)
        {
            if (CheckCollision(*Player, coins[i]))
            {
                coins[i].Destroyed = true;
                score++;
            }
        }
    }
}  

void Game::Zap()
{
    for (int i = 0; i < 50000; i++)
    {
        if (!zapper[i].Destroyed)
        {
            if (CheckCollision(*Player, zapper[i]))
            {
                this->State = GAME_LOSE;
                std::cout << score << "\n";
            }
        }
    }
}  

Game::Game (unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
    delete renderer;
    delete Player;
}

void Game::Init()
{
    this->State = GAME_MENU;

    srand(time(0));

    ResourceManager::LoadShader("../shader/vertex.vs", "../shader/fragment.frag", nullptr, "sprite");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    Shader myShader;
    myShader = ResourceManager::GetShader("sprite");
    renderer = new SpriteRenderer(myShader);

    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("../fonts/font.TTF", 24);

    ResourceManager::LoadTexture("../textures/back.png", false, "back");
    // ResourceManager::LoadTexture("../textures/back.png", false, "back");
    ResourceManager::LoadTexture("../textures/player.png", true, "player");
    ResourceManager::LoadTexture("../textures/zapper_up.png", true, "zapper");
    ResourceManager::LoadTexture("../textures/zapper_down.png", true, "zapperl");
    ResourceManager::LoadTexture("../textures/coin.png", true, "coin");

    // this->Level = 1;
    // level = 1;

    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.5,
        this->Height / 2   
    );
 
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("player"));  
    Player->Destroyed = false; 

    for (int i = 0; i < 50000; i++)
    {

        float x = (100 + rand() % 400) + prev;
        float y = rand() % 400;
        prev = x;

        int select = rand() % 2;

        if (select == 0)
        {
            GameObject zap(glm::vec2(x, y), glm::vec2(120.0f, 60.0f), 
                    ResourceManager::GetTexture("zapperl"), 
                    glm::vec3(1.0f, 1.0f, 1.0f)
                );
            zap.IsSolid = true;
            zap.Destroyed = false;
            zapper.push_back(zap);
        }

        else
        {
            GameObject zap(glm::vec2(x, y), glm::vec2(60.0f, 120.0f), 
                        ResourceManager::GetTexture("zapper"), 
                        glm::vec3(1.0f, 1.0f, 1.0f)
                    );

            zap.IsSolid = true;
            zap.Destroyed = false;
            zapper.push_back(zap);
        }

    
    }

    for (int j = 0; j < 3000; j++)
    {  
        height = rand() % 400;

        for (int i = 0; i < 10; i++)
        {
            float x = rand() % 40 + prev_coin;
            float y = height;
            prev_coin = x;

            GameObject coin(glm::vec2(x, y), glm::vec2(30.0f, 30.0f), 
                ResourceManager::GetTexture("coin"), 
                glm::vec3(1.0f, 1.0f, 1.0f)
            );

            coin.IsSolid = true;
            coin.Destroyed = false;
            coins.push_back(coin);       

        }


    }
}

void Game::Update(float dt)
{
    if(this->State == GAME_ACTIVE)
    {
        if (delay % 300 == 0)
        {
            level++;
            std::cout << "hello\n";
        }

        if (level > 3)
        {
            this->State = GAME_WIN;
            std::cout << score << "\n";
        }

        backoff--;

        if (backoff == -800)
            backoff = 0;

        
        for (int i = 0; i < 50000; i++)
        {
            zapper[i].Position.x -= (5 * level);
            
        }

        for (int i = 0; i < 30000; i++)
            coins[i].Position.x -= (10 + level) ;


        delay++;

        std::cout << level << " " << delay << "\n";
        
        this->CollectCoins();
        this->Zap();
    }
}


void Game::ProcessInput(float dt)
{

    float velocity = PLAYER_VELOCITY * dt;
    float acc = 15.0f;
    float up_acc = 20.0f;

    if (this->Keys[GLFW_KEY_SPACE])
    {
        if (Player->Position.y >= 0.0f)
        {
            velocity += up_acc * dt;
            Player->Position.y -= velocity;
        }
    }

    else 
    {
        if (Player->Position.y <= this->Height / 1.5)
        {
            velocity += acc * dt;
            Player->Position.y += velocity;
        }
    }


    if (this->Keys[GLFW_KEY_ENTER])
    {
        this->State = GAME_ACTIVE;
        score = 0;
        level = 0;
        delay = 0;
        // Game::Init();

    }
}

void Game::Render()
{
    // Rendering the background
    Texture2D myTexture;
    myTexture = ResourceManager::GetTexture("back");
    renderer->DrawBack(myTexture, glm::vec2(backoff, 0), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    if (backoff < 0)
        renderer->DrawBack(myTexture, glm::vec2(backoff + 800, 0), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    
    // Rendering the player
    Player->Draw(*renderer);


    if (this->State == GAME_ACTIVE)
    {
        // Rendering zappers
        
        for (int i = 0 ; i < 50000; i++)
            zapper[i].Draw(*renderer);

        // Rendering coins
        for (int i = 0 ; i < 30000; i++)
            coins[i].Draw(*renderer);

        std::stringstream sscore; sscore << score;
        Text->RenderText("Score: " + sscore.str(), 5.0f, 5.0f, 1.0f);


        std::stringstream slevel; slevel << level;
        Text->RenderText("Level: " + slevel.str(), 200.0f, 5.0f, 1.0f);
    }  

    if (this->State == GAME_WIN)
    {
        Text->RenderText(
            "You WON! ", 220.0, Height / 2 - 80.0, 2.0
        );

        std::stringstream sscore; sscore << score;
        Text->RenderText(
            "Total score = " + sscore.str(), 210.0, Height / 2 - 40.0, 2.0
        );

        Text->RenderText(
            "Press ENTER to play again", 200.0, Height / 2, 2.0
        );
    }  

    if (this->State == GAME_LOSE)  
    {
        Text->RenderText(
            "You LOST  :(", 220.0, Height / 2 - 80.0, 2.0
        );

        std::stringstream sscore; sscore << score;
        Text->RenderText(
            "Total score = " + sscore.str(),210.0, Height / 2 - 40.0, 1.25
        );

        Text->RenderText(
            "Press ENTER to try again", 200.0, Height / 2, 1.25
        );

    } 

    if (this->State == GAME_MENU)
    {
        Text->RenderText(
            "JETPACK JOYRIDE", 220.0, Height / 2 - 160.0, 1.5
        );
        
        Text->RenderText(
            "PRESS ENTER TO BEGIN", 220.0, Height / 2 - 40.0, 1.25
        );

        Text->RenderText(
            "PRESS AND HOLD SPACE TO MOVE", 180.0, Height / 2, 1.25
        );

        Text->RenderText(
            "(Try it out!)", 320.0, Height / 2 + 30, 1.00
        );

    }     
}
