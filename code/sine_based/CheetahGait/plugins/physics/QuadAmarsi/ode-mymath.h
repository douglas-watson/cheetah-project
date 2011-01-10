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
 * \file ode-mymath.h
 *
 *  \date May 19, 2010
 *  \author tuleu
 */

#ifndef LIBAMARSI_QUAD_ODEMYMATH_H_
#define LIBAMARSI_QUAD_ODEMYMATH_H_
#define _ODE_ODECPP_H_
#include <ode/ode.h>

void diff(dVector3& res, const dVector3& a,const dVector3& b);

void add(dVector3& res, const dVector3& a,const dVector3& b);

void mult(dVector3& res, const dVector3& a, double b);

void scal(dVector3& res, const dVector3& a, const dVector3& b);

double norm2(const dVector3& a);

double norm(const dVector3& a);

void normalize(dVector3 &res,const dVector3& a);


#endif // libamarsi-quad_ODEMYMATH_H_
