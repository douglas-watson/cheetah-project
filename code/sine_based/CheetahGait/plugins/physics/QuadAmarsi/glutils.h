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
 * \file glutils.h
 *
 *  \date May 19, 2010
 *  \author tuleu
 */

#ifndef LIBAMARSI_QUAD_GLUTILS_H_
#define LIBAMARSI_QUAD_GLUTILS_H_

class Vector3;

//void drawVec(const Vector3 & pos, const Vector3 &direc, double factor,
//             const Vector3 & color1, const Vector3 & color2);


class GLUtils {
public:

  static void drawVec(const Vector3 & pos , const Vector3 &direc ,
                      double factor , const Vector3 & color1 ,
                      const Vector3 & color2, bool pretty);

  static void drawVecBasic(const Vector3 & pos , const Vector3 &direc ,
                           double factor , const Vector3 & color1 ,
                           const Vector3 & color2);

  static void drawVecPretty(const Vector3 & pos , const Vector3 &direc ,
                            double factor , const Vector3 & color);

private :
  struct GLList{
    GLList();
    GLuint id;
    bool initialized;
  };

  static GLList d_prettyArrowList;

};
#endif // libamarsi-quad_GLUTILS_H_
