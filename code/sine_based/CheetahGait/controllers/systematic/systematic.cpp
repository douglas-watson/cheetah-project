#include "SystematicSearchController.h"

int main(int argc, char **argv)
{
	// First we create an instance of our controller
	SystematicSearchController cont(argc,argv);

	// Then the physics needs to be initialized
	amarsi::Supervisor::instance().initSimulation();

	// Now as everything is ready, we tell our Controller that it can start
	// to control the robot.
	return cont.run();
}
