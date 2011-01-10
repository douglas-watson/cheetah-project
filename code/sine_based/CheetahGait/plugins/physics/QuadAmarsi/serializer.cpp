/*
 * Copyright (c) 2008-2010 Alexandre Tuleu
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
 * \file serializer.hpp
 * \date 10/12/2008
 * \author Alexandre Tuleu
 */
#include "libamarsi-quad/Message/serializer.hpp"
#include <iostream>
#include <memory.h>


Serializer::Serializer(){
  data=new unsigned char[STARTING_SIZE_OF_BLOCK];
  serialize_pointer=0;
  unserialize_pointer=0;
  size=STARTING_SIZE_OF_BLOCK;
}

Serializer::Serializer(const unsigned char* data_, int size_)
  : data(new unsigned char[size_])
  , size(size_)
  , serialize_pointer(size_)
  , unserialize_pointer(0) {
  memcpy(&(data[0]),&(data_[0]),size_ );
}

void Serializer::set_serialized_data(const unsigned char* data_,int size_){
  if(data)
    delete []data;
  data =new unsigned char[size_];
  size=size_;
  serialize_pointer=size_;
  unserialize_pointer=0;
  memcpy(&(data[0]),&(data_[0]),size_);
}

Serializer::~Serializer(){
  if(data)
    delete []data;
}

void Serializer::create_a_bigger_block(){
  unsigned char* tmp=new unsigned char[2*this->size];
  memcpy(&(tmp[0]),&(data[0]),this->size);
  delete []data;
  data = tmp;
  size = 2*size;
}

void Serializer::generic_add(const void* value, int size_value){
  if(serialize_pointer+size_value>size)
    this->create_a_bigger_block();
  memcpy(&(data[serialize_pointer]), value, size_value);
  serialize_pointer+=size_value;
}


Serializer& operator<< (Serializer& s, const char v ){
  s.generic_add(&v, sizeof(char));
  return s;
}
Serializer& operator<< (Serializer& s, const double v){
  s.generic_add(&v, sizeof(double));
  return s;
}

Serializer& operator<< (Serializer& s, const float v){
  s.generic_add(&v, sizeof(float));
  return s;
}
Serializer& operator<< (Serializer& s, const int v){
  s.generic_add(&v, sizeof(int));
  return s;
}
Serializer& operator<< (Serializer& s, const bool v){
  s.generic_add(&v, sizeof(bool));
  return s;
}

void Serializer::generic_remove(void* v, int size_value){
  if(unserialize_pointer+size_value>size){
    unsigned char *tmp= new unsigned char[size_value];
    for(int i=0;i<size_value;++i)
      tmp[i]='\0';
    memcpy(v, &(tmp[0]), size_value);
    std::cout<<"WARNING : you overflow the content of the serializer"<<std::endl;
  } else {
    memcpy(v,&(data[unserialize_pointer]), size_value);
  }
  unserialize_pointer+=size_value;
}


Serializer& operator>> (Serializer& s, char& v){
  s.generic_remove(&v, sizeof(char));
  return s;
}
Serializer& operator>> (Serializer& s, double& v){
  s.generic_remove(&v, sizeof(double));
  return s;
}
Serializer& operator>> (Serializer& s, float& v){
  s.generic_remove(&v, sizeof(float));
  return s;
}
Serializer& operator>> (Serializer& s, int& v){
  s.generic_remove(&v, sizeof(int));
  return s;
}

Serializer& operator>> (Serializer& s, bool& v){
  s.generic_remove(&v, sizeof(bool));
  return s;
}

int Serializer::get_size() {
  return serialize_pointer;
}

unsigned char* Serializer::get_copy_of_data() {
  unsigned char *tmp= new unsigned char[this->serialize_pointer];
  memcpy(&(tmp[0]),&(data[0]), serialize_pointer);
  return tmp;
}
