/**
 * \file BasicController.h
 *
 *  \date May 14, 2010
 *  \author tuleu
 */

#ifndef LIBAMARSIQUAD_BASICCONTROLLER_H_
#define LIBAMARSIQUAD_BASICCONTROLLER_H_

#include <libamarsi-quad/libamarsi-quad.h>

class BasicController : public amarsi::Controller{
public:
  BasicController(int &argc, char** argv);
  virtual ~BasicController();

  virtual int run();

  virtual void userInitSimulation();
  virtual void userInitEnvironment();
private :
  //amplitude of oscillation for the Hip joints.
  double d_amplitudeHip;
  //offset of the oscillations for the Hip Joint.
  double d_offsetHip;
  //amplitude of the extension/contraction of the leg
  double d_amplitudeKnee;
  //offset of the knee command
  double d_offsetKnee;
  //frequency of the oscillations
  double d_frequency;
  // angle of the slope
  double d_angle;
};

#endif // LIBAMARSIQUAD_BASICCONTROLLER_H_
