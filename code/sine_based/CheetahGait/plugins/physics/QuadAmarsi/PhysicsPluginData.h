/**
 * \file PhysicsPluginData.h
 *
 *  \date Jun 28, 2010
 *  \author tuleu
 */

#ifndef LIBAMARSI_QUAD_PHYSICSPLUGINDATA_H_
#define LIBAMARSI_QUAD_PHYSICSPLUGINDATA_H_

#include <map>
#include <vector>
#include <set>
#include <string>


#define  _ODE_ODECPP_H_
#include <ode/ode.h>
#include <plugins/physics.h>

#include <libamarsi-quad/common/global.h>
#include <libamarsi-quad/basic-math/math.hpp>
#include "MotionRestrictor.h"
#include <libamarsi-quad/common/CheetahMessages.h>
#include "BodyResetter.h"

#ifdef PHYSICS_DEBUG_OUT
#define DEBUG_OUT(text) std::cerr<<"[DEBUG] : "<<text<<std::endl;
#define DEBUG_LINE DEBUG_OUT("Pass here "<<__FILE__<<":"<<__LINE__)
#else
#define DEBUG_OUT(text)
#define DEBUG_LINE
#endif

#define MAX_CONTACTS 100


namespace amarsi {

	typedef std::set<dBodyID> SetOfBody;
	typedef std::set<dGeomID> SetOfGeom;


	class MotionRestrictor;



	class PhysicsPluginData {
	public:

		PhysicsPluginData();
		virtual ~PhysicsPluginData();

		/**
		 * Sets the motion restrictor
		 * @param mainBody
		 */
		void setMotionRestrictor(dBodyID mainBody);

		/**
		 * Add a body to the set of body that should not touch the ground
		 *
		 * @param body
		 */
		void addDeniedContactBody(dBodyID body);

		/**
		 * Add a body to the list of floor element;
		 * @param floorElementName
		 */
		void addFloorElement(const std::string & floorElementName);

		/**
		 * Set the ERP and CFM
		 * @param message
		 */
		void setPhysicsParameter(const SetPhysicsSimulationParameter & message);

		/**
		 * Test if either one of the two body is a floor element. swap the pointer
		 * to match name if needed
		 * @param floorElement
		 * @param other
		 * @return false if neither of the two body is a floor element.
		 */
		bool getFloorElement(dGeomID & floorElement, dGeomID & other);


		void addMonitoredBody(dGeomID geom, amarsi::Limbs limb);
		bool isMonitored(dGeomID geom, amarsi::Limbs & limbOfTheContact);

		//    dJointID tibiaJoint(amarsi::Limbs l) const;

		MotionRestrictor & motionRestrictor() const;

		double toeERP() const;
		double toeCFM() const;
		double toeFriction() const;

		void addFeedback(dJointID joint, amarsi::Limbs l,const dVector3 & pos);


		void addNewBodyContact(dBodyID body);

		int getMaxContact() const;

		dContact * getContactStructure();
		void addNewContacts(unsigned int nbContact);

		void processFeedbackWithPosition();

		const Vector3 & grf(amarsi::Limbs l) const;
		const Vector3 & grfPos(amarsi::Limbs l) const;
		const SetOfBody & deniedContactBodies() const;


		void resetFeedbackData();

		void disablePhysics();

		bool good() const;
		DisplayMode mode() const;


		//    void addKneeJoint(dJointID jid, amarsi::Limbs limb);

		dWorldID world;
		dSpaceID space;
		dJointGroupID contactJointGroup;


		bool isAFloorElement(dGeomID body) const;


		bool isInContact(dBodyID);
		void printAllBodyInContactOnStderr() const;

		void setMainBody(const std::string & name);

		void perturbate(const Vector3 & force, unsigned int nbStep );

		void applyPerturbation();//to call each timestep;

		const Vector3 & currentPerturbationForce() const;
		Vector3 currentPertubationApplyPoint() const;

		void setDisplayMode(amarsi::DisplayMode mode);

		void resetSimulationAtEndOfStep();
		bool resetAsked();
		void resetSimulation();

		void addResettableBody(dBodyID id);
		void saveState();
	private :
		typedef std::multimap<amarsi::Limbs,unsigned int> MapOfFeedback;
		typedef std::map<dGeomID,amarsi::Limbs> GeomAssignation;
		typedef std::vector<BodyResetter> SetOfResetter;
		bool d_physicsEnabled;
		DisplayMode d_m;
		MotionRestrictor * d_motionRestrictor;

		//to add perturbation
		dBodyID d_mainBody;
		Vector3 d_targetForce;
		Vector3 d_actualForce;
		unsigned int d_nbStep;

