/**
 * \file webots_physics_init.cpp
 *
 *  \date May 19, 2010
 *  \author tuleu
 */

#include <iostream>
#include <sstream>
#include <ctime>
#define _ODE_ODECPP_H_
#include <plugins/physics.h>
#ifndef FOR_PHYSICS_PLUGIN
#define FOR_PHYSICS_PLUGIN
#endif //FOR_PHYSICS_PLUGIN

#include <libamarsi-quad/common/CheetahMessages.h>

#include "physics_global.h"

amarsi::PhysicsPluginData *s_data(NULL);
//  bool physics_enabled(true);
//  DisplayMode m_((DisplayMode) (NONE));
//  dWorldID world=NULL;
//  dSpaceID space= NULL;
//  dJointGroupID contact_joint_group = NULL;
//  amarsi::MotionRestrictor* deamb = NULL;
//  std::vector<dBodyID> deniedContactBody_;
//  std::map<dGeomID,int> monitoredContacts_;
//  std::vector<dBodyID> allowedContactBody_;
//  std::vector<dBodyID> currentContactBody_;
//  MapOfFeedBack feedbacks_;
//  dGeomID floor_;
//  unsigned int feedback_num;
//  dJointFeedback feedbackTable[MAX_CONTACTS];
//  dVector3 feedbackPosTable[MAX_CONTACTS];
//  std::vector<std::pair<Vector3,Vector3> > feedbackWithPosition_;
//  std::ofstream *file_;//("/home/tuleu/amarsiLink/data/Joint.txt");
//  double toe_erp;
//  double toe_crf;
//  std::vector<dJointID> femursTibia_;
//std::vector<dBodyID> tibias_;
//std::vector<Vector3> positionsOnFemurs_;
//std::vector<Vector3> positionsOnTibia_;
/**
 * Returns a dBody id from its DEF name.
 * @param def DEF name of teh desired dBody
 * @return a pointer to the dBody if found NULL otherwise
 */
dBodyID getBodyFromDEF(const char *def) {
	//std::cout<<"Try to get body "<<def<<std::endl;
	dGeomID geom = dWebotsGetGeomFromDEF(def);
	if (! geom) {
		printf("Fatal error: did not find dGeom for DEF %s", def);
		s_data->disablePhysics();
		return NULL;
	}

	dBodyID body;
	if (dGeomIsSpace(geom)){
		body = dGeomGetBody(dSpaceGetGeom((dSpaceID)geom, 0));
		std::cerr<<"I don't know why I have to do this"<<std::endl;
		s_data->disablePhysics();
	}else
		body = dGeomGetBody(geom);

	if (! body) {
		printf("Fatal error: did not find dBody for DEF %s", def);
		s_data->disablePhysics();
		return NULL;
	}

	return body;
}


void insertGeomInMonitored(dGeomID geom, amarsi::Limbs limb){
	if(!dGeomIsSpace(geom)){
		s_data->addMonitoredBody(geom,limb);
		//std::cerr<<" Directly found geom of type "<<dGeomGetClass(toe)<<std::endl;
		return;
	}
	//std::cerr<<"IS a space, explore childrens"<<std::endl;
	dSpaceID geomSpace=(dSpaceID)(geom);
	for (int ii=0;ii<dSpaceGetNumGeoms(geomSpace);++ii){
		dGeomID g=dSpaceGetGeom(geomSpace,ii);
		if(g){
			//std::cerr<<"store a geom of type "<<dGeomGetClassData(g)<<std::endl;
			s_data->addMonitoredBody(geom,limb);
		}
	}

}

