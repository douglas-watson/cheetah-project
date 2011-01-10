/**
 SystematicSearchController.h
 *
 *  \date October 21, 2010
 *  \author tuleu
 */

#ifndef LIBAMARSIQUAD_SYSTEMATICSEARCHCONTROLLER_H_
#define LIBAMARSIQUAD_SYSTEMATICSEARCHCONTROLLER_H_

#include <libamarsi-quad/libamarsi-quad.h>

class SystematicSearchController : public amarsi::Controller
{
	public:
		SystematicSearchController(int &argc, char** argv);
		virtual ~SystematicSearchController();

		int run();

		// Control the robot for one run with given parameter
		void controlRobotForOneRun();

		// Send all command tio actuator to generate the trot trajectory,
		// according to the current simulation time.
		void generateTrotTrajectories();

		virtual void userInitSimulation();
		virtual void userInitEnvironment();

	private:
		double stepFunction(double amplitude, double phase, double offset);
		double sineFunction(double amplitude, double phase, double offset);

		// Amplitude of oscillation for the fore hip joints.
		double d_amplitudeForeHip;

		// Amplitude of oscillation for the hind hip joints.
		double d_amplitudeHindHip;

		// Offset of the oscillations for the fore hip Joint.
		double d_offsetForeHip;

		// Offset of the oscillations for the hind hip Joint.
		double d_offsetHindHip;

		// Amplitude of the extension/flexion of the fore legs.
		double d_amplitudeForeKnee;

		// Amplitude of the extension/flexion of the hind legs.
		double d_amplitudeHindKnee;

		// Offset of the knee command.
		double d_offsetKnee;

		// Phase lag between knee and hip command.
		double d_kneePhaseLag;

		// Frequency of the oscillations.
		double d_frequency;

		// Duration of one run of teh robot with selected parameter
		double d_duration;
};

#endif // LIBAMARSIQUAD_SYSTEMATICSEARCHCONTROLLER_H_
