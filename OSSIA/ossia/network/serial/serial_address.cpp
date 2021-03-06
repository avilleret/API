#include <ossia/network/serial/serial_address.hpp>
#include <ossia/network/serial/serial_node.hpp>

namespace ossia
{
namespace net
{

serial_address::serial_address(
    const serial_address_data& p,
    serial_node& parent):
  generic_address{parent},
  mData{p}
{
}

void serial_address::valueCallback(const value& val)
{
  this->setValue(val);
  send(mValue);
}


}
}
