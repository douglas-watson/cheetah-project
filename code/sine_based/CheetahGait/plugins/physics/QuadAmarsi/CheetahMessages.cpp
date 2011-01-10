/*
 * Copyright (c) 2009-2010 Alexandre Tuleu
 *
 * This file is part of libamarsi-quad.
 *
 * libamarsi-quad is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libamarsi-quad is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libamarsi-quad.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
/**
 * \file CheetahMessages.cpp
 *
 * \date 16 oct. 2009
 * \author Alexandre Tuleu
 */

#include "libamarsi-quad/common/CheetahMessages.h"

#include <iostream>

void initAllAmarsiMessages(){
	SetMotionRestrictor::registered=InternalMessage::
			register_type(static_cast<MessageType>(SET_MOTION_RESTRICTOR),
			              &SetMotionRestrictor::create);
	PhysicsFeedback::registered=InternalMessage::
			register_type(static_cast<MessageType>(PHYSICS_FEEDBACK),
			              &PhysicsFeedback::create);
	SetPhysicsSimulationParameter::registered=InternalMessage::
			register_type(static_cast<MessageType>(SET_PHYSICS_SIMULATION_PARAMETER),
			              &SetPhysicsSimulationParameter::create);
	KneeOrder::InternalMessage::
	register_type(static_cast<MessageType>(KNEE_ORDER),
	              &KneeOrder::create);
	Perturbation::InternalMessage::
	register_type(static_cast<MessageType>(PERTURBATION),
	              &Perturbation::create);
	ResetMessage::registered = InternalMessage::
			register_type(static_cast<MessageType>(RESET_SIMULATION),
			              &ResetMessage::create);
	DisplayHint::InternalMessage::
	register_type(static_cast<MessageType>(DISPLAY_HINT),
	              &DisplayHint::create);
}

bool SetMotionRestrictor::registered(false);

const int SetMotionRestrictor::FREE=0;
const int SetMotionRestrictor::SAGITAL_FIXED_PITCH=1;
const int SetMotionRestrictor::SAGITAL_FREE_PITCH=2;

//bool ContactState::registered=InternalMessage::
//    register_type(CONTACT_STATE,&ContactState::create);



bool PhysicsFeedback::registered(false);

bool SetPhysicsSimulationParameter::registered(false);

std::ostream & operator<<(std::ostream & out, const PhysicsFeedback &a){
	if(a.forces_.size()==4){
		out<<"Left fore : "<<a.forces_[0]<<", "
				<<"right fore : "<<a.forces_[1]<<", "
				<<"left hind : "<<a.forces_[2]<<", "
				<<"right hind : "<<a.forces_[3]
				                             <<"contact : ";
		if(a.d_hasABadContact)
			out<<"yes.";
		else
			out<<"no.";
	}else
		out<<"Empty forces information.";
	return out;
}

bool KneeOrder::registered(false);

bool ResetMessage::registered(false);
