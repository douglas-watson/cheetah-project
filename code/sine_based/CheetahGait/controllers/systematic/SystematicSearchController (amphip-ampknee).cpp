/**
 * \file BasicController.cpp
 *
 *  \date October 21, 2010
 *  \author tuleu
 */

//#define TEST_ACTUATOR_IH_PRESENCE

#include <libamarsi-quad/utils/PreferenceReader.h>
#include "SystematicSearchController.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <webots/Supervisor.hpp>
#include <string.h>
#include <stdlib.h>
#include <string>


using namespace std;

#define PI 3.1415265

std::string getImageFile(std::string value) {
  std::ostringstream o;
  if (!(o << "../../" << value << ".jpg"))
    return "";
  return o.str();
}

std::string getResultFile(std::string value) {
  std::ostringstream o;
  if (!(o << "../../" << value << ".txt"))
    return "";
  return o.str();
}


SystematicSearchController::SystematicSearchController(int& argc, char** argv)
:
	amarsi::Controller(argc, argv)
{
}

SystematicSearchController::~SystematicSearchController()
{
}

void SystematicSearchController::userInitSimulation()
{
	// Add some motion restriction
	//amarsi::Supervisor::instance().configureMotionRestriction("0.0:SAGITAL_FIXED_PITCH|1.0:SAGITAL_FREE_PITCH|2.0:FREE");
  amarsi::Supervisor::instance().configureMotionRestriction("0.0:FREE");
}

void SystematicSearchController::userInitEnvironment()
{
	// Put the robot on a slope of angle d_angle in the front direction.
	amarsi::Supervisor::instance().setFlatFloor();
}

double SystematicSearchController::stepFunction(double amplitude, double phase, double offset)
{
	double phaseNorm = fmod(phase, 2 * PI);

	if (phaseNorm > PI)
	{
		amplitude = 0;
	}

	return amplitude + offset;
}

double SystematicSearchController::sineFunction(double amplitude, double phase, double offset)
{
	return amplitude * cos(phase) + offset;
}

void SystematicSearchController::generateTrotTrajectories()
{
	double phase = 2 * PI * amarsi::Clock::getTime() * d_frequency;
	double dephase = phase + PI;

	double commandLeftForeHip;
	double commandRightForeHip;

	// Calculate joint angles for fore hip
	commandLeftForeHip = sineFunction(d_amplitudeForeHip, phase, d_offsetForeHip);
	commandRightForeHip = sineFunction(d_amplitudeForeHip, dephase, d_offsetForeHip);

	double commandLeftHindHip;
	double commandRightHindHip;

	// Calculate joint angles for hind hip
	commandLeftHindHip = sineFunction(d_amplitudeHindHip, dephase, d_offsetHindHip);
	commandRightHindHip = sineFunction(d_amplitudeHindHip, phase, d_offsetHindHip);

	double commandLeftForeKnee;
	double commandRightForeKnee;

	// Calculate joint angles for fore knee
	commandLeftForeKnee = stepFunction(d_amplitudeForeKnee, phase + d_kneePhaseLag, d_offsetKnee);
	commandRightForeKnee = stepFunction(d_amplitudeForeKnee, dephase + d_kneePhaseLag, d_offsetKnee);

	double commandLeftHindKnee;
	double commandRightHindKnee;

	// Calculate joint angles for hind knee
	commandLeftHindKnee = stepFunction(d_amplitudeHindKnee, dephase + d_kneePhaseLag, d_offsetKnee);
	commandRightHindKnee = stepFunction(d_amplitudeHindKnee, phase + d_kneePhaseLag, d_offsetKnee);

	// Actuate fore hip
	actuator(amarsi::LEFT_FORE_HIP).setCommand(commandLeftForeHip);
	actuator(amarsi::RIGHT_FORE_HIP).setCommand(commandRightForeHip);

	// Actuate hind hip
	actuator(amarsi::LEFT_HIND_HIP).setCommand(commandLeftHindHip);
	actuator(amarsi::RIGHT_HIND_HIP).setCommand(commandRightHindHip);

	// Actuate fore knee
	actuator(amarsi::LEFT_FORE_KNEE).setCommand(commandLeftForeKnee);
	actuator(amarsi::RIGHT_FORE_KNEE).setCommand(commandRightForeKnee);

	// Actuate hind knee
	actuator(amarsi::LEFT_HIND_KNEE).setCommand(commandLeftHindKnee);
	actuator(amarsi::RIGHT_HIND_KNEE).setCommand(commandRightHindKnee);
}


