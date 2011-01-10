/**
 * \file CheckGroundContact.cpp
 *
 *  \date Jun 28, 2010
 *  \author tuleu
 */


#include "CheckGroundContact.h"
#include "physics_global.h"

void amarsi::CheckGroundContact::operator()(dBodyID b){
  if(!valid_)
    return;
  valid_ = ! s_data->isInContact(b);
}

bool amarsi::CheckGroundContact::valid_(false);
