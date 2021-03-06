#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/state/message.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/midi/detail/channel.hpp>

#include <ossia/network/midi/midi_address.hpp>
#include <ossia/network/midi/midi_device.hpp>
#include <ossia/network/midi/midi_node.hpp>

#include <iostream>

namespace ossia
{
namespace net
{
namespace midi
{
const std::string& midi_node_name(midi_size_t i);

class OSSIA_EXPORT note_on_N_node final : public midi_node
{
  const std::string& mName;

public:
  note_on_N_node(
      midi_size_t channel,
      midi_size_t note, midi_device& aDevice, ossia::net::node_base& aParent)
      : midi_node{aDevice, aParent}, mName{midi_node_name(note)}
  {
      mAddress = std::make_unique<midi_address>(
          address_info{channel, address_info::Type::NoteOn_N, note}, *this);
  }

  ~note_on_N_node() { aboutToBeDeleted(*this); }

  std::string getName() const final override
  {
    return mName;
  }
};

class OSSIA_EXPORT note_off_N_node final : public midi_node
{
  const std::string& mName;

public:
  note_off_N_node(
          midi_size_t channel,
      midi_size_t note, midi_device& aDevice, ossia::net::node_base& aParent)
      : midi_node{aDevice, aParent}, mName{midi_node_name(note)}
  {
      mAddress = std::make_unique<midi_address>(
          address_info{channel, address_info::Type::NoteOff_N, note}, *this);
  }

  ~note_off_N_node() { aboutToBeDeleted(*this); }

  std::string getName() const final override
  {
    return mName;
  }
};

class OSSIA_EXPORT control_N_node final : public midi_node
{
  const std::string& mName;

public:
  control_N_node(
      midi_size_t channel,
      midi_size_t param, midi_device& aDevice, ossia::net::node_base& aParent)
      : midi_node{aDevice, aParent}, mName{midi_node_name(param)}
  {
      mAddress = std::make_unique<midi_address>(
          address_info{channel, address_info::Type::CC_N, param}, *this);
  }

  ~control_N_node() { aboutToBeDeleted(*this); }

  std::string getName() const final override
  {
    return mName;
  }
};

class OSSIA_EXPORT program_N_node final : public midi_node
{
  const std::string& mName;

public:
  program_N_node(
      midi_size_t channel,
      midi_size_t param, midi_device& aDevice, ossia::net::node_base& aParent)
      : midi_node{aDevice, aParent}, mName{midi_node_name(param)}
  {
      mAddress = std::make_unique<midi_address>(
          address_info{channel, address_info::Type::PC_N, param}, *this);
  }

  ~program_N_node() { aboutToBeDeleted(*this); }

  std::string getName() const final override
  {
    return mName;
  }
};

class OSSIA_EXPORT program_node final : public midi_node
{
public:
  program_node(midi_size_t channel, midi_device& aDevice) :
      midi_node(aDevice, aDevice)
  {
      mAddress = std::make_unique<midi_address>(
          address_info{channel, address_info::Type::PC, 0}, *this);

      for (int i = 0; i < 128; i++)
      {
        auto ptr = std::make_unique<program_N_node>(channel, i, mDevice, *this);
        mChildren.push_back(std::move(ptr));
      }
  }

  ~program_node() { aboutToBeDeleted(*this); }

  std::string getName() const final override
  {
    using namespace std::literals;
    return "program"s;
  }
};

class OSSIA_EXPORT note_on_node final : public midi_node
{
public:
  note_on_node(midi_size_t channel, midi_device& aDevice) :
      midi_node(aDevice, aDevice)
  {
      mAddress = std::make_unique<midi_address>(
          address_info{channel, address_info::Type::NoteOn, 0}, *this);

      for (int i = 0; i < 128; i++)
      {
        auto ptr = std::make_unique<note_on_N_node>(channel, i, mDevice, *this);
        mChildren.push_back(std::move(ptr));
      }
  }

  ~note_on_node() { aboutToBeDeleted(*this); }

  std::string getName() const final override
  {
    using namespace std::literals;
    return "on"s;
  }
};

class OSSIA_EXPORT note_off_node final : public midi_node
{
public:
  note_off_node(midi_size_t channel, midi_device& aDevice) :
      midi_node(aDevice, aDevice)
  {
      mAddress = std::make_unique<midi_address>(
          address_info{channel, address_info::Type::NoteOff, 0}, *this);

      for (int i = 0; i < 128; i++)
      {
        auto ptr = std::make_unique<note_off_N_node>(channel, i, mDevice, *this);
        mChildren.push_back(std::move(ptr));
      }
  }

  ~note_off_node() { aboutToBeDeleted(*this); }

  std::string getName() const final override
  {
    using namespace std::literals;
    return "off"s;
  }
};

class OSSIA_EXPORT control_node final : public midi_node
{
public:
  control_node(midi_size_t channel, midi_device& aDevice) :
      midi_node(aDevice, aDevice)
  {
      mAddress = std::make_unique<midi_address>(
          address_info{channel, address_info::Type::CC, 0}, *this);

      for (int i = 0; i < 128; i++)
      {
        auto ptr = std::make_unique<control_N_node>(channel, i, mDevice, *this);
        mChildren.push_back(std::move(ptr));
      }
  }

  ~control_node() { aboutToBeDeleted(*this); }

  std::string getName() const final override
  {
    using namespace std::literals;
    return "control"s;
  }
};

class OSSIA_EXPORT channel_node final : public midi_node
{
  const midi_size_t mChannel;
  const std::string& mName;

public:
  channel_node(midi_size_t channel, midi_device& aDevice)
      : midi_node(aDevice, aDevice)
      , mChannel{channel}
      , mName(midi_node_name(channel))
  {
      {
        mChildren.push_back(std::make_unique<note_on_node>(mChannel, mDevice));
      }
      {
        mChildren.push_back(std::make_unique<note_off_node>(mChannel, mDevice));
      }
      {
        mChildren.push_back(std::make_unique<control_node>(mChannel, mDevice));
      }
      {
        mChildren.push_back(std::make_unique<program_node>(mChannel, mDevice));
      }
  }

  ~channel_node() { aboutToBeDeleted(*this); }

  std::array<ossia::message, 2> note_on(midi_size_t note, midi_size_t vel)
  {
      return {{
          ossia::message{ *mChildren[0]->getAddress(), std::vector<ossia::value>{int32_t{note}, int32_t{vel}}},
          ossia::message{ *mChildren[0]->children()[note]->getAddress(), int32_t{vel}}
      }};
  }

  std::array<ossia::message, 2> note_off(midi_size_t note, midi_size_t vel)
  {
      return {{
          ossia::message{*mChildren[1]->getAddress(), std::vector<ossia::value>{int32_t{note}, int32_t{vel}}},
          ossia::message{*mChildren[1]->children()[note]->getAddress(), int32_t{vel}}
      }};
  }

  std::string getName() const final override
  {
    return mName;
  }
};
}
}
}
