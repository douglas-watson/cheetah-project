/**
 * \file BodyResetter.cpp
 *
 *  \date Oct 28, 2010
 *  \author tuleu
 */

#include "BodyResetter.h"

BodyResetter::BodyResetter(dBodyID id)
 : d_body(id){
}

BodyResetter::~BodyResetter(){
}


void BodyResetter::push(){
	d_storedStates.push_back(State(d_body));
}
bool BodyResetter::pop(){
	if(d_storedStates.empty())
		return false;
	d_storedStates.back().applyToBody(d_body);
	d_storedStates.pop_back();
	return true;
}

void BodyResetter::restoreLast(){
	if(d_storedStates.empty())
		return;
	d_storedStates.back().applyToBody(d_body);
}

BodyResetter::State::State(dBodyID id){
	setFromBody(id);
}
BodyResetter::State::~State(){}
void BodyResetter::State::setFromBody(dBodyID id){
	const dReal * ptr;
	ptr =dBodyGetPosition(id);
	d_x=ptr[0];
	d_y=ptr[1];
	d_z=ptr[2];

	ptr = dBodyGetQuaternion(id);
	d_q[0]=ptr[0];
	d_q[1]=ptr[1];
	d_q[2]=ptr[2];
	d_q[3]=ptr[3];

	ptr =dBodyGetLinearVel(id);
	d_xDot = ptr[0];
	d_yDot = ptr[1];
	d_zDot = ptr[2];

	ptr =dBodyGetAngularVel(id);
	d_wx = ptr[0];
	d_wy = ptr[1];
	d_wz = ptr[2];

}
void BodyResetter::State::applyToBody(dBodyID id){
	dBodySetPosition(id,d_x,d_y,d_z);
	dBodySetLinearVel(id,d_xDot,d_yDot,d_zDot);
	dBodySetAngularVel(id,d_wx,d_wy,d_wz);
	dBodySetQuaternion(id,d_q);

}
