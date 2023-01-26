#include "Project_SDL1.h"
#include <random>

// Initialize SDL
void init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    throw std::runtime_error("SDL could not initialize! SDL_Error: " +
                             std::string(SDL_GetError()));

  if (!IMG_Init(IMG_INIT_PNG))
    throw std::runtime_error("SDL image could not initialize! SDL_Error: " +
                             std::string(IMG_GetError()));
}


// Animal base class
Animal::Animal(SDL_Surface* texture, unsigned x, unsigned y)
    : texture_(texture), x_(x), y_(y) {}

void Animal::draw(SDL_Surface* screen) {
  // Create destination rectangle
  SDL_Rect dst_rect;
  dst_rect.x = x_;
  dst_rect.y = y_;
  dst_rect.w = texture_->w;
  dst_rect.h = texture_->h;

  // Draw texture to screen
  SDL_BlitScaled(texture_, nullptr, screen, &dst_rect);
}

// Sheep class
Sheep::Sheep(SDL_Surface* texture, unsigned x, unsigned y)
    : Animal(texture, x, y) {}

void Sheep::move() {
  // Move sheep along straight line
  x_ += 1;
  y_ += 1;

  // Check if sheep has reached edge of screen and bounce back if necessary
  if (x_ + texture_->w > arena_width)
    x_ = arena_width - texture_->w;
  if (y_ + texture_->h > arena_height)
    y_ = arena_height - texture_->h;
}

// Wolf class
Wolf::Wolf(SDL_Surface* texture, unsigned x, unsigned y)
    : Animal(texture, x, y) {}

void Wolf::move() {
  // Move wolf randomly within arena bounds
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist_x(0, arena_width - texture_->w);
  std::uniform_int_distribution<int> dist_y(0, arena_height - texture_->h);
  x_ = dist_x(mt);
  y_ = dist_y(mt);
}

// Ground class
Ground::Ground(SDL_Surface* screen, Application& app)
    : screen_(screen), frame_count_(0), app_(app) {}

Ground::~Ground() {
  // Destroy animals
  for (auto& animal : animals_)
    animal.reset();
}

void Ground::loop() {
  // Clear screen
  SDL_FillRect(screen_, nullptr, SDL_MapRGB(screen_->format, 0, 0, 0));

  for (auto& animal : animals_)
    animal->move();
  
  // Draw all animals to screen
  for (auto& animal : animals_)
    animal->draw(screen_);
  
  // Update window surface
  SDL_UpdateWindowSurface(app_.window_);
  
  // Increment frame counter
  ++frame_count_;
  
  // Delay to maintain desired frame rate
  SDL_Delay(frame_time * 1000);
}

void Ground::add_animal(std::unique_ptr<Animal> animal) {
  animals_.push_back(std::move(animal));
}

void Ground::remove_animal(Animal* animal) {
  // Find animal in vector and remove it
  for (auto it = animals_.begin(); it != animals_.end(); ++it) {
    if (it->get() == animal) {
      animals_.erase(it);
      break;
    }
  }
}

// Application class
Application::Application() {
  // Create window
  window_ = SDL_CreateWindow("SDL Project", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, frame_width, frame_height,
                             SDL_WINDOW_SHOWN);
  if (!window_)
    throw std::runtime_error("Window could not be created! SDL_Error: " +
                             std::string(SDL_GetError()));
  
  // Get window surface
  surface_ = SDL_GetWindowSurface(window_);
  
  // Load sheep texture
  sheep_texture = IMG_Load("../../media/sheep.png");
  if (!sheep_texture)
    throw std::runtime_error("Could not load sheep texture! SDL_Error: " +
                             std::string(IMG_GetError()));
  
  // Load wolf texture
  wolf_texture = IMG_Load("../../media/wolf.png");
  if (!wolf_texture)
    throw std::runtime_error("Could not load wolf texture! SDL_Error: " +
                             std::string(IMG_GetError()));
  
  // Create ground
  ground_ = std::make_unique<Ground>(surface_, *this);
}

Application::~Application() {
  // Destroy ground
  ground_.reset();
  
  // Destroy window
  SDL_DestroyWindow(window_);
  
  // Free sheep texture
  SDL_FreeSurface(sheep_texture);
  
  // Free wolf texture
  SDL_FreeSurface(wolf_texture);
}

void Application::run() {
  // Main loop
  while (true) {
    ground_->loop();
  }
}
