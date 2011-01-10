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
 * \file webots_physics_collide.cpp
 *
 *  \date May 19, 2010
 *  \author tuleu
 */
#include <algorithm>
#ifndef FOR_PHYSICS_PLUGIN
  #define FOR_PHYSICS_PLUGIN
#endif //FOR_PHYSICS_PLUGIN
#include <libamarsi-quad/common/CheetahMessages.h>
#include "CheckGroundContact.h"
#include "physics_global.h"

//small bug that is present on version 6.1.5 of webots
extern "C"{
  void webots_physics_step_end();
}



/**
 * Intercept the collision of any contact with the floor_. Checks if it is a
 * monitoredContact_ and enable the feedback structures if it is the case.
 * @param g1 the first geom
 * @param g2 the second geom
 */

#undef DEBUG_LINE
#define DEBUG_LINE

int webots_physics_collide(dGeomID g1, dGeomID g2) {
  DEBUG_OUT("Start Collision handling.")
  if(dGeomIsSpace(g1)||dGeomIsSpace(g2)){
    EXIT_FROM_COLLIDE(0)
  }
  DEBUG_LINE
  dGeomID floorElement(g1), other(g2);
  if(!s_data->getFloorElement(floorElement,other)){
    EXIT_FROM_COLLIDE(0)
  }
  DEBUG_LINE
  //now we intercept the contact.
  amarsi::Limbs limbOftheContact;
  DEBUG_LINE
  bool monitored=s_data->isMonitored(other,limbOftheContact);
  DEBUG_LINE
  int maxContact= s_data->getMaxContact();
  DEBUG_LINE
  dContact *contacts= s_data->getContactStructure();
  DEBUG_LINE

  int i;

  DEBUG_LINE

  int n = dCollide(g1, g2, maxContact, &(contacts[0].geom), sizeof(dContact));
  s_data->addNewContacts(n);
  DEBUG_LINE
  for (i = 0; i < n; ++i) {//for each contact create the dContact structure.
    dContact &c=contacts[i];
    if(monitored)//it's a toe
      c.surface.mode = dContactApprox1 | dContactSoftCFM | dContactSoftERP | dContactBounce;
    else
      c.surface.mode= dContactApprox1;
    c.surface.mu = s_data->toeFriction(); //2.0;
    c.surface.mu2 = s_data->toeFriction(); //2.0;
    c.surface.soft_erp = s_data->toeERP(); //0.1
    c.surface.soft_cfm = s_data->toeCFM(); //0.2
    c.surface.bounce = 0.0;
    c.surface.bounce_vel = 0.0;
    c.fdir1[0]=1.0;
    c.fdir1[1]=0.0;
    c.fdir1[2]=0.0;
    c.fdir1[3]=0.0;
    //c.geom = contacts[i];

    //create the contact and attach it
    dJointID jid = dJointCreateContact(s_data->world,
                                       s_data->contactJointGroup,
                                       &c);

    dJointAttach(jid, dGeomGetBody(g1), dGeomGetBody(g2));

    if(monitored){
      s_data->addFeedback(jid,limbOftheContact,c.geom.pos);
    }

  }
  DEBUG_LINE
  if(n>0){//we store the body in contact with the ground.
    DEBUG_LINE
    s_data->addNewBodyContact(dGeomGetBody(other));//currentContactBody_.push_back(g1==floor_?dGeomGetBody(g2):dGeomGetBody(g1));
  }
  DEBUG_OUT("Finish collision handling")
  EXIT_FROM_COLLIDE(1)
}





/**
 * Gets the feedback information back, and sends values to the Controller back.
 */

void webots_physics_step_end(){
	if(s_data->resetAsked()){
		s_data->resetSimulation();
		PhysicsFeedback m(ID_PHYSICS,ID_ROBOT);
		m.setForce(Vector3::ZERO,amarsi::LEFT_FORE);
		m.setForce(Vector3::ZERO,amarsi::LEFT_HIND);
		m.setForce(Vector3::ZERO,amarsi::RIGHT_FORE);
		m.setForce(Vector3::ZERO,amarsi::RIGHT_HIND);
		m.setHasABadContact(false);
		m.send_me();
		s_data->resetFeedbackData();
	}


  DEBUG_OUT("Start Step end.")
  /* Read feedback information from 'feedback' */
  s_data->processFeedbackWithPosition();
  PhysicsFeedback m(ID_PHYSICS,ID_ROBOT);

  m.setForce(s_data->grf(amarsi::LEFT_FORE),amarsi::LEFT_FORE);
  m.setForce(s_data->grf(amarsi::LEFT_HIND),amarsi::LEFT_HIND);
  m.setForce(s_data->grf(amarsi::RIGHT_HIND),amarsi::RIGHT_HIND);
  m.setForce(s_data->grf(amarsi::RIGHT_FORE),amarsi::RIGHT_FORE);

  amarsi::CheckGroundContact test;
  std::for_each(s_data->deniedContactBodies().begin(),
                s_data->deniedContactBodies().end(),
                test);
#ifdef PHYSICS_DEBUG_OUT
  std::cerr<<"There is "<<s_data->deniedContactBodies().size()<<" bodies forbidden contact."<<std::endl;
  for(amarsi::SetOfBody::const_iterator body= s_data->deniedContactBodies().begin();
      body != s_data->deniedContactBodies().end();
      ++body){
    std::cerr<<"  "<<"body "<<*body<<std::endl;
  }
  s_data->printAllBodyInContactOnStderr();
#endif
  m.setHasABadContact(!test.isValid());
#ifdef PHYSICS_DEBUG_OUT
  if(m.hasABadContact()){
    std::cerr<<("Got a bad contact.")<<std::endl;
  }
#endif
  m.send_me();

  s_data->resetFeedbackData();
  DEBUG_OUT("Finish Step end.")

  //  currentContactBody_.clear();
  //
  //  feedbacks_.clear();
  //  feedback_num=0;
}
