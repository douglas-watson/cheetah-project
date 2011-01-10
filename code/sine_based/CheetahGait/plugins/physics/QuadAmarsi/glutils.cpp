/*
 * Copyright (c) 2009-2010 ALexandre Tuleu
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
 * \file glutils.cpp
 *
 *  \date May 19, 2010
 *  \author tuleu
 */

/**
 * Draw sith OpenGL a nice arrow.
 * @param pos position of the arrow
 * @param direc direction of the arrow
 * @param factor dimension of the arrow (scale factor)
 * @param color1 color at the base of the arrow
 * @param color2 color at the end of teh arrow
 */
#define _ODE_ODECPP_H_//remove some ode bad include
#include <plugins/physics.h>

#include <libamarsi-quad/basic-math/Vector3.h>

#include "glutils.h"

void GLUtils::drawVec(const Vector3 & pos , const Vector3 &direc ,
                             double factor , const Vector3 & color1 ,
                             const Vector3 & color2, bool pretty){
  if(pretty){
    drawVecPretty(pos,direc,factor,color1);
    return;
  }
  drawVecBasic(pos,direc,factor,color1,color2);
}
void GLUtils::drawVecBasic(const Vector3 & pos, const Vector3 &direc, double factor,
                           const Vector3 & color1, const Vector3 & color2){

  Vector3 orth(direc.cy(),-direc.cx(),0.0);
  if(orth==Vector3::ZERO)
    orth=Vector3(direc.cz(),0.0,0.0);
  Vector3 orth2;
  direc.prodVect(orth,orth2);
  double factor2=direc.norm()*factor/15;


  orth.normalize();
  orth*=factor2;
  orth2.normalize();
  orth2*=factor2;

  Vector3 direcMinus(direc*factor);
  Vector3 endOfVec(pos+direcMinus);
  direcMinus*=0.93;
  direcMinus+=pos;
  glBegin(GL_LINES);
  glColor3dv(color1.toArray());
  glVertex3dv(pos.toArray());
  glColor3dv(color2.toArray());
  glVertex3dv(endOfVec.toArray());
  glEnd();
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glDisable(GL_CULL_FACE);
  glLineWidth(1.0);
  glBegin(GL_TRIANGLE_FAN);


  glVertex3dv(endOfVec.toArray());


  glVertex3dv((direcMinus+orth).toArray());


  glVertex3dv((direcMinus+orth2).toArray());


  glVertex3dv((direcMinus-orth).toArray());


  glVertex3dv((direcMinus-orth2).toArray());


  glVertex3dv((direcMinus+orth).toArray());

  glEnd();
  glPopAttrib();
}

GLUtils::GLList GLUtils::d_prettyArrowList;

GLUtils::GLList::GLList(){
  id = 0;
  initialized = false;
}

void GLUtils::drawVecPretty(const Vector3 & pos , const Vector3 &direc ,
                            double factor , const Vector3 & color){

  double lXY=std::sqrt(std::pow(direc.cx(),2)+std::pow(direc.cy(),2));
  double theta(std::atan2(direc.cx(),direc.cy()));
  double phi(std::atan2(direc.cz(),lXY));


  glPushMatrix();
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glRotated(phi,1.0,0.0,0.0);
  glRotated(theta,0.0,0.0,1.0);

  factor= direc.norm()*factor;
  glScaled(factor,factor,factor);
  glTranslated(pos.cx(),pos.cy(),pos.cz());

  glColor3dv(color.toArray());

  if(d_prettyArrowList.initialized){
    glCallList(d_prettyArrowList.id);
  } else {
    d_prettyArrowList.initialized = true;
    d_prettyArrowList.id = glGenLists(1);
    glNewList(d_prettyArrowList.id,GL_COMPILE_AND_EXECUTE);


    glBegin(GL_QUAD_STRIP);
    double cos,sin,angle;
    for(unsigned int i = 0;i<=20;++i){
      angle = i*6.28318531/((double)20);
      cos = std::cos(angle);
      sin= std::sin(angle);
      glNormal3d(cos,sin,0.0);
      glVertex3d(0.05*cos,0.05*sin,0.0);
      glVertex3d(0.05*cos,0.05*sin,0.8);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3d(0.0,0.0,1.0);
    glVertex3d(0.0,0.0,1.0);
    for(unsigned int i = 0;i<=20;++i){
      angle = i*6.28318531/((double)20);
      cos = std::cos(angle);
      sin= std::sin(angle);
      glNormal3d(cos,sin,0.0);
      glVertex3d(0.15*cos,0.05*sin,0.8);
    }
    glEnd();


    glEndList();



  }


  glPopMatrix();
  glPopAttrib();




}
