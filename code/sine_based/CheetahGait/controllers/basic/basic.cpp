#include "BasicController.h"


int main(int argc, char ** argv){
  //first we create an instance of our amarsi::Controller
  BasicController cont(argc,argv);
  //then the physics need to be initialized, but the Controller must be created.
  amarsi::Supervisor::instance().initSimulation();
  //now as everything is ready, we tell our Controller that it can start
  //to control the robot.
  return cont.run();
}

/**
 * \example BasicControllerExample
 *
 * <h1>Creation of a controller with libamarsi-quad</h1>
 *
 * This example show how to create a controller with libamarsi-quad.
 * This controller will just perform a in phase trot, using uncoupled sinusoïdal
 * oscillators. Therobot will be placed on slopes of different sizes, with
 * increased angle.
 *
 * \note The goal of this example is to provide a guided tour of the
 * API, not to provide an example of an efficient gait or method.
 *
 * \section basic architecture of the controller
 *
 * First we decide to create our own controller BasicController that derive
 * from Controller, and we had a method run() :
 * \dontinclude BasicController.h
 * \skip include
 * \until run();
 *
 * The class also defines some private fields that parametrize the resulting gait.
 *
 * \skip private
 * \until };
 *
 * Then the structure of the implementation is really simple. in run() we just
 * make a double loop, one for increasing the size of the slope, and the other
 * for controlling the robot for 10 second before putting him on another slope:
\code
int BasicController::run(){
  for(unsigned int i = 0; i < 10; ++i){
    while(amarsi::Clock::getTime()<10.0){

      //place your control here (read from sensor, write to Actuator ... )

      step();//run the physic simulation for one step.
    }
    //increase the angle of the slope
    d_angle += 0.05;
    amarsi::Supervisor::instance().resetSimulation();
  }
  amarsi::Supervisor::instance().quitSimulation();
  return 0;
}
\endcode
 * Now we must remember that the initialization of the simulated world must be
 * handled manually. So we must call Supervisor::initSimulation() before any other
 * call to the amarsi::Controller interface. But Supervisor::initSimulation()
 * need that a amarsi::Controller is created.... So we place it ideally between
 * the creation of our BasicController and the call to BasicController::run() :
 *
 * \include basic.cpp
 *
 * \section useActuator Using Actuator and Sensor .
 *
 * The next step is to use actuator and sensor. so in our main loop we just
 * update the phase of the oscillator and the order to the
 * different actuators :
 * \dontinclude BasicController.cpp
 * \skip //compute the phase
 * \until actuator(amarsi::RIGHT_HIND_KNEE).setOrder((phase<=PI?0.0:d_amplitudeKnee)+d_offsetKnee);
 *
 * \section supervisor Controlling the Simulation with the Supervisor.
 *
 * The next step is to use the Supervisor to have some control over the
 * simulation :
 *  - Reset the simulation of the robot every 10.0 seconds
 *  - Restrict the motion of the robot to its sagital plane at the beginning of
 *    each iteration
 *  - Put the robot at each iteration on a slope
 *
 * for the first one, we just call amarsi::Supervisor::resetSimulation() after
 * exiting our main loop.
 *
 * \note a call to amarsi::Supervisor::resetSimulation() is really different than a call
 * to webots::Supervisor::simulationRevert(), as it will not quit the controller,
 * but just reset position, inertia of the physics and Clock of the simulation.
 *
 * \subsection restrictionOfTheMotion Restriction of the robot motion.
 *
 * Next we want to :
 *  - restrict completely the motion of the robot to its sagital plane for the
 *    first two second of the simulation
 *  - release it a little bit, by allowing a pitch movement of the robot for the
 *    next 2 seconds.
 *  - leave it completely motion free for the end of the simulation.
 *
 * This could be done by using the amarsi::Supervisor::configureMotionRestrictor() method.
 * It takes one string argument, that specify each wanted state of the
 * MotionRestriction for a specific time using the syntax
 * \c "time:STATE | time:STATE | ....." where \c STATE is either :
 *  - \c FREE : for leaving the robot free
 *  - \c SAGITAL_FREE_PITCH, to restrict the motion into the robot sagital plane, but
 *    allowing the pitch movement.
 *  - \c SAGITAL_FIXED_PITCH, to restrict the motion into the robot sagital plane,
 *    and disallow any pitch movement.
 *
 *    In our example, the configuration string will be :
 *    "0.0:SAGITAL_FIXED_PITCH|2.0:SAGITAL_FREE_PITCH|4.0:FREE". We want to call
 *    Supervisor::configureMotionRestrictor at each beginning of a simulation,
 *    so we redefine the amarsi::Controller::userInitSimulation() virtual method in our
 *    BasicController :
 *    \dontinclude BasicController.cpp
 *    \skip void BasicController::userInitSimulation(){
 *    \until }
 *
 * In this loop we also adde two other nice feature :
 * - We enable the drawing of the ground reaction forces on the feet of the cheetah.
 * - We enable a tracking of the robot, that doesn't follow the vertical motion
 *   of the robot. It helps to better see the gait of the robot by removing these
 *   unwanted camera moves. However these features is still a little bit buggy
 *   and doesn't intercat well with users camera moves/panning/zooming. You may
 *   want to disable it by commenting this line
 *
 * \subsection Put the robot on a slope
 *
 *
 *
 * the next step is to use to put the robot on a slope. For this purpose we just
 * use amarsi::Supervisor::setSlope() method. However we cannont do it in
 * userInitSimulation(), as it require an initialization of the physics that
 * will callback userInitSimulation back. This method has to be placed in
 * userInitEnvironment.
 *
 * \dontinclude BasicController.cpp
 * \skip void BasicController::userInitEnvironment(){
 *    \until }
 * \section prefs Using Preferences Mechanism to parameterize the gait.
 *
 *
 */
