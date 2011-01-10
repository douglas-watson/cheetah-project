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
 * \file webots_physics_draw.cpp
 *
 *  \date May 19, 2010
 *  \author tuleu
 */
#include <iostream>
#include "physics_global.h"
#include <ode/ode.h>
#include <plugins/physics.h>

#include "glutils.h"


/**
 * Draws the knee forces or the GRF according to m_
 */
void webots_physics_draw() {
  if(s_data->mode()&amarsi::NO_DISPLAY)//no drawing required.
    return

   glPushAttrib(GL_ALL_ATTRIB_BITS);

   glDisable(GL_LIGHTING);
   glLineWidth(2);
   glColor3f(0,0,0);
   double factor=0.002;

   glLineWidth(4);
   //glDisable(GL_DEPTH_TEST);
   bool drawPretty = s_data->mode() & amarsi::USE_PRETTY_ARROW;
   if(s_data->mode()& amarsi::SHOW_GROUND_REACTION_FORCES){
     for(amarsi::Limbs l=amarsi::LEFT_FORE;l< amarsi::NUMBER_OF_LIMBS;
         l = (amarsi::Limbs) (l+1)){
       const Vector3 & grf(s_data->grf(l));
       if(grf!=Vector3::ZERO){

           GLUtils::drawVec(s_data->grfPos(l),grf,
                            factor*10,Vector3(0.0,0.0,1.0),Vector3(1.0,1.0,0.0),
                            drawPretty);


       }
     }

   }

   if(s_data->mode() & amarsi::SHOW_PERTURBATION_FORCE){
     const Vector3 & pertu = s_data->currentPerturbationForce();
     if(pertu.norm()>0.0){
       GLUtils::drawVec(s_data->currentPertubationApplyPoint(),pertu,
                        factor*10,Vector3(1.0,0.5,0.5),Vector3(1.0 , 1.0 , 0.0)
                        ,drawPretty);
     }
   }
   glPopAttrib();
}

