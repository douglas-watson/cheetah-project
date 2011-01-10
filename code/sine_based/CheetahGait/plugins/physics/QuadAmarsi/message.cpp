/*
 * Copyright (c) 2008-2010 Mikaël Mayer
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
 * \file          message.cpp
 * \date          14/10/2008
 * \brief         Protocol to communicate from roombots to supervisor
 *                Implementation file
 * \author        Mika\"el Mayer
 */

#include <string.h>
#include <stdio.h>
#include <sstream>

#include "libamarsi-quad/Message/message_private.hpp"
#include "libamarsi-quad/Message/MessageException.h"






// Useful reminder if the user forgets to register an emitter/receiver.
const char *EMITTER_RECEIVER_TIP=
    "To register an emitter/receiver:\n * Add for example #include \"emitter_receiver_robot.hpp\"; in your main file.\n * Add Message::set_emitter_receiver(EmitterReceiverRobot::create());\n   at the beginning of your code.";

const char *REGISTRATION_TIP =
    "To register a message, see files in the example folder:\nmy_message.hpp, my_message.cpp and my_message_main.cpp.";

// Message implementation

Message::Message() {
  m = NULL;
}
Message::Message(InternalMessage *m_ex) {
  m = m_ex;
}

Message::~Message() {
  if(m) delete m;
}

Message::Message(const Message &message) {
  m = message.m->clone();
}

Message& Message::operator=(const Message &message) {
  if(m) delete m;
  m = message.m->clone();
  return *this;
}

int Message::get_id_sender() {
  if(m) return m->get_id_sender();
  else throw MessageException("Trying to get sender of an empty Message.");
}

int Message::get_id_receiver() {
  if(m) return m->get_id_receiver();
  else throw MessageException("Trying to get receiver of an empty Message.");
}

bool Message::am_I_receiver(int id) {
  if(m) return m->am_I_receiver(id);
  else throw MessageException("Trying to check recipients of an empty Message.");
}

void Message::set_emitter_receiver(EmitterReceiver *er) {
  if(InternalMessage::emitter_receiver && InternalMessage::emitter_receiver != er)
    delete InternalMessage::emitter_receiver;
  InternalMessage::emitter_receiver = er;
}

Message Message::receive_message() {
  return Message(InternalMessage::receive_message());
}


Message Message::receive_message(const unsigned char* data, int size){
  return Message(InternalMessage::receive_message(data,size));
}

MessageType Message::type() {
  if(m) return m->type();
  else throw MessageException("Trying to get type of an empty Message.");
}

// InternalMessage implementation
#include <iostream>

bool InternalMessage::register_type(MessageType type_n, MESSAGE_CREATOR registration) {
  unsigned int id=static_cast<unsigned int>(type_n);
  //std::cout<<"id : "<<id<<" "<<registration<<std::endl;
  //MESSAGE_CREATOR t = registered_types[id];
  /*if(t != NULL||registration==NULL) {
    throw Exception("Already registered message type : %d, %d\nMessage type not registered.\n%s", type_n,t, REGISTRATION_TIP);
  }*/
  if(registeredTypes_==NULL)
    registeredTypes_=new std::list<MessageType>();

  registered_types[id] = registration;
  registeredTypes_->push_back(type_n);
  //std::cout<<(unsigned int)(registered_types[id])<<std::endl;
  num_registered_types += 1;
  return true;
}

InternalMessage::InternalMessage(int id_send_ex, int id_recv_ex):
    id_send(id_send_ex),id_recv(id_recv_ex) {
}

int InternalMessage::get_id_sender() {
  return id_send;
}

int InternalMessage::get_id_receiver() {
  return id_recv;
}

bool InternalMessage::am_I_receiver(int id) {
  return (id_recv == ALL_RECIPIENTS) || (id_recv == id);
}

MESSAGE_CREATOR InternalMessage::get_message_creator(unsigned char type) {
  return registered_types[type];
}

bool InternalMessage::is_registered() {
  return get_message_creator(this->type()) != NULL;
}

void InternalMessage::serialize_complete(unsigned char** data, int *returnedSize) {
  if(!is_registered()) {
	std::ostringstream os;
	os<<"A message class (type=="<<type()<<") tried to be serialized, but class was not registered."
	  <<std::endl<<REGISTRATION_TIP<<std::endl;
	throw MessageException(os.str());
  }
  Serializer s=Serializer();
  //*size = 1 + InternalMessage::size_complete();
  //*data = new unsigned char[*size + 1];
  //unsigned char *data_current = *data;
  s<<((char) type());
  //data_current[0] = type();
  //data_current += 1;
  //Serialize id_send and id_recv
  //data_current = InternalMessage::serialize_data(data_current);
  s=InternalMessage::serialize_data(s);

  s=serialize(s);
  *returnedSize=s.get_size();
  *data=s.get_copy_of_data();
  //Serialize other fields
  //  data_current = serialize(data_current);
}

void InternalMessage::unserialize_complete(const unsigned char* data, int sizeOfData) {
  //Unserialize id_send and id_recv
  Serializer s=Serializer(data,sizeOfData);
  s=InternalMessage::unserialize_data(s);

  //Unserialize other fields
  s = unserialize(s);
}

