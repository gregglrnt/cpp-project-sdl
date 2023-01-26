#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <vector>
#include <set>
#include <cmath>
// Defintions
constexpr double frame_rate = 60.0; // refresh rate
constexpr double frame_time = 1. / frame_rate;
constexpr unsigned frame_width = 640; // Width of window in pixel
constexpr unsigned frame_height = 480; // Height of window in pixel
// Minimal distance of animals to the border
// of the screen
constexpr unsigned frame_boundary = 100;

constexpr int animal_size = 32;
constexpr int player_size = 32;

constexpr int sheepSpeed = 2;
constexpr int wolfSpeed = 1;
constexpr int dogSpeed = 4;
constexpr int playerSpeed = 4;
constexpr int dogRadius = 40;
constexpr float dogRot = 0.01;

constexpr char sheepSpritePath[] = "../media/sheep.png";
constexpr char wolfSpritePath[] = "../media/wolf.png";
constexpr char playerSpritePath[] = "../media/player.png";
constexpr char dogSpritePath[] = "../media/dog.png";

constexpr int MAX_ANIMALS = 50;

constexpr int HUNT_DISTANCE = 10;
constexpr int INTERACT_DISTANCE = 20;
constexpr int BREED_MS = 4000;

constexpr int STARVE_MS = 8000;
constexpr int CLICK_DISTANCE = 200;
// Helper function to initialize SDL
void init();

struct Vec2 {
int x,y;
};

class Interactable {
protected:
std::setstd::string tags;
public:
virtual ~Interactable();

void addTag(const std::string& tag);
bool hasTag(const std::string& tag);
void removeTag(const std::string& tag);
virtual void interact(std::shared_ptr<Interactable> other);
};

class RenderedObject : public Interactable {
protected:
SDL_Surface* window_surface_ptr_;
SDL_Surface* image_ptr_;
SDL_Rect animalRect;
int w, h, x, y;
public:
RenderedObject(SDL_Surface* window, const std::string& textureFile);
virtual ~RenderedObject();
void draw();
void setPos(int x, int y);
void setSize(int w, int h);

int getWidth() { return w;}
int getHeight() { return h;}
int getX() {return x;}
int getY() {return y;}
};

class MovingObject : public RenderedObject {
public:
int xSpeed, ySpeed;
public:
MovingObject(SDL_Surface* window, const std::string& textureFile);
virtual ~MovingObject();

virtual void move();
void setSpeed(int x, int y);
void randomizeSpeed(int min, int max)
{
  xSpeed = min + (std::rand() % (max - min));
  ySpeed = min + (std::rand() % (max - min));

  if(xSpeed == 0 && ySpeed == 0) randomizeSpeed(min, max);
}
Vec2 getPos() {return {getX(), getY()};}
int getDistTo(Vec2 pos) {
  int p = pos.x - getX();
  int q = pos.y - getY();
  return std::sqrt( (p*p)+(q*q) );
}

};

class animal : public MovingObject {
public:
animal(SDL_Surface* window_surface_ptr,const std::string& file_path);
// todo: The constructor has to load the sdl_surface that corresponds to the
// texture
virtual ~animal(); // todo: Use the destructor to release memory and "clean up"

//todo: Define a method that moves the animal
// according to its current position and speed
virtual void move() = 0;
};

class sheep : public animal {
public:
sheep(SDL_Surface* window_surface_ptr);
// todo: The constructor has to load the sdl_surface that corresponds to the
// texture
virtual ~sheep(); // todo: Use the destructor to release memory and "clean up"
virtual void move();
};

class wolf : public animal {
long long time_since_last_meal;
std::vector<std::shared_ptr<sheep>> sheep_around;
public:
wolf(SDL_Surface* window_surface_ptr);
// todo: The constructor has to load the sdl_surface that corresponds to the
// texture
virtual ~wolf(); // todo: Use the destructor to release memory and "clean up"
virtual void move();
bool isHungry();
void eat();
std::shared_ptr<sheep> getNearestSheep();
};

class Player : public MovingObject {
Vec2 target;
bool move_to_target;
public:
Player(SDL_Surface* window_surface_ptr, int x, int y);
// todo: Create a constructor that initializes the window surface and the player's
// starting position
virtual ~Player(); // todo: Use the destructor to release memory and "clean up"

// todo: Define a function that updates the player's position and direction
// based on its current speed and target position
void move();
void handleClick(int x, int y);
};


class Dog : public MovingObject {
int click_x, click_y;
bool new_click;
Vec2 target;
std::vector<std::shared_ptr<Interactable>> targets;
int target_index;
bool move_to_target;
int ttl;
public:
Dog(SDL_Surface* window_surface_ptr, int x, int y);
// todo: Create a constructor that initializes the window surface and the dog's
// starting position
virtual ~Dog(); // todo: Use the destructor to release memory and "clean up"

// todo: Define a function that updates the dog's position and direction
// based on its current speed and target position
void move();
void handleClick(int x, int y);
void update();
};


class Ground {
std::set<std::unique_ptr<Interactable>> animals;
SDL_Surface* window_surface_ptr_;
int frame_counter_;
public:
Ground(SDL_Surface* window_surface_ptr);
// todo: Create a constructor that initializes the window surface
virtual ~Ground(); // todo: Use the destructor to release memory and "clean up"

// todo: Define a function that updates the window surface and all animals
// on it
void update();
std::vector<std::shared_ptr<Interactable>> getAllInRadius(Vec2 pos, int radius);
};



class Application {
SDL_Window* window_ptr_;
SDL_Surface* window_surface_ptr_;
Ground ground_;
public:
Application(unsigned num_sheep, unsigned num_wolves);
// todo: Create a constructor that initializes the window and ground
virtual ~Application(); // todo: Use the destructor to release memory and "clean up"

// todo: Define a function that updates the window and ground
int loop(unsigned simulation_time_ms);
};

