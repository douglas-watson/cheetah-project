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
/*
 * File:
 * Date:
 * Description:
 * Author:
 * Modifications:
 */

#include <iostream>
#include <fstream>

#ifndef FOR_PHYSICS_PLUGIN
  #define FOR_PHYSICS_PLUGIN
#endif //FOR_PHYSICS_PLUGIN
#include <libamarsi-quad/common/CheetahMessages.h>


#include "physics_global.h"
#include "ode-mymath.h"



/**
 * Prints name of dGeomClass and there IDs
 */
// void odeClassReminder(){
//   std::cout<<"dSphereClass : "<<dSphereClass<<std::endl
//            <<"dBoxClass : "<<dBoxClass<<std::endl
//            <<"dCCylinderClass : "<<dCCylinderClass<<std::endl
//            <<"dCylinderClass : "<<dCylinderClass<<std::endl
//            <<"dPlaneClass : "<<dPlaneClass<<std::endl
//            <<"dRayClass : "<<dRayClass<<std::endl
//            <<"dGeomTransformClass : "<<dGeomTransformClass<<std::endl
//            <<"dTriMeshClass : "<<dTriMeshClass<<std::endl
//            <<"dFirstSpaceClass : "<<dFirstSpaceClass<<std::endl
//            <<"dSimpleSpaceClass : "<<dSimpleSpaceClass<<std::endl
//            <<"dHashSpaceClass : "<<dHashSpaceClass<<std::endl
//            <<"dQuadTreeSpaceClass : "<<dQuadTreeSpaceClass<<std::endl
//            <<"dLastSpaceClass : "<<dLastSpaceClass<<std::endl
//            <<"dFirstUserClass : "<<dFirstUserClass<<std::endl
//            <<"dLastUserClass : "<<dLastUserClass<<std::endl
//            <<"dGeomNumClasses : "<<dGeomNumClasses<<std::endl;
// }

/**
 * Sets a new deambulator
 * @param type
 */
void setNewDeambulator(int type){
  if(type==SetMotionRestrictor::FREE)
    s_data->motionRestrictor().setNewRestriction(amarsi::MotionRestrictor::NONE);
  else if(type==SetMotionRestrictor::SAGITAL_FIXED_PITCH){
    s_data->motionRestrictor().setNewRestriction(amarsi::MotionRestrictor::SAGITAL_FIXED_PITCH);
  }else if(type==SetMotionRestrictor::SAGITAL_FREE_PITCH)
    s_data->motionRestrictor().setNewRestriction(amarsi::MotionRestrictor::SAGITAL_FREE_PITCH);
}
/**
 * Handles a new incoming message
 * @param m
 */
void handleNewMessage(Message &m){
  DEBUG_OUT("    Handle message "<<m.type()<<".")
  switch (m.type()) {
    case SET_MOTION_RESTRICTOR:{
      SetMotionRestrictor mm = SetMotionRestrictor::cast(m);
      setNewDeambulator(mm.getType());
      break;
    }
    case SET_PHYSICS_SIMULATION_PARAMETER:{
      SetPhysicsSimulationParameter mm = SetPhysicsSimulationParameter::cast(m);
      s_data->setPhysicsParameter(mm);
      break;
    }
    case KNEE_ORDER:{
      KneeOrder mm = KneeOrder::cast(m);
      //      std::cerr<<"get order for limb "<<mm.limb()<<" : "<< mm.torque() <<std::endl;
//      dJointAddHingeTorque(s_data->tibiaJoint(mm.limb()),-mm.torque());
      break;
    }
    case PERTURBATION:{
      Perturbation mm = Perturbation::cast(m);
      std::cerr<<"received a perturbation "<<mm.force()<<" "
               <<mm.numberOfSimulationStep()<<std::endl;
      s_data->perturbate(mm.force(), mm.numberOfSimulationStep());
      break;
    }
    case DISPLAY_HINT:{
      DisplayHint mm = DisplayHint::cast(m);
      s_data->setDisplayMode(mm.mode());
      break;
    }
    case RESET_SIMULATION:{
    	s_data->resetSimulationAtEndOfStep();
    }
    default:
      break;
  }
}


/**
 * Gets the message of the Controller, and applyKneeForces()
 */

void webots_physics_step() {
  DEBUG_OUT("Start iteration.")
  while(Message::waiting_messages()){
    DEBUG_OUT("  Start reading message.")
    Message m(Message::receive_message());
    if(m.is_valid() && m.am_I_receiver(ID_PHYSICS)){
      handleNewMessage(m);
    }
    DEBUG_OUT("  Message read.")
  }
  s_data->applyPerturbation();
  DEBUG_OUT("Finish Iteration.")
 }



void webots_physics_cleanup() {
  delete s_data;
  /*
   * Here you need to free any memory you allocated in above, close files, etc.
   * You do not need to free any ODE object, they will be freed by Webots.
   */
}