		//for handling the floor contact interception
		SetOfGeom d_floorElements;
		SetOfBody d_deniedContactBodies;
		SetOfBody d_allowedContactBodies;
		SetOfBody d_currentContactBodies;
		GeomAssignation d_monitoredContacts;
		MapOfFeedback d_feedbacks;
		int d_feedbackNum;

		dContact d_handledContactJoint[MAX_CONTACTS];
		unsigned int d_numberOfHandledContact;
		dJointFeedback d_feedbackTable[MAX_CONTACTS];
		dVector3 d_feedbackPosTable[MAX_CONTACTS];

		std::vector<std::pair<Vector3,Vector3> > d_feedbackWithPosition;


		//manual forces
		//std::vector<dJointID> d_femurTibia;


		//toe contact
		double d_toeERP;
		double d_toeCFM;
		double d_toeFriction;

		//to quick change of floor
		unsigned int numberOfFloorFirst;
		unsigned int numberOfFloorSecond;

		bool d_resetAsked;

		SetOfResetter d_bodyToReset;
	};

	inline void PhysicsPluginData::setMotionRestrictor(dBodyID mainBody){
		d_motionRestrictor = new MotionRestrictor(world,space,
		                                          contactJointGroup,mainBody);
	}

	inline void PhysicsPluginData::addDeniedContactBody(dBodyID body){
		d_deniedContactBodies.insert(body);
	}

	inline void PhysicsPluginData::addFloorElement(const std::string & floorElementName){
		dGeomID ele = dWebotsGetGeomFromDEF(floorElementName.c_str());
		if(!ele){
			std::cerr<<"Didn't not found requisited body "<<floorElementName
					<<" , so I quit."<<std::endl;
			exit(1);
		}
		d_floorElements.insert(ele);
	}

	inline void PhysicsPluginData::
	setPhysicsParameter(const SetPhysicsSimulationParameter & message){
		d_toeCFM = message.cfm();
		d_toeERP = message.erp();
		d_toeFriction = message.friction();
	}

	inline void PhysicsPluginData::addMonitoredBody(dGeomID geom, amarsi::Limbs limb){
		d_monitoredContacts.insert(std::make_pair(geom,limb));
	}

	//  inline dJointID PhysicsPluginData::tibiaJoint(amarsi::Limbs l) const{
	//    if(l==amarsi::NONE || l==amarsi::NUMBER_OF_LIMBS)
	//      return 0;/// \todo should throw exception
	//    return d_femurTibia[l];
	//  }

	inline MotionRestrictor & PhysicsPluginData::motionRestrictor() const{
		return *d_motionRestrictor;
	}

	inline double PhysicsPluginData::toeERP() const {
		return d_toeERP;
	}

	inline double PhysicsPluginData::toeCFM() const {
		return d_toeCFM;
	}

	inline double PhysicsPluginData::toeFriction() const {
		return d_toeFriction;
	}

	inline int PhysicsPluginData::getMaxContact() const{
		int availableSlots = MAX_CONTACTS-d_numberOfHandledContact;
		return availableSlots <4 ? availableSlots : 4;
	}

	inline bool PhysicsPluginData::isMonitored(dGeomID geom,
	                                           amarsi::Limbs & limbOfTheContact){
		GeomAssignation::const_iterator fi=d_monitoredContacts.find(geom);
		if(fi!=d_monitoredContacts.end()){
			limbOfTheContact=fi->second;
			return true;
		}
		limbOfTheContact=amarsi::NONE;
		return false;
	}

	inline void PhysicsPluginData::addNewBodyContact(dBodyID body){
		d_currentContactBodies.insert(body);
	}

	inline const Vector3 & PhysicsPluginData::grf(amarsi::Limbs l) const{
		if(l==amarsi::NONE || l==amarsi::NUMBER_OF_LIMBS)
			return Vector3::ZERO;/// \todo should throw exception
			return d_feedbackWithPosition[l].first;
	}

	inline const Vector3 & PhysicsPluginData::grfPos(amarsi::Limbs l) const{
		if(l==amarsi::NONE || l==amarsi::NUMBER_OF_LIMBS)
			return Vector3::ZERO;/// \todo should throw exception
		return d_feedbackWithPosition[l].second;
	}

	inline const SetOfBody & PhysicsPluginData::deniedContactBodies() const{
		return d_deniedContactBodies;
	}

	inline void PhysicsPluginData::resetFeedbackData(){
		d_currentContactBodies.clear();
		d_feedbacks.clear();
		d_feedbackNum=0;
		d_numberOfHandledContact=0;
	}