int SystematicSearchController::run()
{
	const amarsi::State & state = amarsi::Supervisor::instance().state();

	
  
  // get kneePhaseLag from preference file
  std::ifstream pref_file;
  pref_file.open ("../../pref_file"); 
	std::string pref_value;
  std::string dir = "../../amplitudeHip-";
  pref_file >> pref_value;

  std::string image_file;
	std::string result_file;
	
	image_file = dir + pref_value + ".jpg";
  std::cout << image_file;
	result_file = dir + pref_value + ".txt";
  
  // Open a file to put result in:
	std::ofstream file(result_file.c_str());



	//=====================================================================
	// INITIALIZATION OF THE SYSTEMATIC SEARCH
	//=====================================================================
	// Here we initialize the systematic serach, by frozing some parameters
	// to some fixed value, and defining on what interval the other are left
	// open. In this example we made arbritraty choice but its up to you to
	// find the right set of parameter you left open, in term of what
	// interpretation you will be able to get from the result, and time of
	// computation.
	//=====================================================================

  double d_amplitudeForeHip_t,	d_amplitudeHindHip_t, d_amplitudeForeKnee_t, d_amplitudeHindKnee_t;
	double d_offsetForeHip_t, d_offsetHindHip_t, d_kneePhaseLag_t;


//GAIT 1
	d_amplitudeForeHip_t  = atof(pref_value.c_str());
	d_amplitudeHindHip_t  = atof(pref_value.c_str());
	d_amplitudeForeKnee_t = 0.6333;
	d_amplitudeHindKnee_t = 0.3667;
	// Offsets
	d_offsetForeHip_t = 0.0667;
	d_offsetHindHip_t = -0.0667;
	d_offsetKnee    = 0.2;  //not a variable
	// Phase lag between knee and hip
	//d_kneePhaseLag_t = 0.8;
  d_kneePhaseLag_t = 0.8;

  
	// Frequency of the gait
	d_frequency = 2;     //not a variable
  
/*
//GAIT 2
	d_amplitudeForeHip_t  = 0.4;
	d_amplitudeHindHip_t  = 0.4;
	d_amplitudeForeKnee_t = 0.6333;
	d_amplitudeHindKnee_t = 0.1;
	// Offsets
	d_offsetForeHip_t = -0.0667;
	d_offsetHindHip_t = 0.2;
	d_offsetKnee    = 0.02;  //not a variable
	// Phase lag between knee and hip
	d_kneePhaseLag_t = 1.2;
	// Frequency of the gait
	d_frequency = 2.0;
  //not a variable
  */
/*
//GAIT 3
	d_amplitudeForeHip_t  = 0.4;
	d_amplitudeHindHip_t  = 0.4;
	d_amplitudeForeKnee_t = 0.9;
	d_amplitudeHindKnee_t = 0.3667;
	// Offsets
	d_offsetForeHip_t = 0.2;
	d_offsetHindHip_t = -0.2;
	d_offsetKnee    = 0.2;  //not a variable
	// Phase lag between knee and hip
	d_kneePhaseLag_t = 1.2;
	// Frequency of the gait
	d_frequency = 1.5;     //not a variable
*/
/*
	// Amplitude parameters
	d_amplitudeForeHip  = 0.3667;
	d_amplitudeHindHip  = 0.3667;
	d_amplitudeForeKnee = 0.3667;
	d_amplitudeHindKnee = 0.2333;

	// Offsets
	d_offsetForeHip = 0.0;
	d_offsetHindHip = 0.0;
	d_offsetKnee    = 0.0;

	// Phase lag between knee and hip
	d_kneePhaseLag = 0.0;

	// Frequency of the gait
	d_frequency = 2.0;

	// Duration of one run in seconds
	d_duration = 15.0;
*/

d_duration = 15.0;

  // Write headres in result file
  file << "d_amplitudeForeHip d_amplitudeHindHip d_offsetForeHip d_offsetHindHip "
       << "d_amplitudeForeKnee d_amplitudeHindKnee d_offsetKnee d_kneePhaseLag "
       << "d_frequency d_duration distance angle stability" << std::endl;

	//=====================================================================
	// Here we test only for two opened parameter for five steps.You will
	// certainly have to change these parameters and their boundaries, so
	// feel free to add,remove loops, and conditions. Please be sure to take
	// into account computation time!
	//=====================================================================
		
    
  //A loop for controlling the hip amplitude
  int nb_steps_i = 1; //number of steps around the point proposed by the gait
  double ss_i = 0.1; //step size around the gait
	for (int i=0; i < nb_steps_i; ++i)
	{
		// Fore and hind hip amplitudes are the same in this example
		d_amplitudeForeHip = (((nb_steps_i-1)/2) - i)*ss_i + d_amplitudeForeHip_t;
		d_amplitudeHindHip = d_amplitudeForeHip;
  
		//A loop for controlling the fore knee amplitude
    int nb_steps_j = 20; //number of steps around the point proposed by the gait
    double ss_j = 0.02; //step size around the gait
		for (int j=0; j < nb_steps_j; ++j)
		{
			d_amplitudeForeKnee = (((nb_steps_j-1)/2) - j)*ss_j + d_amplitudeForeKnee_t;

      //A loop for controlling the hind knee amplitude
      int nb_steps_k = 20; //number of steps around the point proposed by the gait
      double ss_k = 0.02; //step size around the gait
      for (int k=0; k < nb_steps_k; ++k)
      {
        d_amplitudeHindKnee = (((nb_steps_k-1)/2) - k)*ss_k + d_amplitudeHindKnee_t;

        //A loop for controlling the fore hip offset
        int nb_steps_l = 1; //number of steps around the point proposed by the gait
        double ss_l = 0.1; //step size around the gait
        for (int l=0; l < nb_steps_l; ++l)
        {
          d_offsetForeHip = (((nb_steps_l-1)/2) - l)*ss_l + d_offsetForeHip_t;

          //A loop for controlling the hind hip offset
          int nb_steps_m = 1; //number of steps around the point proposed by the gait
          double ss_m = 0.1; //step size around the gait
          for (int m=0; m < nb_steps_m; ++m)
          {
            d_offsetHindHip = (((nb_steps_m-1)/2) - m)*ss_m + d_offsetHindHip_t;

            //A loop for controlling the knee phase lag
            int nb_steps_n = 1; //number of steps around the point proposed by the gait
            double ss_n = 0.1; //step size around the gait
            for (int n=0; n < nb_steps_n; ++n)
            {
              d_kneePhaseLag = (((nb_steps_n-1)/2) - n)*ss_n + d_kneePhaseLag_t;


			// Control the robot with the calculated parameters
      
      //Boucle pour l'orientation du monde
     // for(double slope=-0.5; slope<0.5; slope += 0.1){
			
      //amarsi::Supervisor::instance().setSlope(true, slope);
      
      //controlRobotForOneRun();
      
      double max_angle(1);
      double stability(0);
      
      //const amarsi::State &state = amarsi::Supervisor::instance().state();
      
      
      // Run the simulation for d_duration seconds
      while (amarsi::Clock::getTime() < d_duration)
      {
        // Set all the desired joint angles
        generateTrotTrajectories();

        // Simulate one timestep
        step();

        //=============================================================
        // TEST OF THE STATE OF THE ROBOT
        // ============================================================
        // Here we test the state of the robot, and if it is in a bad
        // situation, (for example it as fallen on the ground), we stop
        // the run to spare some computation time. You can (and we
        // encourage you) remove or add test here
        // ============================================================

      //MAX ANGLE
      //correxpond a un test si l'angle entre les deux vecteurs est plus grand que 60 degrés
      //if(state.startingUpDirection*state.upDirection < 0.5)
      if (state.startingUpDirection*state.upDirection < max_angle)
        max_angle = state.startingUpDirection*state.upDirection;

      //STABILITY
      //The higher the less stable
      stability += state.startingUpDirection*state.upDirection;


        if (state.upDirection * state.startingUpDirection < cos(80 * PI / 180.0))
        {
          // Here the robot has a roll or pitch of more than 80
          // degrees
          break;
        }
      }
      
			//=====================================================
			// OUTPUT SECTION
			//=====================================================
			// Here we compute the value we monitor. We give some
			// example as comment, but there is better way to do it,
			// and better value to monitor.
			//=====================================================

			// First we output the parameters used for the run
			file << d_amplitudeForeHip << " "
			     << d_amplitudeHindHip << " "
			     << d_offsetForeHip << " "
			     << d_offsetHindHip << " "
			     << d_amplitudeForeKnee << " "
			     << d_amplitudeHindKnee << " "
			     << d_offsetKnee << " "
			     << d_kneePhaseLag << " "
			     << d_frequency << " ";
			     
      double duration = amarsi::Clock::getTime();
           
      file << duration << " ";

			// Compute the algebraic distance in front direction of
			// the robot

			Vector3 frontDirection = state.frontDirection;

			frontDirection.y() = 0.0;
			frontDirection.normalize();

			double distance = max(((state.position - state.startingPosition) * frontDirection), 0.0);
			file << distance << " ";
      
      file << max_angle << " ";
      
      stability /= duration;
      file << stability << " ";

			// Note: You may want to add other values!!!

			// Close the line
			file << std::endl;

			// Reset the position and dynamics of the robot for the
			// next run.
			amarsi::Supervisor::instance().resetSimulation();
        //cout << image_file << endl;
      //}
		}
	}
  }
  }
  }
  }
    
 // webots::Supervisor().exportImage(image_file, 100);
  
	amarsi::Supervisor::instance().simulationQuit();
	return 0;
}

void SystematicSearchController::controlRobotForOneRun()
{
	const amarsi::State &state = amarsi::Supervisor::instance().state();

	// Run the simulation for d_duration seconds
	while (amarsi::Clock::getTime() < d_duration)
	{
		// Set all the desired joint angles
		generateTrotTrajectories();

		// Simulate one timestep
		step();

		//=============================================================
		// TEST OF THE STATE OF THE ROBOT
		// ============================================================
		// Here we test the state of the robot, and if it is in a bad
		// situation, (for example it as fallen on the ground), we stop
		// the run to spare some computation time. You can (and we
		// encourage you) remove or add test here
		// ============================================================

		if (state.upDirection * state.startingUpDirection < cos(80 * PI / 180.0))
		{
			// Here the robot has a roll or pitch of more than 80
			// degrees
			return;
		}
	}
}


