/*!
 * \file publication.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <iostream>
#include <memory>
#include <functional>

#include <ossia/ossia.hpp>

using namespace ossia;
using namespace std;

void printValueCallback(const value& v);
void printValue(const value& v);

int main()
{
  auto local_proto_ptr = std::make_unique<ossia::net::local_protocol>();
  ossia::net::local_protocol& local_proto = *local_proto_ptr;
  // declare this program "B" as Local device
  ossia::net::generic_device device{std::move(local_proto_ptr), "B"};

  /* publish each feature of program "B" as address into a tree
     /test
     /test/my_bang
     /test/my_bool
     /test/my_int
     /test/my_float
     /test/my_string
     /test/my_destination
     /test/my_tuple
     */


  auto localTestNode = device.createChild("test");

  auto localImpulseNode = localTestNode->createChild("my_impulse");
  auto localImpulseAddress = localImpulseNode->createAddress(val_type::IMPULSE);
  localImpulseAddress->add_callback(printValueCallback);

  auto localBoolNode = localTestNode->createChild("my_bool");
  auto localBoolAddress = localBoolNode->createAddress(val_type::BOOL);
  localBoolAddress->add_callback(printValueCallback);

  auto localIntNode = localTestNode->createChild("my_int");
  auto localIntAddress = localIntNode->createAddress(val_type::INT);
  localIntAddress->add_callback(printValueCallback);

  auto localFloatNode = localTestNode->createChild("my_float");
  auto localFloatAddress = localFloatNode->createAddress(val_type::FLOAT);
  localFloatAddress->add_callback(printValueCallback);

  auto localStringNode = localTestNode->createChild("my_string");
  auto localStringAddress = localStringNode->createAddress(val_type::STRING);
  localStringAddress->add_callback(printValueCallback);

  // auto localDestinationNode = localTestNode->createChild("my_destination");
  // auto localDestinationAddress = localDestinationNode->createAddress(val_type::DESTINATION);
  // localDestinationAddress->addCallback(printValueCallback);

  auto localTupleNode = localTestNode->createChild("my_tuple");
  auto localTupleAddress = localTupleNode->createAddress(val_type::TUPLE);
  localTupleAddress->add_callback(printValueCallback);

  // update tree value
  localImpulseAddress->pushValue(Impulse{});
  localBoolAddress->pushValue(true);
  localIntAddress->pushValue(123);
  localFloatAddress->pushValue(0.5);
  localStringAddress->pushValue("hello world !"s);

  // FIXME
  // Destination d(localFloatNode);
  // localDestinationAddress->pushValue(&d);

  localTupleAddress->pushValue(std::vector<ossia::value>{0., 1., 2.});

  // declare a distant program as a Minuit device
  local_proto.exposeTo(std::make_unique<net::minuit_protocol>("B", "127.0.0.1", 9999, 6666));

  while (true)
    ;
}

void printValueCallback(const value& v)
{
  cerr << "Callback: " << value_to_pretty_string(v) << "\n";
}

