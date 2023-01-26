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

// application constructor
// Initializes the game with a certain number of sheep and wolves
// n_sheep: number of sheep to be added to the game
// n_wolf: number of wolves to be added to the game
// This function creates the main application window, and sets it's size and position
application::application(unsigned n_sheep, unsigned n_wolf) {
  // Creates the main window for the application, with the title "Project_SDL1"
  window_ptr_ = SDL_CreateWindow("Project_SDL1",
  // Sets the window to be centered on the screen
  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
  // Sets the width and height of the window
  frame_width, frame_height, 0);
  // Error handling for if the window was not created correctly
  if(!window_ptr_) {
    std::cout <<"Error creating window\n";
  }
  // Get the surface of the window
  window_surface_ptr_ = SDL_GetWindowSurface(window_ptr_);
  // Error handling for if the surface was not acquired correctly
  if(!window_surface_ptr_) {
    std::cout <<"Failed to get window surface\n";
  }
  // creates a unique pointer to the ground
  gameGround = std::make_unique<ground>(window_surface_ptr_);
  // calls the function to add player
  gameGround->add_player();
  // calls the function to add shepherd dog
  gameGround->add_shepherd_dog();
  // creates n_sheep number of sheep
  for(int i = 0; i < n_sheep; ++i) {
    gameGround->add_animal(0, {0,0}, true);
  }
  // creates n_wolf number of wolves
  for(int i = 0; i < n_wolf; ++i) {
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
    //flag to check if the game is running
  bool isRunning =  true;
    //ticks per frame
  unsigned int ticks_per_frame = 1000.0f / (float)frameRate;

  //Simulation flag to control movement
//flag to check if the simulation is running

  bool simulating = true;

  // Time of the first tick to measure total running time of the app
    //get the current ticks
  unsigned int firstTick = SDL_GetTicks();

    //game loop
  while(isRunning)
  {
      //get the performance counter
    auto start = SDL_GetPerformanceCounter();
      //get the current ticks
    unsigned int startTick = SDL_GetTicks();

      //event variable
    SDL_Event e;
    int mouse_x = 0, mouse_y = 0;

    int ix = 0, iy = 0;
      //check for events
    while(SDL_PollEvent(&e))
    {
      //Quit
      if(e.type == SDL_QUIT)
      {
        isRunning = false;
        break;
      }
        //if keydown event
      else if(e.type == SDL_KEYDOWN)
      {
        switch(e.key.keysym.sym)
        {
          case SDLK_LEFT:
            ix = -1; //set the vertical direction of player movement to upwards
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
            ix =0; //set the horizontal direction of player movement to 0 if no arrow key is pressed
            iy =0; //set the vertical direction of player movement to 0 if no arrow key is pressed
            break;
        }
        gameGround->setPlayerInput(ix, iy); //pass the input values of player movement to the gameGround object
      }
      else if(e.type == SDL_KEYUP)
      {
          //set the player input to 0 when the arrow key is released
        gameGround->setPlayerInput(0,0);
      }
      else if(e.type == SDL_MOUSEBUTTONDOWN)
      {
        SDL_GetMouseState(&mouse_x, &mouse_y); //get the mouse position
        std::cout << "mouse" << std::endl;
        if(e.button.button == SDL_BUTTON_LEFT)
        {
          gameGround->setMouseInput(mouse_x, mouse_y); //pass the mouse position to the gameGround object
          std::cout << "mouse left"  << std::endl;
        }
      }
    }

    //ground loop
    if(simulating)
    {
      gameGround->update(); //update the game state
    }
    //Update surface
    SDL_UpdateWindowSurface(window_ptr_);

    //if frame finished early
    auto end = SDL_GetPerformanceCounter();
    float elapsed = (end - start) / (float) SDL_GetPerformanceFrequency() * 1000.0f;
    SDL_Delay(std::floor(ticks_per_frame - elapsed)); // delay the frame to match the frame rate

    //cap frame rate
    unsigned int endTick = SDL_GetTicks();
    float frameTime = (endTick - startTick) / 1000.0f;

    // Simulation time limit
    if(endTick - firstTick > period * 1000) //if the time since the game started is greater than the period
    {
      simulating = false;
    }

    // Application time limit
    if(endTick - firstTick > (period + 5) * 1000) // if the time since the game started is greater than the period plus 5 seconds
    {

      std::cout << "SCORE: "<< gameGround->getScore() << std::endl; //print the score

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
    //checks if the id parameter passed to the function is 0, meaning that the animal being added is a sheep.
  if(id == 0)
  {
//creates a new instance of the sheep class and assigns it to a shared pointer called newSheep.
    std::shared_ptr<sheep> newSheep =
      std::make_shared<sheep>(window_surface_ptr_, sheepSpritePath);
      //sets the size of the newSheep object to the value of the animal_size variable.
    newSheep->setSize(animal_size, animal_size);
    int hw = newSheep->getWidth();
    int hh = newSheep->getHeight();
      
      // These lines generate random x and y positions for the sheep within the boundaries of the frame. The positions are calculated by adding the width and height of the sheep object, the frame boundary, and a random value generated by the rand() function. The random value is calculated by taking the modulus of the result of (frame_width - frame_boundary - hw) or (frame_height - frame_boundary - hh) and adding it to the previous values.

    int randomX =  hw + frame_boundary + (std::rand() % (frame_width - frame_boundary - hw));
    int randomY = hh + frame_boundary + (std::rand() % (frame_height - frame_boundary - hh));
    if(random)
    newSheep->setPos( randomX, randomY);
    else
      newSheep->setPos(pos.x, pos.y);

    newSheep->randomizeSpeed(-sheepSpeed, sheepSpeed);
    // adds the new sheep object to the sheeps set and the allAnimals set.
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

    // This loop goes through all the animals in the allAnimals set and creates a preyList vector containing all the objects that have the tag "prey".
  std::vector<std::shared_ptr<MovingObject>> preyList;
  for(auto a : allAnimals)
  {
    if(a->hasTag("prey"))
    {
      preyList.push_back(a);
    }
  }

  //Update all the hunters for now just wolf
    // this loop goes through all the objects in the wolves set and calls the setPreyList function on each, passing the preyList vector created in the previous loop as an argument.
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
 
//This function sets the player's speed based on input from the user.
void ground::setPlayerInput(int ix, int iy)
{
    //Calculates the magnitude of the input vector using the Pythagorean theorem
  int sqt = std::sqrt(ix*ix + iy*iy);
    //Calculates the x,y component of the unit vector in the direction of the input
  int dx = sqt == 0 ? 0 : ix / sqt;
  int dy = sqt == 0 ? 0 : iy / sqt;
    //Sets the player's speed based on the input vector, scaled by the playerSpeed variable
  player->setSpeed(dx*playerSpeed, dy*playerSpeed);
}


// This function is used to set the input for the shepherd dog character,
// based on the x and y coordinates of the mouse click.
void ground::setMouseInput(int x, int y)
{
  //Check if clicked inside playArea
  if(x < frame_boundary ||
     x >= frame_width-frame_boundary) return; // if the x value of the click is less than the frame boundary, return nothin.
  if(y < frame_boundary ||
     y >= frame_height-frame_boundary) return; //if the y value of the click is less than the frame boundary, return (do nothing)
  int dist = dog->getDistTo({x, y}); //get the distance between the dog and the point where the mouse was clicked
  if(dist < CLICK_DISTANCE && !commandingUnit) //if the distance is less than the predefined distance and the dog is not currently being commanded
  {
    std::cout << dist << "click distance" << std::endl;
    dog->startCommand(); //start commanding the dog
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
    //fills the window surface with a green color (hex code 0x02AA02).
  SDL_FillRect(window_surface_ptr_, NULL, 0x02AA02);

  //Set dog to round the player
//iterates through all elements of the allAnimals container.
  for(auto& a : allAnimals)
  {
    a->move();
    a->draw();
  }

  player->move();
  player->draw();
    
//calls the remove_dead_animals() function. It removes any animal objects that are marked as "dead" from the allAnimals container.
  remove_dead_animals();

  //Only breed sheep for now
  //calls the add_new_animals() function. It adds any new animal objects to the allAnimals container, if there is space for them.
  add_new_animals();



}

    //This function is called remove_dead_animals() and its purpose is to remove any animal objects that have been marked as "dead" from the game.
void ground::remove_dead_animals()
{
   // initializing a dead variable to false and creating an empty vector of shared pointers to MovingObject called animalRm.
    //A boolean variable to check if any animal is dead.
  bool dead = false;
    //A vector to store the animals that need to be removed.
  std::vector<std::shared_ptr<MovingObject>> animalRm;
    //Iterate through all animals in the game
  for(auto a : allAnimals)
  {
      //If the animal has the "dead" tag, add it to the removal vector and set the 'dead' variable to true
    if(a->hasTag("dead"))
    {
      animalRm.push_back(a);
      dead = true;
    }
  }
    //Remove all animals that are in the removal vector from the allAnimals set
  for(auto x : animalRm)
  {
    allAnimals.erase(x);
  }

    //If at least one animal is deads
  if(dead)
  {
      //A vector to store the wolves that need to be removed.
    std::vector<std::shared_ptr<wolf>> wolfRm;
      //Iterate through all wolves in the game
    for(auto a : wolves)
    {
        //If the wolf has the "dead" tag, add it to the removal vector
      if(a->hasTag("dead"))
      {
        wolfRm.push_back(a);
      }
    }
      //Remove all wolves that are in the removal vector from the wolves setr
    for(auto x : wolfRm)
    {
      wolves.erase(x);
    }
      //A vector to store the sheep that need to be removed.
    std::vector<std::shared_ptr<sheep>> sheepRm;
      //Iterate through all sheep in the game
    for(auto a : sheeps)
    {
        //If the sheep has the "dead" tag, add it to the removal vector
      if(a->hasTag("dead"))
      {
        sheepRm.push_back(a);
      }
    }

      //Remove all sheep that are in the removal vector from the sheeps set
    for(auto x : sheepRm)
    {
      sheeps.erase(x);
    }
      //A vector to store the list of prey
    std::vector<std::shared_ptr<MovingObject>> preyList;
      //Iterate through all animals in the game
    for(auto a : allAnimals)
    {
        //If the animal has the "prey" tag, add it to the prey list
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
    //if no animal is dead
  else
  {
      //do nothing and return
    return;
  }

}

    
void ground::add_new_animals()
{
    //iterate over all the animals in the allAnimals set
  for(auto a : allAnimals)
  {
      //nested loop to check for interaction between all pairs of animals in the allAnimals set
    for(auto b : allAnimals)
    {
        //checks if the distance between animal "a" and animal "b" is less than a predefined constant "INTERACT_DISTANCE".
      if(a->getDistTo(b->getPos()) < INTERACT_DISTANCE &&
         a->hasTag("female") && b->hasTag("male"))
      {
          //if the conditions are met, call the interact function on the animal "a" with animal "b" as the parameter
        a->interact(b);
      }
    }
  }

    //create a vector to store positions where new animals will be added
  std::vector<std::pair<int,Vec2>> addPositions;
    //iterate over all the animals in the allAnimals set
  for(auto a : allAnimals)
  {
      //check if the animal has the "child" and "sheep" tags
    if(a->hasTag("child") && a->hasTag("sheep"))
    {
        //remove the "child" tag
      a->removeTag("child");
        //store the position where the new animal will be added
      addPositions.push_back({0, a->getPos()});
    }
  }

    //iterate over the stored positions
  for(auto v : addPositions){
      //call the add_animal function to add the new animal at the stored position
    add_animal(v.first, v.second);
  }
}

    /*
     This function is a member function of the Interactable class.
     It adds a new tag to the list of tags associated with the object.
     The function takes in one parameter, a string called "tag" which represents the tag to be added.
     */
void Interactable::addTag(const std::string& tag)
{
    //checks if the object already has the tag
  if(!hasTag(tag))
    // If the object does not have the tag, the function inserts the tag into the set of tags associated with the object.
  tags.insert(tag);
}

//This function is checking if the given tag is present in the "tags" container (which can be a set or map)
bool Interactable::hasTag(const std::string& tag)
{
    //The find() function searches the container for an element with a key equivalent to k and returns an iterator to it if found, otherwise it returns an iterator to end().
      //So if the find() function returns an iterator to the end of the container, that means the tag is not present
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

    //This function is a constructor of the "RenderedObject" class.
    //It is called when a new object of this class is created.
RenderedObject::RenderedObject(SDL_Surface* window_surface_ptr, const std::string& textureFile) {
  window_surface_ptr_ = window_surface_ptr;
    //The pointer to the SDL_Surface object is stored in the "window_surface_ptr_" variable so it can be used later.
  image_ptr_ = load_surface_for(textureFile, window_surface_ptr);

  animalRect.w = image_ptr_->w;
  animalRect.h = image_ptr_->h;
  animalRect.x = 0;
  animalRect.y = 0;
}

RenderedObject::~RenderedObject()
{

}
    
    //Copy the image of the object onto the window surface, using the rectangle as the destination location and scaling the image if necessary
void RenderedObject::draw()
{
    //Create a rectangle to hold the position and size of the object
  SDL_Rect rect;
    //Set the x and y position of the rectangle to the x and y position of the object
  rect.x = x;
  rect.y = y;
    //Set the width and height of the rectangle to the width and height of the object
  rect.w = w;
  rect.h = h;
    //Copy the image of the object onto the window surface, using the rectangle as the destination location and scaling the image if necessary
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

    //This function is a constructor of the class sheep which is inherited from class animal
sheep::sheep(SDL_Surface* window_surface_ptr, const std::string& filePath)
: animal( window_surface_ptr, filePath){
  addTag("sheep");

  addTag("prey");
    //A random number between 0 and 99 is generated, if it is less than 50, addTag function is called to add the tag "female" to the object
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

    
    /*
         This function is a member function of the sheep class, which is derived from the Interactable class.
         This function is used to simulate the sheep's ability to give birth to new sheep.
         It does not do anything more than just adding the tag for
     */
void sheep::interact(std::shared_ptr<Interactable> other)
{
    //Check if the other Interactable object being interacted with is also a sheep, and that it is a male, and this sheep is female
  if(other->hasTag("sheep") && other->hasTag("male") && hasTag("female"))
  {
      //Get the current time in milliseconds
    auto now = SDL_GetTicks();
      //Check if the time since the last time this sheep had a child is less than BREED_MS
    if(now - lastChild < BREED_MS) return;
      //If the conditions are met, add the "child" tag to this sheep
    addTag("child");
    //std::cout << "Sheep pregnant!" << std::endl;
      //save the time of this interaction as the last time this sheep had a child
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
    // Get the current time in milliseconds
  int now = SDL_GetTicks();
    // If the wolf has not eaten in STARVE_MS milliseconds, it dies
  if(now - lastFood > STARVE_MS)
  {
    addTag("dead");
    return;
  }
    // Check if the dog is created
  if(dog == nullptr)
  {
    std::cout << "DOG an Wolf creation order wrong!" << std::endl;
    return;
  }
    // Get the position of the dog
  Vec2 dogPos = dog->getPos();
    // Get the distance between the wolf and the dog in x and y axis
  int dogdx = dogPos.x - x;
  int dogdy = dogPos.y - y;
    // Get the distance between the wolf and the dog
  int dogDist = (dogdx * dogdx) + (dogdy * dogdy);
    // Check the direction of the dog in x axis
  if(dogdx > 0) dogdx = 1;
  else if(dogdx < 0) dogdx = -1;
  else dogdx = 0;
    // Check the direction of the dog in y axis
  if(dogdy > 0) dogdy = 1;
  else if(dogdy < 0) dogdy = -1;
  else dogdy = 0;
    // if the distance between the wolf and the dog is less than 100 pixels
  if(std::sqrt(dogDist) < 100)
  {
      // set the speed of the wolf in the opposite direction of the dog
    setSpeed(-dogdx * wolfSpeed, -dogdy * wolfSpeed);
    setPos(x+xSpeed, y+ySpeed);
    //std::cout << "Dog close!" << std::endl;
    return;
  }

  // Dog not close
    //If there are no prey available, move randomly within the frame boundaries
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
  //iterating through a list of prey objects (of type std::shared_ptr<MovingObject>) and finding the one that is closest to the wolf.
  std::shared_ptr<MovingObject> nearest;
    // T initialized to a large value, and is updated with the distance to the nearest prey (by calling getDistTo() on each prey object)
  int minDist = 1000000; // Declare and initialize a variable minDist as 1000000
  for(auto prey : preyList)// Iterate over the preyList and for each prey
  {
    int dist = getDistTo(prey->getPos());// Calculate the distance between the current prey and the wolf using the getDistTo function
    if(minDist > dist) // If the minimum distance is greater than the distance calculated above
    {
      minDist = dist; // Update minDist with the current distance
      nearest = prey; // Update nearest with the current prey
    }
  }

  // Calculate directions to nearest sheep
    // calculates the difference in x position between the wolf and the nearest sheep
  int dX = nearest->getX() - x ;// + nearestSheep->getWidth();
    // checks if the sheep is to the right of the wolf, if so set dX to 1, if to the left, set to -1, otherwise set to 0
  if(dX > 0) dX = 1;
  else if(dX < 0) dX = -1;
  else dX = 0;
    //  calculates the difference in y position between the wolf and the nearest sheep
  int dY = nearest->getY() - y;// + nearestSheep->getHeight();
  if(dY > 0) dY = 1;
  else if(dY < 0) dY = -1;
  else dY = 0;

   // checks if the sheep is not in the same position as the wolf, if so set the speed of the wolf in the direction of the sheep
  if(dX != 0 || dY != 0)
  {
      // This line sets the speed of the wolf in the direction of the nearest sheep
    setSpeed(dX * wolfSpeed, dY * wolfSpeed);
      // This line updates the position of the wolf based on its speed
    setPos(x+xSpeed, y+ySpeed);

  }
    // This line checks if the wolf is close enough to the sheep to interact with it (hunt it)
  if(minDist < HUNT_DISTANCE)
  {
      // If the wolf is close enough, it interacts with the sheep (hunt it)
    interact(nearest);
      // This line updates the last time the wolf ate food
    lastFood = now;
  }
}


void wolf::interact(std::shared_ptr<Interactable> other)
{
    //This function is checking if the interactable object passed as an argument has the tag "prey"
  if(other->hasTag("prey"))
  {
      //if the interactable object does not have the tag "dead"
    if(!other->hasTag("dead"))
        //the tag "dead" is added to it
    other->addTag("dead");
  }
}





Player::Player(SDL_Surface* window_surface_ptr, const std::string& filePath)
  : MovingObject(window_surface_ptr, filePath)
{
  addTag("player");
}

 //   This function is the move() method of the Player class.
void Player::move()
{
  //TAKE input from player to move player
    // Assign the next x and y position of the player based on the current speed
  int nx = x+xSpeed;
  int ny = y+ySpeed;
    // Check if the next x position is outside of the game boundary and if so, set it to the boundary value
  if(nx > frame_width - frame_boundary) nx = frame_width - frame_boundary;
  else if(nx < frame_boundary)  nx = frame_boundary;
    // Check if the next y position is outside of the game boundary and if so, set it to the boundary value
  if(ny > frame_height - frame_boundary) ny = frame_height - frame_boundary;
  else if(ny < frame_boundary) ny = frame_boundary;
    // update the player's position to the calculated value
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
      //calculate the distance to the target position
    int dx = targetPos.x - x;
    int dy = targetPos.y - y;

    float dist = std::sqrt((dx * dx) + (dy * dy));
      // if the distance is less than or equal to 5 pixels
    if(dist <= 5)
    {
        //stop the move command
      moveCommand = false;
        //start moving back
      moveBack = true;
      return;
    }
      // If dx is positive move right
    if(dx > 0) dx = 1;
      // if dx is negative move left
    else if(dx < 0) dx = -1;
    else dx = 0;
      // if dy is positive move down
    if(dy > 0) dy = 1;
      // if dy is negative move up
    else if(dy < 0) dy = -1;
    else dy = 0;

      //set the dog's speed to dx * dogSpeed and dy * dogSpeed
    setSpeed(dx * dogSpeed, dy * dogSpeed);
      //set the dog's position to x + xSpeed and y + ySpeed
    setPos(x+xSpeed, y+ySpeed);
  }
  else if(moveBack)
  {
      //calculate the distance to the center of the round
    int dx = roundCenter->getX() - x;
    int dy = roundCenter->getY() - y;

    float dist = std::sqrt((dx * dx) + (dy * dy));
      //if the distance is less than or equal to the radius
    if(dist <= radius)
    {
        //stop moving back
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
    //set the dog's speed to dx * dogSpeed and dy * dogSpeed
  else
  {
      //set the speed of rotation
    float speed = dogRot;
      //calculate the x position of the dog
    int lx = radius * std::cos(angle);
      //calculate the y position of the dog
    int ly = radius * std::sin(angle);
      //increment the angle of rotation
    angle += speed;
      //if the angle is 360, set it to 0
    if(angle == 360.0f) angle = 0.0f;
    if(angle == -360.0f) angle = 0.0f;
      // set the position of the dog to the new calculated position using the angle, radius and round center coordinates
    setPos(lx + roundCenter->getX(), ly + roundCenter->getY());
  }


}
