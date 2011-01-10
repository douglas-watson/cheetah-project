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
 * \file MotionRestrictor.h
 *
 * \date 16 nov. 2009
 * \author Alexandre Tuleu
 */

#ifndef DEAMBULATOR_H_
#define DEAMBULATOR_H_

#define _ODE_ODECPP_H_
#include <list>
#include <ode/ode.h>
#include <plugins/physics.h>


namespace amarsi {
  /**
   * A motion restrictor that forbid movement outside the sagital plane.
   *
   * This restriction is made by adding ODE bodies to the cheetah, without any
   * mass. This class handle the construction/destruction of these bodies, and
   * let the user enable/disable the restriction with setNewDeambulator()
   * 
   */

  class MotionRestrictor {
  public :
    /**
     * A list of ODE bodies
     */
    typedef std::list<dJointID> ListOfJoint;

    /**
     * A List of ODE joints.
     */
    typedef std::list<dBodyID> ListOfBody;
    /**
     * The available restricted state
     */
    enum State{NONE,              //!< NONE the robot is free
               SAGITAL_FIXED_PITCH, //!< SAGITAL_FIXED_PITCH the robot can only have motion in the sagital plane.
               SAGITAL_FREE_PITCH}; //!< SAGITAL_FREE_PITCH the robot can only have motion in the sagital plane, and the pitch rotation is fixed.

    static dWorldID world;
    static dSpaceID space;
    static dJointGroupID contactJointGroup;

  protected :
    /**
     * Bodies created for a restriction.
     */
    ListOfBody bodies_;
    /**
     * Joints created for a restriction.
     */
    ListOfJoint joints_;
    /**
     * Group of the joints created for a restriction.
     */
    dJointGroupID jointsGroup_;

    /**
     * State of the Deambulator.
     */
    State s_;
    ///@name Position of the Cheetah.
    //@{
    dReal *xAxis_;
    dReal *zAxis_;
    dReal *pos_;
    //@}
    /**
     * Main body of the robot
     */
    dBodyID mainRobot_;
    /**
     * Remove all rpevious restriction.
     */
    void cleanPrevious();
    /**
     * Creates the new list of dummy bodies/joints for the given State.
     * @param s the new State
     */
    void setNewList(State s);
    /**
     * Gets the position of the Cheetah.
     * @param cheetah ID of the main Cheetah body.
     */
    void getPosition(dBodyID cheetah);
    /**
     * Add a dummy body to the world, without any mass.
     * @return pointer to the body.
     */
    dBodyID addDummyBody();

  public:
    /**
     * Creates a new Deambulator
     * @param w pointer to the dWorld
     * @param s pointer to the dSpace of geom
     * @param jid pointer to the dJointGroup of contacts.
     * @param mainRobot dBodyID of the robot.
     */
    MotionRestrictor(dWorldID w, dSpaceID s, dJointGroupID jid, dBodyID mainRobot);
    virtual ~MotionRestrictor();
    /**
     * Sets a new State for the Deambulator
     * @param s the new State.
     */
    void setNewRestriction(State s);

    /**
     * Returns current State.
     * @return
     */
    State getState()const{
      return s_;
    }
  };

}

#endif /* DEAMBULATOR_H_ */
