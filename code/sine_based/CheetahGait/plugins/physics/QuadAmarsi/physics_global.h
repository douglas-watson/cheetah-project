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
 * \file physics_global.h
 *
 *  \date May 19, 2010
 *  \author tuleu
 */

#ifndef LIBAMARSI_QUAD_PHYSICS_GLOBAL_H_
#define LIBAMARSI_QUAD_PHYSICS_GLOBAL_H_

#include "PhysicsPluginData.h"

#define EXIT_FROM_COLLIDE(exit_code)					\
  return exit_code;


extern amarsi::PhysicsPluginData *s_data;


//
//    /**
//    * \c true if the initialization was performed without error.
//    */
//    extern bool physics_enabled;//true
//    /**
//    * Display flags of 3D information. You should provide a combinaison of flags with
//    * | operator.
//    */
//    enum DisplayMode{
//    NONE=0x00,         //!< NONE no OpenGL Display
//    KNEE_FORCE=0x01,   //!< KNEE_FORCE Display the Knee Forces.
//    GROUND_CONTACT=0x02//!< GROUND_CONTACT Display the GRFs.
//    };
//
//    /**
//    * Defines a assignation map for the feedback. for each monitored contact,
//    * indicates to which index of the feedbackTable the value are.
//    */
//    typedef std::multimap<int,unsigned int> MapOfFeedBack;
//    /**
//    * The display mode used as a combination of DisplayMode flags.
//    */
//    extern DisplayMode m_;//((DisplayMode) (KNEE_FORCE|GROUND_CONTACT));
//    extern dWorldID world;//=NULL;
//    extern dSpaceID space;// = NULL;
//    extern dJointGroupID contact_joint_group;// = NULL;
//    /**
//    * The Deambulator used to restrict motion
//    */
//    extern amarsi::Deambulator* deamb;//=NULL;
//
//    /**
//    * List of bodies where contact with monotoredContact_ is denied.
//    */
//    extern  std::vector<dBodyID> deniedContactBody_;
//    /**
//    * List of bodies where contact is monitored for 3D Force sensors. It is
//    * essentially the foot.
//    */
//    extern  std::map<dGeomID,int> monitoredContacts_;
//    /**
//    * List of bodies where contact is ever allowed
//    */
//    extern  std::vector<dBodyID> allowedContactBody_;
//    /**
//    * List of bodies that are currently in contact in the simulation with the floor_.
//    */
//    extern  std::vector<dBodyID> currentContactBody_;
//    /**
//    * For each monitoredContact, point to a list of feedbackTable indexes
//    */
//    extern  MapOfFeedBack feedbacks_;
//    //std::vector<std::list<std::pair<dJointFeedback*,int> > > feedBack_;
//    /**
//    * Pointer to the floor.
//    */
//    extern dGeomID floor_;
//    #define MAX_CONTACTS 100
//    /**
//    * Number of feedback for this step.
//    */
//    extern  unsigned int feedback_num;
//    /**
//    * lists of GRF.
//    */
//    extern  dJointFeedback feedbackTable[MAX_CONTACTS];
//    /**
//    * lists of position of GRF.
//    */
//    extern  dVector3 feedbackPosTable[MAX_CONTACTS];
//    /**
//    * GRF with its position.
//    */
//    extern  std::vector<std::pair<Vector3,Vector3> > feedbackWithPosition_;
//
//    extern double toe_erp;
//
//    extern double toe_crf;
//
//    extern std::vector<dJointID> femursTibia_;
//    //extern std::vector<dBodyID> femurs_;
//    //extern std::vector<dBodyID> tibias_;
//    //extern std::vector<Vector3> positionsOnFemurs_;
//    //extern std::vector<Vector3> positionsOnTibia_;
//
//


#endif // libamarsi-quad_PHYSICS_GLOBAL_H_
