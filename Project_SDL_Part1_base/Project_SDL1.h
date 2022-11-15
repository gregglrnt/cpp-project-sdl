// SDL_Test.h: Includedatei für Include-Standardsystemdateien
// oder projektspezifische Includedateien.
/*Pape Ibrahima diawara*/

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <vector>

// Defintions
constexpr double frame_rate = 60.0; // refresh rate
constexpr double frame_time = 1. / frame_rate;
constexpr unsigned frame_width = 640; // Width of window in pixel
constexpr unsigned frame_height = 480; // Height of window in pixel
// Minimal distance of animals to the border
// of the screen
constexpr unsigned frame_boundary = 100;

constexpr int animal_size = 64;

constexpr int sheepSpeed = 2;
constexpr int wolfSpeed = 2;

constexpr char sheepSpritePath[] = "../media/sheep.png";
constexpr char wolfSpritePath[] = "../media/wolf.png";

// Helper function to initialize SDL
void init();

class animal {
private:
  SDL_Surface* window_surface_ptr_; // ptr to the surface on which we want the
                                    // animal to be drawn, also non-owning
  SDL_Surface* image_ptr_; // The texture of the sheep (the loaded image), use
                           // load_surface_for
  // todo: Attribute(s) to define its position
protected:
  SDL_Rect animalRect;
  int w, h, x, y;
public:
  animal(const std::string& file_path, SDL_Surface* window_surface_ptr);
  // todo: The constructor has to load the sdl_surface that corresponds to the
  // texture
  virtual ~animal(); // todo: Use the destructor to release memory and "clean up
               // behind you"

  void draw(); // todo: Draw the animal on the screen <-> window_surface_ptr.
                 // Note that this function is not virtual, it does not depend
                 // on the static type of the instance
  void setPos(int x, int y);
  void setSize(int w, int h);
  virtual void move() = 0; // todo: Animals move around, but in a different
           // fashion depending on which type of animal
    int getWidth() { return w;}
    int getHeight() { return h;}
    int getX() {return x;}
    int getY() {return y;}
};

// Insert here:
// class sheep, derived from animal
class sheep : public animal {
  // todo
  // Ctor
  int xSpeed = 0, ySpeed = 0;
public:
  sheep(const std::string& filePath, SDL_Surface* window_surface_ptr);
  // Dtor
  virtual ~sheep();

  // implement functions that are purely virtual in base class
  void move() override;
  void setSpeed(int x, int y);
};

// Insert here:
// class wolf, derived from animal
// Use only sheep at first. Once the application works
// for sheep you can add the wolves
class wolf : public animal {
    int xSpeed = 0, ySpeed = 0;

  std::vector<std::shared_ptr<sheep>> allSheep;
public:
  wolf(const std::string& filePath, SDL_Surface* window_surface_ptr);
  // Dtor
  virtual ~wolf();

  // implement functions that are purely virtual in base class
  void move() override;
  void setSpeed(int x, int y);

  // Sheeps to tracks every frame
  void addSheepToTrack(std::shared_ptr<sheep> sheep);
};

// The "ground" on which all the animals live (like the std::vector
// in the zoo example).
class ground {
private:
  // Attention, NON-OWNING ptr, again to the screen
  SDL_Surface* window_surface_ptr_;
  // Some attribute to store all the wolves and sheep
  // here
  std::vector<std::shared_ptr<sheep>> sheeps;
  std::vector<std::shared_ptr<wolf>> wolves;
public:
  ground(SDL_Surface* window_surface_ptr); // todo: Ctor
  ~ground(); // todo: Dtor, again for clean up (if necessary)
  void add_animal(int id); // todo: Add an animal
  void update(); // todo: "refresh the screen": Move animals and draw them
  // Possibly other methods, depends on your implementation
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
