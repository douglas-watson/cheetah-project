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
 * \file MotionRestrictor.cpp
 *
 *  \date 16 nov. 2009
 *  \author Alexandre Tuleu
 */

#include "MotionRestrictor.h"
#include <cmath>
#include <iostream>

using namespace std;

dWorldID amarsi::MotionRestrictor::world = NULL;
dSpaceID amarsi::MotionRestrictor::space = NULL;
dJointGroupID amarsi::MotionRestrictor::contactJointGroup = NULL;

namespace amarsi {

  MotionRestrictor::MotionRestrictor(dWorldID w, dSpaceID s, dJointGroupID jid, dBodyID mainRobot)
    : mainRobot_(mainRobot){
    world = w;
    space = s;
    contactJointGroup = jid;
    s_ = NONE;
    jointsGroup_ = dJointGroupCreate(0);
    xAxis_ = new dReal[3];
    zAxis_ = new dReal[3];
    pos_ = new dReal[3];
  }

  void MotionRestrictor::cleanPrevious() {
    dJointGroupEmpty(jointsGroup_);
    for (ListOfBody::iterator b = (++bodies_.begin()); b != (--bodies_.end()); ++b) {
      dBodyDestroy(*b);
    }
    joints_.clear();
    bodies_.clear();
  }

  void MotionRestrictor::setNewRestriction(State s) {
    s_=s;
    cleanPrevious();
    setNewList(s);
    //setODEObject();
  }

  /*void Deambulator::setODEObject(){
   ListOfBody::iterator b=bodies_.begin();
   for(ListOfJoint::iterator j=joints_.begin();
   j!=joints_.end();
   ++j)
   dJointAttach(*j,*b,*(++b));
   }*/

  void MotionRestrictor::getPosition(dBodyID cheetah) {

    const dReal * p = dBodyGetPosition(cheetah);

    pos_[0] = p[0];
    pos_[1] = p[1];
    pos_[2] = p[2];
    dVector3 x;
    dBodyVectorFromWorld(cheetah, 1.0, 0.0, 0.0, x);

    xAxis_[0] = 0.0;
    xAxis_[1] = 0.0;
    xAxis_[2] = 0.0;
    zAxis_[0] = 0.0;
    zAxis_[1] = 0.0;
    zAxis_[2] = 0.0;

    dReal norm = std::pow(x[0], 2) + std::pow(x[2], 2);
    if (norm > 0.0000000001) {
      xAxis_[0] = x[0] / norm;
      xAxis_[2] = x[2] / norm;
    } else {
      if (x[0] > x[2]) {
        xAxis_[0] = 1.0;
      } else {
        xAxis_[2] = 1.0;
      }
    }

    zAxis_[0] = -xAxis_[2];
    zAxis_[2] = xAxis_[0];
  }

  void MotionRestrictor::setNewList(State s) {
    bodies_.push_back(mainRobot_);
    getPosition(bodies_.front());
    std::list<dBodyID>::const_iterator b = bodies_.begin();
    if (s == NONE) {

    } else {
      if (s == SAGITAL_FIXED_PITCH || s == SAGITAL_FREE_PITCH) {
        dJointID translateX = dJointCreateSlider(world, jointsGroup_);
        dBodyID dummy1 = addDummyBody();
        dJointAttach(translateX, 0, dummy1);
        dJointSetSliderAxis(translateX, (dReal) xAxis_[0], (dReal) xAxis_[1],
                            (dReal) xAxis_[2]);
        dJointID translateY = dJointCreateSlider(world, jointsGroup_);
        dBodyID dummy2 = (s == SAGITAL_FREE_PITCH) ? addDummyBody() : *b;
        dJointAttach(translateY, dummy1, dummy2);
        dJointSetSliderAxis(translateY, 0, 1, 0);
        joints_.push_back(translateX);
        joints_.push_back(translateY);
      }
      if (s == SAGITAL_FREE_PITCH) {
        dJointID rotateZ = dJointCreateHinge(world, jointsGroup_);
        dBodyID last = *(--bodies_.end());
        dJointAttach(rotateZ, last, *b);
        dJointSetHingeAnchor(rotateZ, pos_[0], pos_[1], pos_[2]);
        dJointSetHingeAxis(rotateZ, zAxis_[0], zAxis_[1], zAxis_[2]);
        dJointSetHingeParam(rotateZ, dParamLoStop, -1.5);
        dJointSetHingeParam(rotateZ, dParamHiStop, 1.5);
        joints_.push_back(rotateZ);
      }
    }
    bodies_.push_back(0);
  }

  MotionRestrictor::~MotionRestrictor() {
    cleanPrevious();
    dJointGroupDestroy(jointsGroup_);
    delete[] xAxis_;
    delete[] zAxis_;
    delete[] pos_;

  }

  dBodyID MotionRestrictor::addDummyBody() {
    dBodyID b = dBodyCreate(world);
    dBodySetGravityMode(b, 0);
    bodies_.push_back(b);
    return b;
  }

}
