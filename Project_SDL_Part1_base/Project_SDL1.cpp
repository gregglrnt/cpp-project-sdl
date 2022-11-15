// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
/*Pape Ibrahima diawara*/

//

#include "Project_SDL1.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <numeric>
#include <random>
#include <string>

void init() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0)
    throw std::runtime_error("init():" + std::string(SDL_GetError()));

  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
    throw std::runtime_error("init(): SDL_image could not initialize! "
                             "SDL_image Error: " +
                             std::string(IMG_GetError()));
}

namespace {
// Defining a namespace without a name -> Anonymous workspace
// Its purpose is to indicate to the compiler that everything
// inside of it is UNIQUELY used within this source file.

SDL_Surface* load_surface_for(const std::string& filePath,
                              SDL_Surface* window_surface_ptr) {

  // Helper function to load a png for a specific surface
  // See SDL_ConvertSurface
  SDL_Surface* loaded = IMG_Load(filePath.c_str());

  if( loaded == NULL )
  {
    std::cout << "SDL Error: " << SDL_GetError()
      << "Failed to load image: " << filePath << '\n';
  }
  else {
    SDL_Surface* optimized = SDL_ConvertSurface(loaded, window_surface_ptr->format, 0);

    if(optimized == NULL) {
      std::cout << "SDL Error: " << SDL_GetError()
        << "Failed to optimize image: " << filePath << '\n';
    }
    else
    {
      SDL_FreeSurface(loaded);
      return optimized;
    }
  }

  return NULL;
}
} // namespace

application::application(unsigned n_sheep, unsigned n_wolf) {
  window_ptr_ = SDL_CreateWindow("Project_SDL1",
SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, frame_width, frame_height,
0);
  if(!window_ptr_)
  {
    std::cout <<"Error creating window\n";
  }

  window_surface_ptr_ = SDL_GetWindowSurface(window_ptr_);

  if(!window_surface_ptr_)
  {
    std::cout <<"Failed to get window surface\n";
  }

  gameGround = std::make_unique<ground>(window_surface_ptr_);

  for(int i = 0; i < n_sheep; ++i)
  {
    gameGround->add_animal(0);
  }

  for(int i = 0; i < n_wolf; ++i)
  {
    gameGround->add_animal(1);
  }

}

application::~application() {
  SDL_FreeSurface(window_surface_ptr_);
  SDL_DestroyWindow(window_ptr_);

  SDL_Quit();
}


// Main loop of the app
int application::loop(unsigned period) {
  bool isRunning =  true;
  unsigned int ticks_per_frame = 1000.0f / (float)frameRate;

  //Simulation flag to control movement
  bool simulating = true;

  // Time of the first tick to measure total running time of the app
  unsigned int firstTick = SDL_GetTicks();

  while(isRunning)
  {
    auto start = SDL_GetPerformanceCounter();
    unsigned int startTick = SDL_GetTicks();

    SDL_Event e;
    if(SDL_PollEvent(&e))
    {
      //Quit
      if(e.type == SDL_QUIT)
      {
        break;
      }
    }

    //ground loop
    if(simulating)
    {
      gameGround->update();
    }
    //Update surface
    SDL_UpdateWindowSurface(window_ptr_);

    //if frame finished early
    auto end = SDL_GetPerformanceCounter();
    float elapsed = (end - start) / (float) SDL_GetPerformanceFrequency() * 1000.0f;
    SDL_Delay(std::floor(ticks_per_frame - elapsed));

    //cap frame rate
    unsigned int endTick = SDL_GetTicks();
    float frameTime = (endTick - startTick) / 1000.0f;

    // Simulation time limit
    if(endTick - firstTick > period * 1000)
    {
      simulating = false;
    }

    // Application time limit
    if(endTick - firstTick > (period + 5) * 1000)
    {
      isRunning = false;
    }
    //std::cout << "FPS: " << std::to_string(1.0f / frameTime) << std::endl;
  }

  return 0;
}

//Ground
ground::ground(SDL_Surface* window_surface_ptr)
{
  window_surface_ptr_ = window_surface_ptr;
}

ground::~ground()
{
  window_surface_ptr_ = NULL;

}