/*unsigned char* InternalMessage::add_data(unsigned char* &data, const void* field, int field_size) {
  memcpy(data, field, field_size);
  data += field_size;
  return data;
}
const unsigned char* InternalMessage::get_data(const unsigned char* &data, void* field, int field_size) {
  memcpy(field, data, field_size);
  data += field_size;
  return data;
  }*/

//#ifndef DATA_STRING
Serializer& InternalMessage::serialize_data(Serializer& s) {
  s<<id_send<<id_recv;// data = add_data(data, (const void*)&id_send, sizeof(id_send));
  //data = add_data(data, (const void*)&id_recv, sizeof(id_recv));
  return s;
}

Serializer& InternalMessage::unserialize_data(Serializer& s) {
  s>>id_send>>id_recv;//data = get_data(data, &id_send, sizeof(id_send));
  //data = get_data(data, &id_recv, sizeof(id_recv));
  return s;
}

int InternalMessage::size_data() {
  return sizeof(id_send) + sizeof(id_recv);
}
/*#else //#ifndef DATA_STRING
#define INT_SIZE 7
unsigned char* InternalMessage::serialize_data(unsigned char *data) {
  int n = sprintf((char*)data,"%+06d %+06d", id_send, id_recv) + 1;
  if(n != size_data()) throw Exception("Bad sprintf in Message::serialize_data. Got size %d instead of %d.", n, size_data());
  data += n;
  return data;
}

const unsigned char* InternalMessage::unserialize_data(const unsigned char *data) {
  int n = sscanf((char*)data,"%d %d", &id_send, &id_recv);
  if(n != 2) throw Exception("Unable to get the Message fields from '%s'", data);
  data += size_data();
  return data;
}

int InternalMessage::size_data() {
  return 2*INT_SIZE;
}
#endif  //#ifndef DATA_STRING
*/
InternalMessage* InternalMessage::get_message_from_data(const unsigned char *data, int size) {
  size -= 1;
  MESSAGE_CREATOR create_message = get_message_creator(data[0]);
  if( create_message == NULL) {
	std::ostringstream os;
	os<<"Receiving a non-registered message type : "<<data[0]<<"."<<std::endl
	  <<REGISTRATION_TIP<<std::endl;
    throw MessageException(os.str());
  }
  InternalMessage* m = (*create_message)();
  int their_size = m->size_complete();
  if(size < their_size) {
    delete m;
	std::ostringstream os;
	os<<"Bad size for the message type : "<<data[0]<<"expecting more than "
	  <<their_size<<", got "<<size<<"."<<std::endl;
    throw MessageException(os.str());
  }
  data += 1;
  m->unserialize_complete(data, size);
  return m;
}

/**
 * Sends a message and delete it afterwards.
 **/
void InternalMessage::send_message(InternalMessage* m) {
  if(!m->is_registered()) {
	std::ostringstream os;
	os<<"A message class (type=="<<m->type()<<") tried to be sent, but class was not registered."<<std::endl
	  <<REGISTRATION_TIP<<std::endl;
	throw MessageException(os.str());
  }
  if(!emitter_receiver) {
	std::ostringstream os;
	os<<"A message class (type=="<<m->type()<<") tried to be sent, but no global emitter specified."<<std::endl
	  <<EMITTER_RECEIVER_TIP<<std::endl;
	throw MessageException(os.str());
  }
  unsigned char* data = NULL;
  int size = 0;
  m->serialize_complete(&data, &size);
  emitter_receiver->sendPacket(data, size);
  delete []data;
}

/**
 * Receives a message from the receiver queue.
 * Caution: the message has to be deleted by the calling function afterwards!
 * \return a pointer to the newly created InternalMessage.
 */
InternalMessage* InternalMessage::receive_message() {
  if(!emitter_receiver) {
	std::ostringstream os;
	os<<"Trying to receive a message, but no emitter/receiver specified."<<std::endl
	  <<EMITTER_RECEIVER_TIP<<std::endl;
	throw MessageException(os.str());
  }
  const unsigned char* data;
  int size;
  if(waiting_messages()) {
    size = emitter_receiver->getDataSize();
    data = (const unsigned char*)emitter_receiver->getData();
  } else return NULL;
  InternalMessage* m = InternalMessage::get_message_from_data(data, size);
  emitter_receiver->nextPacket();
  return m;
}

InternalMessage* InternalMessage::receive_message(const unsigned char* data, int size) {
  InternalMessage* m = InternalMessage::get_message_from_data(data, size);
  emitter_receiver->nextPacket();
  return m;
}

/**
 * If there are any messages to be picked up.
 * \return \c true if there is some waiting message.
 */
bool InternalMessage::waiting_messages() {
  if(!emitter_receiver) {
	std::ostringstream os;
	os<<"Trying to check for messages, but no emitter/receiver specified."<<std::endl
	  <<EMITTER_RECEIVER_TIP<<std::endl;
	throw MessageException(os.str());
  }
  return emitter_receiver->getQueueLength () > 0;
}

int InternalMessage::size_complete() {
  return InternalMessage::size_data() + size();
}


// Static fields

EmitterReceiver* InternalMessage::emitter_receiver = NULL;


std::list<MessageType>* InternalMessage::registeredTypes_=NULL;

MESSAGE_CREATOR InternalMessage::registered_types[MAX_TYPES] = {0};

int InternalMessage::num_registered_types = 0;



int Message::get_serialized(unsigned char* data){
  data=NULL;
  int size=0;
  this->get_internal_message()->serialize_complete(&data, &size);
  return size;
}
