#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/midi/detail/midi_impl.hpp>
#include <array>
#include <cassert>

#include <atomic>

namespace mm
{
class MidiInput;
class MidiOutput;
}
namespace ossia
{
namespace net
{
namespace midi
{
struct OSSIA_EXPORT midi_info
{
  enum class Type
  {
    RemoteInput,
    RemoteOutput
  };

  midi_info() = default;
  midi_info(Type t, std::string d, int p)
      : type{t}, device{std::move(d)}, port{p}
  {
  }

  Type type{};
  std::string device{};
  int port{};
};

class OSSIA_EXPORT midi_protocol final : public ossia::net::protocol_base
{
  std::unique_ptr<mm::MidiInput> mInput;
  std::unique_ptr<mm::MidiOutput> mOutput;

  std::array<midi_channel, 16> mChannels;

  midi_info mInfo;

public:
  midi_protocol();
  midi_protocol(midi_info);
  ~midi_protocol();

  bool setInfo(midi_info);
  midi_info getInfo() const;

  bool pull(ossia::net::address_base&) override;
  bool push(const ossia::net::address_base&) override;
  bool observe(ossia::net::address_base&, bool) override;
  bool update(ossia::net::node_base& node_base) override;

  std::vector<midi_info> scan();
};
}
}
}
