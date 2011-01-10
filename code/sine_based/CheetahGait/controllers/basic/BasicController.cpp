/**
 * \file BasicController.cpp
 *
 *  \date May 14, 2010
 *  \author tuleu
 */

//#define TEST_ACTUATOR_IH_PRESENCE

#include <libamarsi-quad/utils/PreferenceReader.h>
#include "BasicController.h"
#include <cmath>
#include <iostream>

#define PI 3.1415265

BasicController::BasicController(int& argc, char** argv)
 : amarsi::Controller(argc,argv)
 , d_amplitudeHip(0.3)
 , d_offsetHip(0)
 , d_amplitudeKnee(0.4)
 , d_offsetKnee(0.6)
 , d_frequency(0.5){
}

BasicController::~BasicController() {

}

void BasicController::userInitSimulation(){
  //add some motion restriction
  amarsi::Supervisor::
    instance().configureMotionRestriction("0.0:SAGITAL_FIXED_PITCH|2.0:SAGITAL_FREE_PITCH|4.0:FREE");

  amarsi::Supervisor::instance().setTrackingOn();
  amarsi::Supervisor::
    instance().setDisplayMode( amarsi::SHOW_GROUND_REACTION_FORCES);
}
void BasicController::userInitEnvironment(){
  //put the robot on a slope of angle d_angle in the front direction.
  amarsi::Supervisor::
    instance().setSlope(true,d_angle);
}



int BasicController::run(){
  //we iterate for different slopes
  for(unsigned int i = 0; i<10 ; ++i){
    while(amarsi::Clock::getTime() < 10.0){
      //perform a basic open loop trot

      //compute the phase
      double phase(2*PI*amarsi::Clock::getTime()*d_frequency);
      //adjacents Limbs are dephased from PI
      actuator(amarsi::LEFT_FORE_HIP ).setCommand(d_amplitudeHip*std::cos(phase)
                                      +d_offsetHip);//command for fore hip joint are betweem -2.0 1.2

      actuator(amarsi::RIGHT_FORE_HIP).setCommand(d_amplitudeHip*std::cos(phase+PI)
                                      +d_offsetHip);//command for fore hip joint are betweem -2.0 1.2
      actuator(amarsi::LEFT_HIND_HIP ).setCommand(d_amplitudeHip*std::cos(phase+PI)
                                      +d_offsetHip);//command for fore hip joint are betweem -1.9 1.3
      actuator(amarsi::RIGHT_HIND_HIP).setCommand(d_amplitudeHip*std::cos(phase)
                                      +d_offsetHip);//command for fore hip joint are betweem -1.9 1.3
      while(phase>=2*PI)
        phase-=2*PI;//getting modulo 2*PI

      //LEFT FORE is retracting while phase<=PI, so knee is extended
      actuator(amarsi::LEFT_FORE_KNEE ).setOrder((phase<=PI?0.0:d_amplitudeKnee)+d_offsetKnee);
      //command for knee joint are between [0.0 1.98]

      //RIGHT FORE is retracting while phase>PI, so knee is extended
      actuator(amarsi::RIGHT_FORE_KNEE).setOrder((phase<=PI?d_amplitudeKnee:0.0)+d_offsetKnee);
      //command for knee joint are between [0.0 1.98]

      //in phase with RIGHT FORE
      actuator(amarsi::LEFT_HIND_KNEE ).setOrder((phase<=PI?d_amplitudeKnee:0.0)+d_offsetKnee);
      //command for knee joint are between [0.0 1.98]

      //in phase with LEFT FORE
      actuator(amarsi::RIGHT_HIND_KNEE).setOrder((phase<=PI?0.0:d_amplitudeKnee)+d_offsetKnee);
      //command for knee joint are between [0.0 1.98]

      //minimal available step;
      step();
    }
    d_angle+=0.05;
    amarsi::Supervisor::instance().resetSimulation();
  }
  amarsi::Supervisor::instance().simulationQuit();
  return 0;
}