	inline void PhysicsPluginData::disablePhysics(){
		std::cerr<<"Disable the physics"<<std::endl;
		d_physicsEnabled=false;
		exit(1);
	}

	inline bool PhysicsPluginData::good() const{
		return d_physicsEnabled;
	}

	inline amarsi::DisplayMode PhysicsPluginData::mode() const{
		return d_m;
	}


	//  inline void PhysicsPluginData::addKneeJoint(dJointID jid, amarsi::Limbs limb){
	//    if(limb==amarsi::NONE || limb==amarsi::NUMBER_OF_LIMBS)
	//      return; /// \todo should throw exception.
	//    d_femurTibia[limb]=jid;
	//  }

	inline bool PhysicsPluginData::isAFloorElement(dGeomID id) const{
		SetOfGeom::const_iterator fi=d_floorElements.find(id);
		return fi!=d_floorElements.end();
	}

	inline void PhysicsPluginData::addFeedback(dJointID joint,
	                                           amarsi::Limbs l,
	                                           const dVector3 & pos){
		dJointSetFeedback(joint, &d_feedbackTable[d_feedbackNum]);
		d_feedbackPosTable[d_feedbackNum][0]=pos[0];
		d_feedbackPosTable[d_feedbackNum][1]=pos[1];
		d_feedbackPosTable[d_feedbackNum][2]=pos[2];
		d_feedbacks.insert(std::make_pair(l,d_feedbackNum));
		++d_feedbackNum;
	}

	inline bool PhysicsPluginData::getFloorElement(dGeomID & floorElement,
	                                               dGeomID & other){
		if(numberOfFloorFirst>=numberOfFloorSecond){
			if(isAFloorElement(floorElement)){
				++numberOfFloorFirst;
				return true;
			}else if (isAFloorElement(other)){
				++numberOfFloorSecond;
				dGeomID tmp = floorElement;
				floorElement = other;
				other = tmp;
				return true;
			}
		}else{
			if(isAFloorElement(other)){
				++numberOfFloorSecond;
				dGeomID tmp = floorElement;
				floorElement = other;
				other = tmp;
				return true;
			}else if (isAFloorElement(floorElement)){
				++numberOfFloorFirst;
				return true;
			}
		}
		return false;
	}

	inline bool PhysicsPluginData::isInContact(dBodyID body){
		SetOfBody::const_iterator bi = d_currentContactBodies.find(body);
		return bi != d_currentContactBodies.end();
	}

	inline void PhysicsPluginData::printAllBodyInContactOnStderr() const{
		std::cerr<<"There is "<<d_currentContactBodies.size()<<" body in contact."<<std::endl;
		for(SetOfBody::const_iterator body = d_currentContactBodies.begin();
				body != d_currentContactBodies.end();
				++body){
			std::cerr<<"  Body : "<<*body<<std::endl;
		}
	}

	inline void PhysicsPluginData::setMainBody(const std::string & name){
		d_mainBody = dWebotsGetBodyFromDEF(name.c_str());
		if(!d_mainBody){
			std::cerr<<"Didn't not found the specified main body (DEFNAME : "<<name
					<<"), so I quit."<<std::endl;
			exit(1);
		}
	}

	inline void PhysicsPluginData::perturbate(const Vector3 & force,
	                                          unsigned int nbStep){
		if(d_nbStep != 0){
			std::cerr<<"Already have a pertubation, we drop it"<<std::endl;
			return; // \todo should throw an exceotion
		}
		d_nbStep=nbStep;
		d_targetForce = force;
	}

	inline void PhysicsPluginData::setDisplayMode(DisplayMode mode){
		d_m = mode;
	}
	inline void PhysicsPluginData::resetSimulationAtEndOfStep(){
		d_resetAsked = true;
	}
	inline bool PhysicsPluginData::resetAsked(){
		return d_resetAsked;
	}

	inline void PhysicsPluginData::resetSimulation(){
		for(SetOfResetter::iterator body = d_bodyToReset.begin();
		    body != d_bodyToReset.end();
		    ++body){
			body->restoreLast();
		}
		d_resetAsked = false;
	}

	inline void PhysicsPluginData::addResettableBody(dBodyID id){
		if(!id){
			std::cerr<<"Unable to add resettable body, I quit"<<std::endl;
			exit(1);
		}
		d_bodyToReset.push_back(BodyResetter(id));
	}

	inline void PhysicsPluginData::saveState(){
		for(SetOfResetter::iterator body = d_bodyToReset.begin();
				body != d_bodyToReset.end();
				++body){
			body->push();
		}
	}
}

#endif // LIBAMARSI_QUAD_PHYSICSPLUGINDATA_H_
