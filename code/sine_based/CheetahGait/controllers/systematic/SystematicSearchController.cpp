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
#include <libamarsi-quad/model/LegKinematicModel.h>


using namespace std;

#define PI 3.1415265


// Open a file to put result in:
std::ofstream file_time("../../../output/result_time.txt");
std::ofstream file_stats("../../../output/result_stats.txt");
std::ofstream file_leg_trajectory("../../../output/result_leg_trajectory.txt");
std::ofstream file_leg_trajectory_rframe("../../../output/result_leg_trajectory_rframe.txt");
std::ofstream file_hip_trajectory("../../../output/result_hip_trajectory.txt");
std::ofstream file_knee_trajectory("../../../output/result_knee_trajectory.txt");
std::ofstream file_rob_trajectory("../../../output/result_rob_trajectory.txt");
std::ofstream file_forces("../../../output/result_forces.txt");

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

Vector3 SystematicSearchController::GetLegPosition(amarsi::Limbs leg){
  const amarsi::State& state = amarsi::Supervisor::instance().state();
  const amarsi::LegKinematicModel &myLeg = state.legModel(leg);
  return myLeg.positionInBodyFrame();
}

Vector3 SystematicSearchController::GetGroundReactionForces(amarsi::Limbs leg)
{
  const amarsi::State& state = amarsi::Supervisor::instance().state();
  return state.GRFs[static_cast<size_t>(leg)];
}

double SystematicSearchController::getPitchAngle()
{

	//gets the state of the robot :
	const amarsi::State & state = amarsi::Supervisor::instance().state();
	double frontDirectionY = state.frontDirection[1] ;

	//returns the angle in radians
	// return asin( frontDirectionY );

	//alternatively you can return a rounded value in degrees.
	return (asin( frontDirectionY ) * 180 / M_PI);
}

double SystematicSearchController::getRollAngle()
{

	//gets the state of the robot :
	const amarsi::State & state = amarsi::Supervisor::instance().state();
	double sideDirectionY = state.sideDirection[1] ;

	//returns the angle in radians
	// return asin( sideDirectionY );

	//alternatively you can return a rounded value in degrees.
	return (asin( sideDirectionY ) * 180 / M_PI);
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
  amarsi::Supervisor::instance().setTrackingOn();

}


