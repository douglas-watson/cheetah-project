/**
 * \file BodyResetter.h
 *
 *  \date Oct 28, 2010
 *  \author tuleu
 */

#ifndef LIBAMARSI_QUAD_BODYRESETTER_H_
#define LIBAMARSI_QUAD_BODYRESETTER_H_

#include <list>

#include <ode/ode.h>

/**
 *
 */
class BodyResetter {
public:
	BodyResetter(dBodyID id);
	virtual ~BodyResetter();

	void push();
	bool pop();
	void restoreLast();

	private :
	class State{
	public :
		State(dBodyID id);
		~State();
		void setFromBody(dBodyID id);
		void applyToBody(dBodyID id);
	private :
		dReal d_x,d_y,d_z, d_xDot, d_yDot, d_zDot, d_wx,d_wy,d_wz;
		dReal d_q[4];
	};
	typedef std::list<State> QueueOfState;
	dBodyID d_body;
	QueueOfState d_storedStates;
};

#endif // LIBAMARSI_QUAD_BODYRESETTER_H_
