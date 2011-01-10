/*
 * Copyright (c) 2010 EPFL (Ecole Polytechnique Fédérale de Lausanne)
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
 * \file MessageException.cpp
 *
 *  \date Aug 3, 2010
 *  \author tuleu
 */

#include <libamarsi-quad/Message/MessageException.h>

MessageException::MessageException(const std::string & error) throw()
  : d_error(error){
}

MessageException::~MessageException() throw(){
}


const char * MessageException::what() const throw(){
  return d_error.c_str();
}
