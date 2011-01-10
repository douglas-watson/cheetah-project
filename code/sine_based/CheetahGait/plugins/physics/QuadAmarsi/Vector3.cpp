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
 * \file Vector3.cpp
 *
 *  \date May 10, 2010
 *  \author tuleu
 */

#include "libamarsi-quad/basic-math/Vector3.h"

Vector3 operator^(const Vector3 & a, const Vector3 & b){
	Vector3 res;
	a.prodVect(b,res);
	return res;
}

Vector3::Vector3(bool zero)
 : Vector<3>(zero){
}

Vector3::~Vector3() {
}

void Vector3::prodVect(const Vector3 & other, Vector3 & result) const {
	result._[0]=_[1]*other._[2]-_[2]*other._[1];
	result._[1]=_[2]*other._[0]-_[0]*other._[2];
	result._[2]=_[0]*other._[1]-_[1]*other._[0];
}

 Vector3::Vector3(const Vector3 & src)
 : Vector<3>(src){
}

 Vector3::Vector3(const double *a)
 : Vector<3>(a){
}


const Vector3 & Vector3::operator=(const Vector<3> & src){
  _[0]=src[0];
  _[1]=src[1];
  _[2]=src[2];
  return *this;
}
 const Vector3 Vector3::ZERO(0.0,0.0,0.0);
 const Vector3 Vector3::EX(1.0,0.0,0.0);
 const Vector3 Vector3::EY(0.0,1.0,0.0);
 const Vector3 Vector3::EZ(0.0,0.0,1.0);
