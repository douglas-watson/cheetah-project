/**
 * \file PhysicsPluginData.cpp
 *
 *  \date Jun 28, 2010
 *  \author tuleu
 */

#include "PhysicsPluginData.h"
#include "MotionRestrictor.h"

namespace amarsi {

  PhysicsPluginData::PhysicsPluginData()
  : d_m(NO_DISPLAY)
  , d_nbStep(0)
  , d_feedbackNum(0)
  , d_numberOfHandledContact(0)
  , numberOfFloorFirst(0)
  , numberOfFloorSecond(0)
  , d_resetAsked(false){
    // TODO Auto-generated constructor stub

    d_feedbackWithPosition.assign(amarsi::NUMBER_OF_LIMBS,
                                  std::make_pair(Vector3::ZERO,
                                                 Vector3::ZERO));

    /// \todo Place here default value from prefs
    d_toeCFM=0.001;
    d_toeERP=0.04;
    d_toeFriction=2.0;

//    d_femurTibia.assign(amarsi::NUMBER_OF_LIMBS,0);
  }

  PhysicsPluginData::~PhysicsPluginData() {
    delete d_motionRestrictor;
  }


  void PhysicsPluginData::processFeedbackWithPosition(){
    d_feedbackWithPosition.clear();
    d_feedbackWithPosition.assign(amarsi::NUMBER_OF_LIMBS,
                                  std::make_pair(Vector3::ZERO,
                                                 Vector3::ZERO));

    Vector3 totalForce,position;
    double sumNorm;
    for(amarsi::Limbs limb=LEFT_FORE;limb<amarsi::NUMBER_OF_LIMBS;
        limb = (amarsi::Limbs) (limb + 1)){
      totalForce=Vector3::ZERO;
      position=Vector3::ZERO;
      sumNorm=0.0;
      std::pair<MapOfFeedback::const_iterator,
                MapOfFeedback::const_iterator> contactForcesOfTheLimb =
          d_feedbacks.equal_range(limb);

      for(MapOfFeedback::const_iterator feedMapping = contactForcesOfTheLimb.first;
          feedMapping != contactForcesOfTheLimb.second;
          ++feedMapping){
        dJointFeedback &j=d_feedbackTable[feedMapping->second];
        //get the force in the right direction
        Vector3 fforce(j.f1[1] >0 ? j.f1 : j.f2);
        totalForce += fforce;
        double norm=fforce.norm();
        position += Vector3(d_feedbackPosTable[feedMapping->second]) * norm;
        sumNorm += norm;
      }
      if(contactForcesOfTheLimb.first != contactForcesOfTheLimb.second){
        position/=sumNorm;
        d_feedbackWithPosition[limb].first=totalForce;
        d_feedbackWithPosition[limb].second=position;
      }
    }
  }

  void PhysicsPluginData::applyPerturbation(){
    d_actualForce += 0.1*(d_targetForce-d_actualForce);


    if(d_nbStep>0){
      --d_nbStep;//count down to stop the perturbation
    }
    if(d_nbStep == 0){//we stop the perturbation (smoothly !!!)
      d_targetForce=Vector3::ZERO;
      for(unsigned int i=0;i<3;++i)//we threshold to stop it if small for each
                                    //coordinates
        if(std::abs(d_actualForce[i])<0.1)
          d_actualForce[i]=0.0;
    }
    if(d_actualForce != Vector3::ZERO){
      DEBUG_OUT("Apply force "<<d_actualForce)
      dBodyAddForceAtRelPos(d_mainBody,
                            d_actualForce.cx(),
                            d_actualForce.cy(),
                            d_actualForce.cz(),
                            0.0,0.0,0.0);//apply force to the center of mass of
                                         //the main body (heaviest part of the robot)
    }
  }

  const Vector3 & PhysicsPluginData::currentPerturbationForce() const{
    return d_actualForce;
  }
  Vector3 PhysicsPluginData::currentPertubationApplyPoint() const{
    dVector3 pos;
    dBodyGetRelPointPos(d_mainBody,0.0,0.0,0.0,pos);
    return Vector3(pos);
  }


  dContact * PhysicsPluginData::getContactStructure(){
    return &(d_handledContactJoint[d_numberOfHandledContact]);
  }

  void PhysicsPluginData::addNewContacts(unsigned int nbContact){
    d_numberOfHandledContact+=nbContact;
  }
}