void doStuff(const std::string & prefix,amarsi::Limbs limb){

	std::ostringstream name;
	name<<prefix<<"_TOE";
	dBodyID toeBody=dWebotsGetBodyFromDEF(name.str().c_str());
	dGeomID toe=dWebotsGetGeomFromDEF(name.str().c_str());
	//std::cerr<<"Get Geom of value "<<toe<<std::endl;
	if(!toe || !toeBody){
		std::cerr<<"Did not found "<<name<<" "<<toe<<" "<<toeBody<<std::endl;
		s_data->disablePhysics();
		return;
	}


	std::ostringstream nameTibia;
	nameTibia<<prefix<<"_FRONT_KNEE";
	dBodyID tibia = dWebotsGetBodyFromDEF(nameTibia.str().c_str());
	if(!tibia){
		std::cerr<<"Did not found "<<nameTibia<<std::endl;
		s_data->disablePhysics();
		return;
	}
	//  std::cerr<<nameTibia<<" "<<tibia<<std::endl;
	int numJoint=dBodyGetNumJoints(tibia);

	std::ostringstream nameFemur;
	nameFemur<<prefix<<"_HIP_SERVO";
	dBodyID femur = dWebotsGetBodyFromDEF(nameFemur.str().c_str());
	if(!femur){
		std::cerr<<"Did not found "<<nameFemur<<std::endl;
		s_data->disablePhysics();
		return;
	}
	for(int i=0;i<numJoint;++i){
		dJointID j=dBodyGetJoint(tibia,i);

		dBodyID b1= dJointGetBody(j,0);
		dBodyID b2= dJointGetBody(j,1);

		dReal pos[4];
		if(b1==femur || b2==femur){
			if(b1==femur)
				dJointGetHingeAnchor2(j,pos);
			else
				dJointGetHingeAnchor(j,pos);
			dVector3 posRel;
			dBodyGetPosRelPoint(tibia,pos[0],pos[1],pos[3],posRel);
			//     std::cerr<<"pos : "<<posRel[0]<<" "<<posRel[1]<<" "<<posRel[2]<<std::endl;
			//     s_data->addKneeJoint(j,limb);
		}

	}

	std::ostringstream nameAnkle;
	nameAnkle<<prefix<<"_ANKLE"<<std::flush;
	dBodyID ankleBody = dWebotsGetBodyFromDEF(nameAnkle.str().c_str());
	dGeomID ankle=dWebotsGetGeomFromDEF(nameAnkle.str().c_str());
	if(!ankle || !ankleBody){
		std::cerr<<"Did not found "<<nameAnkle<<" "<<ankle<<" "
		         <<ankleBody<<std::endl;
		s_data->disablePhysics();
		return;
	}
	insertGeomInMonitored(ankle,limb);
	insertGeomInMonitored(toe,limb);

	s_data->addResettableBody(femur);
	s_data->addResettableBody(tibia);
	s_data->addResettableBody(ankleBody);
	s_data->addResettableBody(toeBody);
}

/**
 * Initialize the whole plugins. Gets the monitoredContact, closes the mechanincal
 * @param world_
 * @param space_
 * @param contactJointGroup_
 */
void webots_physics_init(dWorldID world_, dSpaceID space_, dJointGroupID contactJointGroup_) {
#ifdef PHYSICS_DEBUG_OUT
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	DEBUG_OUT("Start Initialization at "<<asctime (timeinfo))
#endif

		s_data= new amarsi::PhysicsPluginData();
	s_data->world=world_;
	s_data->space=space_;
	s_data->contactJointGroup=contactJointGroup_;

	s_data->setMotionRestrictor(dWebotsGetBodyFromDEF("AMARSI"));

	doStuff("LEFT_FORE",amarsi::LEFT_FORE);
	doStuff("RIGHT_FORE",amarsi::RIGHT_FORE);
	doStuff("LEFT_HIND",amarsi::LEFT_HIND);
	doStuff("RIGHT_HIND",amarsi::RIGHT_HIND);
	s_data->addResettableBody(dWebotsGetBodyFromDEF("AMARSI"));
	s_data->addResettableBody(dWebotsGetBodyFromDEF("MOTORS_GROUP"));
	s_data->addResettableBody(dWebotsGetBodyFromDEF("FR4_BODY_PARTS"));

	s_data->saveState();

	s_data->addDeniedContactBody(getBodyFromDEF("MOTORS_GROUP"));

	s_data->addFloorElement("GROUND");
	s_data->addFloorElement("RAMP");
	s_data->addFloorElement("PODIUM");
	s_data->addFloorElement("SLOPE");
	s_data->addFloorElement("HEIGHTMAP");

	s_data->setMainBody("MOTORS_GROUP");

	initAllAmarsiMessages();
	Message::set_emitter_receiver(EmitterReceiverPhysics::create());
	Message::receive_enable(2);

	DEBUG_OUT("Finish Initialization.")
		//  toe_erp=0.04;
		//  toe_crf=0.001;
		}


