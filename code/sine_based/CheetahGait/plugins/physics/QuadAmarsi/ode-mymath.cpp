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
 * \file ode-mymath.cpp
 *
 *  \date May 19, 2010
 *  \author tuleu
 */

#include <cmath>


#include "ode-mymath.h"



void diff(dVector3& res, const dVector3& a,const dVector3& b){
  res[0]=a[0]-b[0];
  res[1]=a[1]-b[1];
  res[2]=a[2]-b[2];
}

void add(dVector3& res, const dVector3& a,const dVector3& b){
  res[0]=a[0]+b[0];
  res[1]=a[1]+b[1];
  res[2]=a[2]+b[2];
}

void mult(dVector3& res, const dVector3& a, double b){
  res[0]=b*a[0];
  res[1]=b*a[1];
  res[2]=b*a[2];
}


void scal(dVector3& res, const dVector3& a, const dVector3& b){
  res[0]=b[0]*a[0];
  res[1]=b[1]*a[1];
  res[2]=b[2]*a[2];
}


double norm2(const dVector3& a){
  return std::pow(a[0],2)+std::pow(a[1],2)+std::pow(a[2],2);
}
double norm(const dVector3& a){
  return std::sqrt(norm2(a));
}

void normalize(dVector3 &res,const dVector3& a){
  double normal=norm(a);
  if(normal>0.0000000001){
    res[0]=a[0]/normal;
    res[1]=a[1]/normal;
    res[2]=a[2]/normal;
  }else{
    res[0]=0.0;
    res[1]=0.0;
    res[2]=0.0;
  }
}
