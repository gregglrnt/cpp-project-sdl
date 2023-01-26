#include "Project_SDL1.h"
#include <stdio.h>
#include <string>
#include <time.h>

// int main(int argc, char* argv[]) {

//   //Randomize
//   std::srand(time(NULL));
//   std::cout << "Starting up the application" << std::endl;

//   if (argc != 4)
//     throw std::runtime_error("Need three arguments - "
//                              "number of sheep, number of wolves, "
//                              "simulation time\n");

//   init();

//   std::cout << "Done with initilization" << std::endl;

//   Application my_app(std::stoul(argv[1]), std::stoul(argv[2]));

//   std::cout << "Created window" << std::endl;

//   int retval = my_app.loop(std::stoul(argv[3]));

//   std::cout << "Exiting application with code " << retval << std::endl;


//   return retval;
// }

int main(int argc, char* argv[]) {
  // Initialize and create application
  Application my_app;
  my_app.run();

  return 0;
}
