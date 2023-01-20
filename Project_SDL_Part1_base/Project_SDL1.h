// SDL_Test.h: Includedatei für Include-Standardsystemdateien
// oder projektspezifische Includedateien.

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
  std::set<std::string> tags;
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
  virtual ~animal(); // todo: Use the destructor to release memory and "clean up
               // behind you"
};

// Insert here:
// class sheep, derived from animal
class sheep : public animal {
  // todo
  // Ctor
  int lastChild = 0;
public:
  sheep(SDL_Surface* window_surface_ptr,const std::string& file_path);
  // Dtor
  virtual ~sheep();

  // implement functions that are purely virtual in base class
  void move() override;
  void interact(std::shared_ptr<Interactable> other) override;
};

// Insert here:
// class wolf, derived from animal
// Use only sheep at first. Once the application works
// for sheep you can add the wolves
class wolf : public animal {
  std::vector<std::shared_ptr<MovingObject>> preyList;
  std::shared_ptr<MovingObject> dog;
  int lastFood = 0;
public:
  wolf(SDL_Surface* window_surface_ptr, const std::string& filePath);
  // Dtor
  virtual ~wolf();

  // implement functions that are purely virtual in base class
  void move() override;

  void interact(std::shared_ptr<Interactable> other) override;

  //TODO: Optimize with move
  void setPreyList(std::vector<std::shared_ptr<MovingObject>> list)
  {
    preyList.clear();
    preyList = list;
  }
  void setDog(std::shared_ptr<MovingObject> p)
  {
      dog = p;
  }
};

class Player : public MovingObject {
public:
    Player(SDL_Surface* window_surface_ptr, const std::string& filePath);
    void move() override;
};

class Dog : public animal {
    private:
    float angle = 0,radius = dogRadius;
    Vec2 targetPos;
    bool moveCommand = false;
    bool commandMode = false;
    bool moveBack = false;
    std::shared_ptr<MovingObject> roundCenter;
public:
    Dog(SDL_Surface* window_surface_ptr, const std::string& filePath);
    void move() override;
    void setRoundCenter(std::shared_ptr<MovingObject> pos)
    {
      roundCenter = pos;
    }
    void startCommand()
    {
      //Only one command at a time
      if(commandMode || moveCommand || moveBack) return;

      std::cout << "command started" << std::endl;
      commandMode = true;

    }
    void endCommand(Vec2 location)
    {
      if(!commandMode) return;
      if(moveCommand) return;

      std::cout << "Command ended" << std::endl;
      commandMode = false;
      targetPos = location;
      moveCommand = true;
    }
};

// The "ground" on which all the animals live (like the std::vector
// in the zoo example).
class ground {
private:
  // Attention, NON-OWNING ptr, again to the screen
  SDL_Surface* window_surface_ptr_;
  // Some attribute to store all the wolves and sheep
  // here

  std::set<std::shared_ptr<MovingObject>> allAnimals;
  std::set<std::shared_ptr<wolf>> wolves;
  std::set<std::shared_ptr<sheep>> sheeps;
  std::shared_ptr<Player> player;
  std::shared_ptr<Dog> dog;

  bool commandingUnit = false;
public:
  ground(SDL_Surface* window_surface_ptr); // todo: Ctor
  ~ground(); // todo: Dtor, again for clean up (if necessary)
  template<typename animal>void add_animal(char* sprite, int speed, Vec2 pos = {0, 0}, bool random = false); // todo: Add an animal
  void update(); // todo: "refresh the screen": Move animals and draw them
  // Possibly other methods, depends on your implementation
  void add_player();
  void add_shepherd_dog();
  void setPlayerInput(int ix, int iy);
  void setMouseInput(int x, int y);

  void remove_dead_animals();
  void add_new_animals();

  int getScore() const { return sheeps.size();};
};

// The application class, which is in charge of generating the window
class application {
private:
  // The following are OWNING ptrs
  SDL_Window* window_ptr_;
  SDL_Surface* window_surface_ptr_;
  SDL_Event window_event_;


  unsigned int frameRate = 60;

  // Other attributes here, for example an instance of ground
  std::unique_ptr<ground> gameGround;
public:
  application(unsigned n_sheep, unsigned n_wolf); // Ctor
  ~application();                                 // dtor

  int loop(unsigned period); // main loop of the application.
                             // this ensures that the screen is updated
                             // at the correct rate.
                             // See SDL_GetTicks() and SDL_Delay() to enforce a
                             // duration the application should terminate after
                             // 'period' seconds
};
