// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
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

  //First add player
  gameGround->add_player();
  //Then add dog
  gameGround->add_shepherd_dog();


  for(int i = 0; i < n_sheep; ++i)
  {
    gameGround->add_animal(0, {0,0}, true);
  }

  for(int i = 0; i < n_wolf; ++i)
  {
    gameGround->add_animal(1, {0,0}, true);
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
    int mouse_x = 0, mouse_y = 0;

    int ix = 0, iy = 0;
    while(SDL_PollEvent(&e))
    {
      //Quit
      if(e.type == SDL_QUIT)
      {
        isRunning = false;
        break;
      }
      else if(e.type == SDL_KEYDOWN)
      {
        switch(e.key.keysym.sym)
        {
          case SDLK_LEFT:
            ix = -1;
            break;
          case SDLK_RIGHT:
            ix = 1;
            break;
          case SDLK_DOWN:
            iy = 1;
            break;
          case SDLK_UP:
            iy = -1;
            break;
          default:
            ix =0;
            iy =0;
            break;
        }
        gameGround->setPlayerInput(ix, iy);
      }
      else if(e.type == SDL_KEYUP)
      {
        gameGround->setPlayerInput(0,0);
      }
      else if(e.type == SDL_MOUSEBUTTONDOWN)
      {
        SDL_GetMouseState(&mouse_x, &mouse_y);
        std::cout << "mouse" << std::endl;
        if(e.button.button == SDL_BUTTON_LEFT)
        {
          gameGround->setMouseInput(mouse_x, mouse_y);
          std::cout << "mouse left"  << std::endl;
        }
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

      std::cout << "SCORE: "<< gameGround->getScore() << std::endl;

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
void ground::add_animal(int id, Vec2 pos, bool random)
{
  if(allAnimals.size() >= MAX_ANIMALS) return;
  if(id == 0)
  {

    std::shared_ptr<sheep> newSheep =
      std::make_shared<sheep>(window_surface_ptr_, sheepSpritePath);
    newSheep->setSize(animal_size, animal_size);
    int hw = newSheep->getWidth();
    int hh = newSheep->getHeight();
    int randomX =  hw + frame_boundary + (std::rand() % (frame_width - frame_boundary - hw));
    int randomY = hh + frame_boundary + (std::rand() % (frame_height - frame_boundary - hh));
    if(random)
    newSheep->setPos( randomX, randomY);
    else
      newSheep->setPos(pos.x, pos.y);

    newSheep->randomizeSpeed(-sheepSpeed, sheepSpeed);

    sheeps.insert(newSheep);
    allAnimals.insert(newSheep);
  }
  else if(id == 1)
  {
    std::shared_ptr<wolf> newWolf = std::make_shared<wolf>(window_surface_ptr_, wolfSpritePath);
    newWolf->setSize(animal_size, animal_size);
    int hw = newWolf->getWidth();
    int hh = newWolf->getHeight();
    int randomX =  hw + frame_boundary + (std::rand() % (frame_width - frame_boundary - hw));
    int randomY = hh + frame_boundary + (std::rand() % (frame_height - frame_boundary - hh));

    if(random)
    newWolf->setPos(randomX, randomY);
    else {
      newWolf->setPos(pos.x, pos.y);
    }
    newWolf->randomizeSpeed(-wolfSpeed, wolfSpeed);
    newWolf->setDog(dog);

    wolves.insert(newWolf);
    allAnimals.insert(newWolf);
  }

  std::vector<std::shared_ptr<MovingObject>> preyList;
  for(auto a : allAnimals)
  {
    if(a->hasTag("prey"))
    {
      preyList.push_back(a);
    }
  }

  //Update all the hunters for now just wolf
  for(auto w : wolves)
  {
    w->setPreyList(preyList);
  }




}

void ground::add_player()
{
  player = std::make_shared<Player>(window_surface_ptr_, playerSpritePath);
  player->setSize(player_size, player_size);


}

void ground::add_shepherd_dog()
{
  dog = std::make_shared<Dog>(window_surface_ptr_, dogSpritePath);
  dog->setSize(animal_size, animal_size);

  dog->setRoundCenter(player);
  allAnimals.insert(dog);
}

void ground::setPlayerInput(int ix, int iy)
{
  int sqt = std::sqrt(ix*ix + iy*iy);
  int dx = sqt == 0 ? 0 : ix / sqt;
  int dy = sqt == 0 ? 0 : iy / sqt;

  player->setSpeed(dx*playerSpeed, dy*playerSpeed);
}

void ground::setMouseInput(int x, int y)
{
  //Check if clicked inside playArea
  if(x < frame_boundary ||
     x >= frame_width-frame_boundary) return;
  if(y < frame_boundary ||
     y >= frame_height-frame_boundary) return;
  int dist = dog->getDistTo({x, y});
  if(dist < CLICK_DISTANCE && !commandingUnit)
  {
    std::cout << dist << "click distance" << std::endl;
    dog->startCommand();
    commandingUnit = true;
  }
  else
  {
    dog->endCommand({x, y});
    commandingUnit = false;
  }
}

/// <summary>
/// Update the ground during each frame
/// </summary>
void ground::update()
{
  SDL_FillRect(window_surface_ptr_, NULL, 0x02AA02);

  //Set dog to round the player

  for(auto a : allAnimals)
  {
    a->move();
    a->draw();
  }

  player->move();
  player->draw();


  remove_dead_animals();

  //Only breed sheep for now
  //
  add_new_animals();



}

void ground::remove_dead_animals()
{
  bool dead = false;

  std::vector<std::shared_ptr<MovingObject>> animalRm;
  for(auto a : allAnimals)
  {
    if(a->hasTag("dead"))
    {
      animalRm.push_back(a);
      dead = true;
    }
  }
  for(auto x : animalRm)
  {
    allAnimals.erase(x);
  }

  if(dead)
  {
    std::vector<std::shared_ptr<wolf>> wolfRm;
    for(auto a : wolves)
    {
      if(a->hasTag("dead"))
      {
        wolfRm.push_back(a);
      }
    }

    for(auto x : wolfRm)
    {
      wolves.erase(x);
    }
    std::vector<std::shared_ptr<sheep>> sheepRm;
    for(auto a : sheeps)
    {
      if(a->hasTag("dead"))
      {
        sheepRm.push_back(a);
      }
    }

    for(auto x : sheepRm)
    {
      sheeps.erase(x);
    }
    std::vector<std::shared_ptr<MovingObject>> preyList;
    for(auto a : allAnimals)
    {
      if(a->hasTag("prey"))
      {
        preyList.push_back(a);
      }
    }

    //Update all the hunters for now just wolf
    for(auto w : wolves)
    {
      w->setPreyList(preyList);
    }
  }
  else
  {
    return;
  }



}


void ground::add_new_animals()
{
  for(auto a : allAnimals)
  {
    for(auto b : allAnimals)
    {
      if(a->getDistTo(b->getPos()) < INTERACT_DISTANCE &&
         a->hasTag("female") && b->hasTag("male"))
      {
        a->interact(b);
      }
    }
  }

  std::vector<std::pair<int,Vec2>> addPositions;
  for(auto a : allAnimals)
  {
    //only sheep for now
    if(a->hasTag("child") && a->hasTag("sheep"))
    {
      a->removeTag("child");
      addPositions.push_back({0, a->getPos()});
    }
  }

  for(auto v : addPositions){
    add_animal(v.first, v.second);
  }
}


void Interactable::addTag(const std::string& tag)
{
  if(!hasTag(tag))
  tags.insert(tag);
}

bool Interactable::hasTag(const std::string& tag)
{
  return tags.find(tag) != tags.end();
}

void Interactable::removeTag(const std::string& tag)
{
  if(hasTag(tag))
  {
    tags.erase(tag);
  }
}

void Interactable::interact(std::shared_ptr<Interactable> other)
{
}

Interactable::~Interactable()
{

}

RenderedObject::RenderedObject(SDL_Surface* window_surface_ptr, const std::string& textureFile) {
  window_surface_ptr_ = window_surface_ptr;
  image_ptr_ = load_surface_for(textureFile, window_surface_ptr);

  animalRect.w = image_ptr_->w;
  animalRect.h = image_ptr_->h;
  animalRect.x = 0;
  animalRect.y = 0;
}

RenderedObject::~RenderedObject()
{

}

void RenderedObject::draw()
{
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;
  SDL_BlitScaled(image_ptr_, 0, window_surface_ptr_, &rect);
}


void RenderedObject::setPos(int x, int y)
{
  this->x = x;
  this->y = y;
}

void RenderedObject::setSize(int w, int h)
{
  this->w = w;
  this->h = h;
}


MovingObject::MovingObject(SDL_Surface* window_surface_ptr, const std::string& textureFile)
  : RenderedObject(window_surface_ptr, textureFile)
{

}

MovingObject::~MovingObject()
{

}

void MovingObject::move()
{

}

void MovingObject::setSpeed(int x, int y)
{
  xSpeed = x;
  ySpeed = y;
}

//Animal
animal::animal(SDL_Surface* window_surface_ptr, const std::string& filePath)
  : MovingObject(window_surface_ptr, filePath) {

}

animal::~animal()
{
  SDL_FreeSurface(image_ptr_);
}

// Draw the respective animal


sheep::sheep(SDL_Surface* window_surface_ptr, const std::string& filePath)
: animal( window_surface_ptr, filePath){
  addTag("sheep");

  addTag("prey");

  if(rand() % 100 < 50)
  {
    addTag("female");
    std::cout << "Sheep spawned: gender->female" << std::endl;
  }
  else {
    addTag("male");
    std::cout << "Sheep spawned: gender->male" << std::endl;
  }
}
sheep::~sheep()
{

}


// Sheep moves
void sheep::move()
{
  //Boundary of the ground horizontal
  //Velocity is reversed with random bounce
  int reboundrand = -sheepSpeed + (std::rand() % (2*sheepSpeed));
  if(x >= frame_width-frame_boundary)
  {
    x = frame_width-frame_boundary-2;
    setSpeed(-xSpeed, reboundrand);
  }
  else if(x <= frame_boundary)
  {
    x = frame_boundary+2;
    setSpeed(-xSpeed, reboundrand);
  }

  //Boundary of the ground vertical
  // Velocity is reversed with random bounce
  if(y >= frame_height-frame_boundary)
  {
    y = frame_height-frame_boundary-2;
    setSpeed(reboundrand, -ySpeed);
  }
  else if(y <= frame_boundary)
  {
    y = frame_boundary + 2;
    setSpeed(reboundrand, -ySpeed);
  }

  //Set the position according to the speed
  setPos(x+xSpeed, y+ySpeed);

}

void sheep::interact(std::shared_ptr<Interactable> other)
{
  if(other->hasTag("sheep") && other->hasTag("male") && hasTag("female"))
  {
    auto now = SDL_GetTicks();
    if(now - lastChild < BREED_MS) return;
    addTag("child");
    //std::cout << "Sheep pregnant!" << std::endl;
    lastChild = now;
  }
}


wolf::wolf(SDL_Surface* window_surface_ptr, const std::string& filePath)
  : animal(window_surface_ptr, filePath){
  addTag("wolf");
}

wolf::~wolf() {

}

// Wolf follows nearest sheep
void wolf::move() {
  int now = SDL_GetTicks();

  if(now - lastFood > STARVE_MS)
  {
    addTag("dead");
    return;
  }

  if(dog == nullptr)
  {
    std::cout << "DOG an Wolf creation order wrong!" << std::endl;
    return;
  }
  Vec2 dogPos = dog->getPos();
  int dogdx = dogPos.x - x;
  int dogdy = dogPos.y - y;
  int dogDist = (dogdx * dogdx) + (dogdy * dogdy);

  if(dogdx > 0) dogdx = 1;
  else if(dogdx < 0) dogdx = -1;
  else dogdx = 0;

  if(dogdy > 0) dogdy = 1;
  else if(dogdy < 0) dogdy = -1;
  else dogdy = 0;

  if(std::sqrt(dogDist) < 100)
  {
    setSpeed(-dogdx * wolfSpeed, -dogdy * wolfSpeed);
    setPos(x+xSpeed, y+ySpeed);
    //std::cout << "Dog close!" << std::endl;
    return;
  }

  // Dog not close

  if(preyList.size() == 0) {
    //Boundary of the ground horizontal
    //Velocity is reversed with random bounce
    int reboundrand = -wolfSpeed + (std::rand() % (2 * wolfSpeed));
    if(x >= frame_width-frame_boundary)
    {
      x = frame_width-frame_boundary-2;
      setSpeed(-xSpeed, reboundrand);
    }
    else if(x <= frame_boundary)
    {
      x = frame_boundary+2;
      setSpeed(-xSpeed, reboundrand);
    }

    //Boundary of the ground vertical
    // Velocity is reversed with random bounce
    if(y >= frame_height-frame_boundary)
    {
      y = frame_height-frame_boundary-2;
      setSpeed(reboundrand, -ySpeed);
    }
    else if(y <= frame_boundary)
    {
      y = frame_boundary + 2;
      setSpeed( reboundrand, -ySpeed);
    }

    //Set the position according to the speed
    setPos(x+xSpeed, y+ySpeed);

    return;
  }


  //Find the sheep
  std::shared_ptr<MovingObject> nearest;
  int minDist = 1000000;
  for(auto prey : preyList)
  {
    int dist = getDistTo(prey->getPos());
    if(minDist > dist)
    {
      minDist = dist;
      nearest = prey;
    }
  }

  // Calculate directions to nearest sheep
  int dX = nearest->getX() - x ;// + nearestSheep->getWidth();
  if(dX > 0) dX = 1;
  else if(dX < 0) dX = -1;
  else dX = 0;
  int dY = nearest->getY() - y;// + nearestSheep->getHeight();
  if(dY > 0) dY = 1;
  else if(dY < 0) dY = -1;
  else dY = 0;

  
  if(dX != 0 || dY != 0)
  {
    setSpeed(dX * wolfSpeed, dY * wolfSpeed);
    setPos(x+xSpeed, y+ySpeed);

  }
  if(minDist < HUNT_DISTANCE)
  {
    interact(nearest);
    lastFood = now;
  }
}

void wolf::interact(std::shared_ptr<Interactable> other)
{
  if(other->hasTag("prey"))
  {
    if(!other->hasTag("dead"))
    other->addTag("dead");
  }
}





Player::Player(SDL_Surface* window_surface_ptr, const std::string& filePath)
  : MovingObject(window_surface_ptr, filePath)
{
  addTag("player");
}

void Player::move()
{
  //TAKE input from player to move player
  int nx = x+xSpeed;
  int ny = y+ySpeed;
  if(nx > frame_width - frame_boundary) nx = frame_width - frame_boundary;
  else if(nx < frame_boundary)  nx = frame_boundary;

  if(ny > frame_height - frame_boundary) ny = frame_height - frame_boundary;
  else if(ny < frame_boundary) ny = frame_boundary;
  setPos(nx, ny);

}

Dog::Dog(SDL_Surface* window_surface_ptr, const std::string& filePath)
  : animal(window_surface_ptr, filePath)
{
  addTag("dog");
}

void Dog::move()
{
  if(moveCommand)
  {
    int dx = targetPos.x - x;
    int dy = targetPos.y - y;

    float dist = std::sqrt((dx * dx) + (dy * dy));

    if(dist <= 5)
    {
      moveCommand = false;
      moveBack = true;
      return;
    }
    if(dx > 0) dx = 1;
    else if(dx < 0) dx = -1;
    else dx = 0;
    if(dy > 0) dy = 1;
    else if(dy < 0) dy = -1;
    else dy = 0;

    setSpeed(dx * dogSpeed, dy * dogSpeed);
    setPos(x+xSpeed, y+ySpeed);
  }
  else if(moveBack)
  {
    int dx = roundCenter->getX() - x;
    int dy = roundCenter->getY() - y;

    float dist = std::sqrt((dx * dx) + (dy * dy));

    if(dist <= radius)
    {
      moveBack = false;
      return;
    }

    if(dx > 0) dx = 1;
    else if(dx < 0) dx = -1;
    else dx = 0;
    if(dy > 0) dy = 1;
    else if(dy < 0) dy = -1;
    else dy = 0;

    setSpeed(dx * dogSpeed, dy * dogSpeed);
    setPos(x+xSpeed, y+ySpeed);
  }
  else
  {
    float speed = dogRot;
    int lx = radius * std::cos(angle);
    int ly = radius * std::sin(angle);

    angle += speed;
    if(angle == 360.0f) angle = 0.0f;
    if(angle == -360.0f) angle = 0.0f;

    setPos(lx + roundCenter->getX(), ly + roundCenter->getY());
  }


}
