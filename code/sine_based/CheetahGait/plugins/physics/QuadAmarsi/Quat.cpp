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
 * \file Quat.cpp
 *
 *  \date May 10, 2010
 *  \author tuleu
 */

#include "libamarsi-quad/basic-math/Quat.h"
#include <cmath>

// TODO optimize it a little bit (renove some copy and thing like that, should really look into quaternions
Quat::Quat(const Vector3 & v, double theta)
  : w_(std::cos(theta/2.0))
  , x_(v.cx()*std::sin(theta/2.0))
	, y_(v.cy()*std::sin(theta/2.0))
	, z_(v.cz()*std::sin(theta/2.0)){
	}

Quat::Quat(const Vector<4> & src)
 : w_(src[0])
 , x_(src[1])
 , y_(src[2])
 , z_(src[3]){

}

Quat Quat::operator*(const Quat &q) const {
  return Quat(w_ * q.w_ - x_ * q.x_ - y_ * q.y_ - z_ * q.z_,
              w_ * q.x_ + x_ * q.w_ + y_ * q.z_ - z_ * q.y_,
              w_ * q.y_ - x_ * q.z_ + y_ * q.w_ + z_ * q.x_,
              w_ * q.z_ + x_ * q.y_ - y_ * q.x_ + z_ * q.w_);
}

Vector3 Quat::rotate(const Vector3 & v) const{
	 if (w_ == 0)
	    return v;
	 Quat nq = (*this) * Quat(v) * (*this).conj();
	 return Vector3(nq.x_, nq.y_, nq.z_);
}

Quat::Quat(bool setZero){
  if(setZero){
    w_=0;
    x_=0;
    y_=0;
    z_=0;
  }
}

Quat::Quat(double w, double x, double y, double z)
 : w_(w)
 , x_(x)
 , y_(y)
 , z_(z){
}

Quat::Quat(const Vector3 & v)
 : w_(0.0)
 , x_(v.cx())
 , y_(v.cy())
 , z_(v.cz()){
}

Quat::~Quat() {
}



Quat Quat::conj() const {
	return Quat(w_,-x_,-y_,-z_);
}