/// <summary>
/// Function to create animals randomly in the ground
/// </summary>
/// <param name="id"> Animal type 0 : sheep, 1 : wolf</param>
void ground::add_animal(int id)
{

  if(id == 0)
  {
    std::shared_ptr<sheep> newSheep =
      std::make_shared<sheep>(sheepSpritePath,window_surface_ptr_);
    newSheep->setSize(animal_size, animal_size);
    int hw = newSheep->getWidth();
    int hh = newSheep->getHeight();
    int randomX =  hw + frame_boundary + (std::rand() % (frame_width - frame_boundary - hw));
    int randomY = hh + frame_boundary + (std::rand() % (frame_height - frame_boundary - hh));
    newSheep->setPos( randomX, randomY);
    newSheep->setSpeed( -2 + (std::rand() % sheepSpeed), -2 +  (std::rand() % sheepSpeed) );

    sheeps.push_back(newSheep);
  }
  else if(id == 1)
  {
    std::shared_ptr<wolf> newWolf = std::make_shared<wolf>(wolfSpritePath, window_surface_ptr_);
    newWolf->setSize(animal_size, animal_size);
    int hw = newWolf->getWidth();
    int hh = newWolf->getHeight();
    int randomX =  hw + frame_boundary + (std::rand() % (frame_width - frame_boundary - hw));
    int randomY = hh + frame_boundary + (std::rand() % (frame_height - frame_boundary - hh));

    newWolf->setPos(randomX, randomY);
    newWolf->setSpeed(0, 0);

    wolves.push_back(newWolf);
  }
  //For wolves to track sheep
  for(auto wolf : wolves)
  {
    for(auto sheep : sheeps)
    {
      wolf->addSheepToTrack(sheep);
    }
  }
}

/// <summary>
/// Update the ground during each frame
/// </summary>
void ground::update()
{
  SDL_FillRect(window_surface_ptr_, NULL, 0x02AA02);

  //Update the animals
  for(auto sheep : sheeps)
  {
    sheep->move();
    sheep->draw();
  }

  for(auto wolf : wolves)
  {
    wolf->move();
    wolf->draw();
  }
}

//Animal
animal::animal(const std::string& filePath, SDL_Surface* window_surface_ptr)
  : window_surface_ptr_(window_surface_ptr) {
  image_ptr_ = load_surface_for(filePath, window_surface_ptr);

  animalRect.w = image_ptr_->w;
  animalRect.h = image_ptr_->h;
  animalRect.x = 0;
  animalRect.y = 0;
}

animal::~animal()
{
  SDL_FreeSurface(image_ptr_);
}

// Draw the respective animal
void animal::draw()
{
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;
  SDL_BlitSurface(image_ptr_, &animalRect, window_surface_ptr_, &rect);
}


void animal::setPos(int x, int y)
{
  this->x = x;
  this->y = y;
}

void animal::setSize(int w, int h)
{
  this->w = w;
  this->h = h;
}


sheep::sheep(const std::string& filePath, SDL_Surface* window_surface_ptr)
: animal(filePath, window_surface_ptr){
}

sheep::~sheep()
{

}


// Sheep moves
void sheep::move()
{
  //Boundary of the ground horizontal
  //Velocity is reversed with random bounce
  if(x > frame_width-frame_boundary)
  {
    x = frame_width-frame_boundary-2;
    setSpeed(-xSpeed, (std::rand() % 2) * sheepSpeed);
  }
  else if(x < frame_boundary)
  {
    x = frame_boundary+2;
    setSpeed(-xSpeed, (std::rand() % 2) * sheepSpeed);
  }

  //Boundary of the ground vertical
  // Velocity is reversed with random bounce
  if(y > frame_height-frame_boundary)
  {
    y = frame_height-frame_boundary-2;
    setSpeed((std::rand() % 2) * sheepSpeed, -ySpeed);
  }
  else if(y < frame_boundary)
  {
    y = frame_boundary + 2;
    setSpeed((std::rand() % 2) * sheepSpeed, -ySpeed);
  }

  //Set the position according to the speed
  setPos(x+xSpeed, y+ySpeed);

}

void sheep::setSpeed(int x, int y)
{
  xSpeed = x;
  ySpeed = y;
}

wolf::wolf(const std::string& filePath, SDL_Surface* window_surface_ptr)
  : animal(filePath, window_surface_ptr){
}

wolf::~wolf() {

}

// Wolf follows nearest sheep
void wolf::move() {
  if(allSheep.size() == 0) return;




  std::shared_ptr<sheep> nearestSheep;
  int minDist = 1000000;
  for(auto sheep : allSheep)
  {
    int p = (sheep->getX() - x);
    int q = (sheep->getY() - y);
    int dist = (p*p) + (q*q);
    if(minDist > dist)
    {
      minDist = dist;
      nearestSheep = sheep;
    }
  }

  // Calculate directions to nearest sheep
  int dX = nearestSheep->getX() - x + nearestSheep->getWidth();
  if(dX > 0) dX = 1;
  else if(dX < 0) dX = -1;
  else dX = 0;
  int dY = nearestSheep->getY() - y + nearestSheep->getHeight();
  if(dY > 0) dY = 1;
  else if(dY < 0) dY = -1;
  else dY = 0;

  
  if(dX != 0 && dY != 0)
  {
    setSpeed(dX * wolfSpeed, dY * wolfSpeed);
    setPos(x+xSpeed, y+ySpeed);
  }
  else {
  }

}

void wolf::setSpeed(int x, int y) {
  xSpeed = x;
  ySpeed = y;
}

//Sheeps to track for wolves
void wolf::addSheepToTrack(std::shared_ptr<sheep> sheep)
{
  allSheep.push_back(sheep);
}
