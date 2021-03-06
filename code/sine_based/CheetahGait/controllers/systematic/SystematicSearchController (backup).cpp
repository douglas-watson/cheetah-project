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

using namespace std;

#define PI 3.1415265

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

	// Open a file to put result in:
	std::ofstream file("../../result.txt");

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

  // Write headres in result file
  file << "d_amplitudeForeHip d_amplitudeHindHip d_offsetForeHip d_offsetHindHip "
       << "d_amplitudeForeKnee d_amplitudeHindKnee d_offsetKnee d_kneePhaseLag "
       << "d_frequency duration distance angle" << std::endl;

	//=====================================================================
	// Here we test only for two opened parameter for five steps.You will
	// certainly have to change these parameters and their boundaries, so
	// feel free to add,remove loops, and conditions. Please be sure to take
	// into account computation time!
	//=====================================================================
	int stepHipAmplitude;

	// A loop for hip amplitude varying from 0 to 1 in five steps
	double minHipAmplitude = 0.30;
	double maxHipAmplitude = 0.50;

	for (stepHipAmplitude = 0; stepHipAmplitude < 5; ++stepHipAmplitude)
	{
		// Fore and hind hip amplitudes are the same in this example
		d_amplitudeForeHip = stepHipAmplitude * (maxHipAmplitude - minHipAmplitude) / 4 + minHipAmplitude;
		d_amplitudeHindHip = d_amplitudeForeHip;
  
		// A loop for fore knee amplitude varying from 0 to 1 in five steps
		double minForeKneeAmplitude = 0.0;
		double maxForeKneeAmplitude = 1.0;

		for (int stepForeKneeAmplitude = 0; stepForeKneeAmplitude < 5; ++stepForeKneeAmplitude)
		{
			// The two amplitude are linked here
			d_amplitudeForeKnee = stepForeKneeAmplitude * (maxForeKneeAmplitude - minForeKneeAmplitude) / 4 + minForeKneeAmplitude;
			//d_amplitudeForeKnee = 0.3667;
      //d_amplitudeHindKnee = d_amplitudeForeKnee;
      //d_amplitudeHindKnee =0.2333;

      //A loop for controlling the hind knee amplitude
      double minHindKneeAmplitude = 0.0;
      double maxHindKneeAmplitude = 1.0;

      for (int stepHindKneeAmplitude = 0; stepHindKneeAmplitude < 5; ++stepHindKneeAmplitude)
      {
        // The two amplitude are linked here
        d_amplitudeHindKnee = stepHindKneeAmplitude * (maxHindKneeAmplitude - minHindKneeAmplitude) / 4 + minHindKneeAmplitude;
        //d_amplitudeForeKnee = 0.3667;
        //d_amplitudeHindKnee = d_amplitudeForeKnee;
        //d_amplitudeHindKnee =0.2333;


        //A loop for controlling the fore hip offset
        double minForeHipOffset = -0.05;
        double maxForeHipOffset = 0.15;

        for (int stepForeHipOffset = 0; stepForeHipOffset < 5; ++stepForeHipOffset)
        {
          d_offsetForeHip = stepForeHipOffset * (maxForeHipOffset - minForeHipOffset) / 4 + minForeHipOffset;

          //A loop for controlling the hind hip offset
          double minHindHipOffset = -0.15;
          double maxHindHipOffset = 0.05;

          for (int stepHindHipOffset = 0; stepHindHipOffset < 5; ++stepHindHipOffset)
          {
            d_offsetHindHip = stepHindHipOffset * (maxHindHipOffset - minHindHipOffset) / 4 + minHindHipOffset;

            //A loop for controlling the knee phase lag
            double minKneePhaseLag = 0.0;
            double maxKneePhaseLag = 1.0;

            for (int stepKneePhaseLag = 0; stepKneePhaseLag < 5; ++stepKneePhaseLag)
            {
              d_kneePhaseLag = stepKneePhaseLag * (maxKneePhaseLag - minKneePhaseLag) / 4 + minKneePhaseLag;

			// Control the robot with the calculated parameters
      //for(int q=0; q<5; q++){
			controlRobotForOneRun();
      
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
			// Compute the algebraic distance in front direction of
			// the robot

      // duration of simulation
      double duration = amarsi::Clock::getTime();
      
      file << duration << " ";

			Vector3 frontDirection = state.frontDirection;

			frontDirection.y() = 0.0;
			frontDirection.normalize();

			double distance = max(((state.position - state.startingPosition) * frontDirection), 0.0);
			file << distance << " ";
      
      //MAX ANGLE
      //correxpond a un test si l'angle entre les deux vecteurs est plus grand que 60 degrés
      //if(state.startingUpDirection*state.upDirection < 0.5)
      double max_angle = state.startingUpDirection*state.upDirection;
      file << max_angle << " ";

			// Note: You may want to add other values!!!

			// Close the line
			file << std::endl;

			// Reset the position and dynamics of the robot for the
			// next run.
			amarsi::Supervisor::instance().resetSimulation();
      //}
		}
	}
  }
  }
  }
  }

	//amarsi::Supervisor::instance().simulationQuit();
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


