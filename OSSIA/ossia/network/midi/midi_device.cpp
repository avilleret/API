#include "midi_device.hpp"
#include <ossia/network/midi/detail/midi_impl.hpp>
#include <ossia/network/midi/midi_protocol.hpp>

namespace ossia
{
namespace net
{
namespace midi
{
midi_device::midi_device(std::unique_ptr<protocol_base> prot)
    : ossia::net::device_base{std::move(prot)}, midi_node{*this, *this}
{
}

std::string midi_device::getName() const
{
  return mName;
}

node_base& midi_device::setName(std::string n)
{
  mName = n;
  return *this;
}

bool midi_device::updateNamespace()
{
  clearChildren();

  try
  {
    for (int i = 1; i <= 16; i++)
    {
      auto ptr = std::make_unique<channel_node>(i, *this);
      mChildren.push_back(std::move(ptr));
    }
  }
  catch (...)
  {
    std::cerr << "refresh failed\n";
    return false;
  }
  return true;
}
}
}
}