void SystematicSearchController::userInitEnvironment()
{
	// Put the robot on a slope of angle d_angle in the front direction.
	amarsi::Supervisor::instance().setFlatFloor();
	// amarsi::Supervisor::instance().setStand();
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

	file_hip_trajectory << commandLeftForeHip << " "
	 	 << commandLeftHindHip << " "
	 	 << commandRightForeHip << " "
	 	 << commandRightHindHip << " ";

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

	file_knee_trajectory << commandLeftForeKnee << " "
	 	 << commandLeftHindKnee << " "
	 	 << commandRightForeKnee << " "
	 	 << commandRightHindKnee << " ";

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

	d_duration = 30.0;

	// Write headers in result file
	file_stats << "amplitudeForeHip amplitudeHindHip offsetForeHip offsetHindHip "
		<< "amplitudeForeKnee amplitudeHindKnee offsetKnee kneePhaseLag "
		<< "frequency duration distance angle stability" << std::endl;
	file_time << "time distance" << std::endl;

	//=====================================================================
	// INITIALIZATION OF THE SYSTEMATIC SEARCH
	//=====================================================================
	// Here we initialize the systematic serach, by frozing some parameters
	// to some fixed value, and defining on what interval the other are left
	// open. In this example we made arbritraty choice but its up to you to
	// find the right set of parameter you left open, in term of what
	// interpretation you will be able to get from the result, and time of
	// computation.
	//~ //=====================================================================

//~ //GAIT 1
	//~ d_amplitudeForeHip  = 0.4;
	//~ d_amplitudeHindHip  = 0.4;
	//~ d_amplitudeForeKnee = 0.6333;
	//~ d_amplitudeHindKnee = 0.3667;
	//~ // Offsets
	//~ d_offsetForeHip = 0.0667;
	//~ d_offsetHindHip = -0.0667;
	//~ d_offsetKnee    = 0.2;  //not a variable
	//~ // Phase lag between knee and hip
	//~ d_kneePhaseLag = 0.8;
  //~ //d_kneePhaseLag_t = atof(pref_value.c_str());

	// Frequency of the gait
	//~ d_frequency = 2;     //not a variable
  

  //~ //Best gait
	//~ d_amplitudeForeHip  = 0.5;
	//~ d_amplitudeHindHip  = 0.5;
	//~ d_amplitudeForeKnee = 0.6133;
	//~ d_amplitudeHindKnee = 0.5067;
	//~ // Offsets
	//~ d_offsetForeHip = 0.0667;
	//~ d_offsetHindHip = -0.0667;
	//~ d_offsetKnee    = 0.2;  //not a variable
	//~ // Phase lag between knee and hip
	//~ d_kneePhaseLag = 0.8;
  
	//~ // Frequency of the gait
	//~ d_frequency = 2;     //not a variable
  
  //~ // Fall over gait
	d_amplitudeForeHip  = 0.5;
	d_amplitudeHindHip  = 0.5;
	d_amplitudeForeKnee = 0.6133;
	d_amplitudeHindKnee = 0.5267;
	// Offsets
	d_offsetForeHip = 0.0667;
	d_offsetHindHip = -0.0667;
	d_offsetKnee    = 0.2;  //not a variable
	// Phase lag between knee and hip
	d_kneePhaseLag = 0.8;
  
	// Frequency of the gait
	d_frequency = 2;     //not a variable
  
    
  //Most stable gait
	//~ d_amplitudeForeHip  = 0.25;
	//~ d_amplitudeHindHip  = 0.25;
	//~ d_amplitudeForeKnee = 0.47
  33;
	//~ d_amplitudeHindKnee = 0.2867;
	//~ // Offsets
	//~ d_offsetForeHip = 0.0667;
	//~ d_offsetHindHip = -0.0667;
	//~ d_offsetKnee    = 0.2;  //not a variable
	//~ // Phase lag between knee and hip
	//~ d_kneePhaseLag = 0.8;
  //~ //d_kneePhaseLag_t = atof(pref_value.c_str());
  
	// Frequency of the gait
	//~ d_frequency = 2;     //not a variable


//~ // FASTEST GAIT
	//~ d_amplitudeForeHip  = 0.6;
	//~ d_amplitudeHindHip  = 0.6;
	//~ d_amplitudeForeKnee = 0.4733;
	//~ d_amplitudeHindKnee = 0.2667;
	//~ // Offsets
	//~ d_offsetForeHip = 0.0667;
	//~ d_offsetHindHip = -0.0667;
	//~ d_offsetKnee    = 0.2;  //not a variable
	//~ // Phase lag between knee and hip
	//~ d_kneePhaseLag = 0.8;
	//~ // Frequency of the gait
	//~ d_frequency = 2.0;
  //~ //not a variable


//~ // Second Fastest gait
	//~ d_amplitudeForeHip_t  = 0.6;
	//~ d_amplitudeHindHip_t  = 0.6;
	//~ d_amplitudeForeKnee_t = 0.6133;
	//~ d_amplitudeHindKnee_t = 0.1667;
	//~ // Offsets
	//~ d_offsetForeHip_t = 0.0667;
	//~ d_offsetHindHip_t = -0.0667;
	//~ d_offsetKnee    = 0.2;  //not a variable
	//~ // Phase lag between knee and hip
	//~ d_kneePhaseLag_t = 0.8;
  //~ //d_kneePhaseLag_t = atof(pref_value.c_str());

	//~ // Frequency of the gait
	//~ d_frequency = 2;     //not a variable


	double max_angle(1);
	double stability(0);

	//Get the position of the legs of the robot
	double xForeLeft, xHindLeft;
	double xForeRight, xHindRight;
	double yForeLeft, yHindLeft;
	double yForeRight, yHindRight;
	double fxForeLeft, fxHindLeft, fxForeRight, fxHindRight;
	double fyForeLeft, fyHindLeft, fyForeRight, fyHindRight;

	file_hip_trajectory 
		<< "setHForeLeft setHHindLeft setHForeRight setHHindRight " 
		<< "realHForeLeft realHHindLeft realHForeRight realHHindRight "
		<< "distance" << std::endl;

	file_knee_trajectory 
		<< "setKForeLeft setKHindLeft setKForeRight setKHindRight " 
		<< "realKForeLeft realKHindLeft realKForeRight realKHindRight "
		<< "distance" << std::endl;

	file_leg_trajectory_rframe << "distance xForeLeft yForeLeft xHindLeft yHindLeft "
		<< "xForeRight yForeRight xHindRight yHindRight" << std::endl;
	file_leg_trajectory << "distance xForeLeft yForeLeft xHindLeft yHindLeft "
		<< "xForeRight yForeRight xHindRight yHindRight" << std::endl;
	file_forces << "distance fxForeLeft fyForeLeft fxHindLeft fyHindLeft "
		<< "fxForeRight fyForeRight fxHindRight fyHindRight" << std::endl;
	file_rob_trajectory << "distance x y z pitch roll" << std::endl;
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

		//Get the position of the legs of the robot

		Vector3 frontDirection = state.frontDirection;
		frontDirection.y() = 0.0;
		frontDirection.normalize();
		double distance = max(((state.position - state.startingPosition) * frontDirection), 0.0);

		file_time << amarsi::Clock::getTime() << " " << distance << std::endl;

		// positions in robot frame
		Vector3 foreLeft_r = GetLegPosition(amarsi::LEFT_FORE);
		Vector3 hindLeft_r = GetLegPosition(amarsi::LEFT_HIND);
		Vector3 foreRight_r = GetLegPosition(amarsi::RIGHT_FORE);
		Vector3 hindRight_r = GetLegPosition(amarsi::RIGHT_HIND);

		// translate to world frame
		Vector3 foreLeft = state.orientation.conj().rotate(foreLeft_r);
		Vector3 hindLeft = state.orientation.conj().rotate(hindLeft_r);
		Vector3 foreRight = state.orientation.conj().rotate(foreRight_r);
		Vector3 hindRight = state.orientation.conj().rotate(hindRight_r);


		xForeLeft = foreLeft[0];
		xHindLeft = hindLeft[0];
		xForeRight = foreRight[0];
		xHindRight = hindRight[0];

		yForeLeft = foreLeft[1];
		yHindLeft = hindLeft[1];
		yForeRight = foreRight[1];
		yHindRight = hindRight[1];

		// in the world's frame:
		file_leg_trajectory << distance << " "
			<< xForeLeft << " " << yForeLeft << " "
			<< xHindLeft << " " << yHindLeft << " "
			<< xForeRight << " " << yForeRight << " "
			<< xHindRight << " " << yHindRight << std::endl;

		// in the robot's frame:
		file_leg_trajectory_rframe << distance << " "
			<< foreLeft_r[0] << " " << foreLeft_r[1] << " "
			<< hindLeft_r[0] << " " << hindLeft_r[1] << " "
			<< foreRight_r[0] << " " << foreRight_r[1] << " "
			<< hindRight_r[0] << " " << hindRight_r[1] << std::endl;

		fxForeLeft = GetGroundReactionForces(amarsi::LEFT_FORE)[0];
		fxHindLeft = GetGroundReactionForces(amarsi::LEFT_HIND)[0];
		fxForeRight = GetGroundReactionForces(amarsi::RIGHT_FORE)[0];
		fxHindRight = GetGroundReactionForces(amarsi::RIGHT_HIND)[0];

		fyForeLeft = GetGroundReactionForces(amarsi::LEFT_FORE)[1];
		fyHindLeft = GetGroundReactionForces(amarsi::LEFT_HIND)[1];
		fyForeRight = GetGroundReactionForces(amarsi::RIGHT_FORE)[1];
		fyHindRight = GetGroundReactionForces(amarsi::RIGHT_HIND)[1];

		file_forces 
			<< distance << " "
			<< fxForeLeft << " "
			<< fyForeLeft << " "
			<< fxHindLeft << " "
			<< fyHindLeft << " "
			<< fxForeRight << " "
			<< fyForeRight << " "
			<< fxHindRight << " "
			<< fyHindRight << std::endl;

		// POSITION, PITCH, AND ROLL 
		double rob_x = state.position.cx();
		double rob_y = state.position.cy();
		double rob_z = state.position.cz();
		double pitch = getPitchAngle();
		double roll  = getRollAngle();

		file_rob_trajectory 
			<< distance << " "
			<< rob_x << " "
			<< rob_y << " "
			<< rob_z << " "
			<< pitch << " "
			<< roll << std::endl;

		// Hip and knee position, compare set value to real value. Position
		// here means the ANGLE.
		// hips:
		double realHForeLeft;
		double realHForeRight;
		double realHHindLeft;
		double realHHindRight;

		realHForeLeft = actuator(amarsi::LEFT_FORE_HIP).position();
		realHHindLeft = actuator(amarsi::LEFT_HIND_HIP).position();
		realHForeRight = actuator(amarsi::RIGHT_FORE_HIP).position();
		realHHindRight = actuator(amarsi::RIGHT_HIND_HIP).position();

		// set values were already printed by generateTrotTrajectories
		file_hip_trajectory 
			<< realHForeLeft << " " 
			<< realHHindLeft << " " 
			<< realHForeRight << " " 
			<< realHHindRight << " "  
			<< distance << std::endl;

		// knees
		double realKForeLeft;
		double realKForeRight;
		double realKHindLeft;
		double realKHindRight;

		realKForeLeft = kneeEncoder(amarsi::LEFT_FORE).position();
		realKHindLeft = kneeEncoder(amarsi::LEFT_HIND).position();
		realKForeRight = kneeEncoder(amarsi::RIGHT_FORE).position();
		realKHindRight = kneeEncoder(amarsi::RIGHT_HIND).position();

		file_knee_trajectory
			<< realKForeLeft << " " 
			<< realKHindLeft << " " 
			<< realKForeRight << " " 
			<< realKHindRight << " " 
			<< distance << std::endl;


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
	file_stats << d_amplitudeForeHip << " "
		<< d_amplitudeHindHip << " "
		<< d_offsetForeHip << " "
		<< d_offsetHindHip << " "
		<< d_amplitudeForeKnee << " "
		<< d_amplitudeHindKnee << " "
		<< d_offsetKnee << " "
		<< d_kneePhaseLag << " "
		<< d_frequency << " ";

	double duration = amarsi::Clock::getTime();

	file_stats << duration << " ";

	// Compute the algebraic distance in front direction of
	// the robot

	Vector3 frontDirection = state.frontDirection;

	frontDirection.y() = 0.0;
	frontDirection.normalize();

	double distance = max(((state.position - state.startingPosition) * frontDirection), 0.0);
	file_stats << distance << " ";

	file_stats << max_angle << " ";

	stability /= duration; // normalize by time
	file_stats << stability << " ";

	// Note: You may want to add other values!!!

	// Close the line
	file_stats << std::endl;

	// Reset the position and dynamics of the robot for the
	// next run.
	// amarsi::Supervisor::instance().resetSimulation();
	//cout << image_file << endl;
	//}

	//cout << image_file << endl;
  
	//~ webots::Supervisor().exportImage(image_file, 100);
  
	// amarsi::Supervisor::instance().simulationQuit();
  
	file_time.close();
	file_stats.close();
	file_leg_trajectory.close();
	file_leg_trajectory_rframe.close();
	file_hip_trajectory.close();
	file_knee_trajectory.close();
	file_rob_trajectory.close();
	file_forces.close();

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


