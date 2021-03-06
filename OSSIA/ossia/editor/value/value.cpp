#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/value/value_comparison.hpp>
#include <ossia/editor/value/value_algorithms.hpp>
#include <ossia/editor/value/value_traits.hpp>
#include <ossia/editor/value/detail/value_conversion_impl.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/detail/logger.hpp>
#include <sstream>

namespace ossia
{


template
OSSIA_EXPORT Impulse convert<Impulse>(const ossia::value& val);
template
OSSIA_EXPORT int convert<int>(const ossia::value& val);
template
OSSIA_EXPORT float convert<float>(const ossia::value& val);
template
OSSIA_EXPORT double convert<double>(const ossia::value& val);
template
OSSIA_EXPORT bool convert<bool>(const ossia::value& val);
template
OSSIA_EXPORT char convert<char>(const ossia::value& val);
template
OSSIA_EXPORT std::string convert<std::string>(const ossia::value& val);
template
OSSIA_EXPORT std::vector<ossia::value> convert<std::vector<ossia::value>>(const ossia::value& val);
template
OSSIA_EXPORT std::array<float, 2> convert<std::array<float, 2>>(const ossia::value& val);
template
OSSIA_EXPORT std::array<float, 3> convert<std::array<float, 3>>(const ossia::value& val);
template
OSSIA_EXPORT std::array<float, 4> convert<std::array<float, 4>>(const ossia::value& val);


template
OSSIA_EXPORT std::array<float, 2> convert<std::array<float, 2>>(const std::vector<ossia::value>& val);
template
OSSIA_EXPORT std::array<float, 3> convert<std::array<float, 3>>(const std::vector<ossia::value>& val);
template
OSSIA_EXPORT std::array<float, 4> convert<std::array<float, 4>>(const std::vector<ossia::value>& val);

Destination::Destination(const Destination& other) = default;
Destination::Destination(Destination&& other) = default;

Destination& Destination::operator=(const Destination&) = default;
Destination& Destination::operator=(Destination&&) = default;

Destination::Destination(ossia::net::address_base& v) : value(v)
{
  // TODO should we also copy the unit of the address ?
}

Destination::Destination(ossia::net::address_base& v, destination_index idx)
    : value(v), index(std::move(idx))
{
  // TODO should we also copy the unit of the address ?
}

Destination::Destination(ossia::net::address_base& v, destination_index idx, const ossia::unit_t& u)
    : value(v), index(std::move(idx)), unit{u}
{
}

Destination::Destination(net::address_base& v, const unit_t&u)
  : value(v), unit{u}
{

}

bool Destination::operator==(const ossia::value& v) const
{
  return comparisons::DestinationValue::apply(*this, v, std::equal_to<>{});
}

bool Destination::operator!=(const ossia::value& v) const
{
  return !comparisons::DestinationValue::apply(*this, v, std::equal_to<>{});
}

bool Destination::operator>(const ossia::value& v) const
{
  return comparisons::DestinationValue::apply(*this, v, std::greater<>{});
}

bool Destination::operator>=(const ossia::value& v) const
{
  return comparisons::DestinationValue::apply(
      *this, v, std::greater_equal<>{});
}

bool Destination::operator<(const ossia::value& v) const
{
  return comparisons::DestinationValue::apply(*this, v, std::less<>{});
}

bool Destination::operator<=(const ossia::value& v) const
{
  return comparisons::DestinationValue::apply(*this, v, std::less_equal<>{});
}

bool operator==(const Destination& lhs, const Destination& rhs)
{
  return lhs.value == rhs.value && lhs.index == rhs.index;
}

bool operator!=(const Destination& lhs, const Destination& rhs)
{
  return lhs.value != rhs.value || lhs.index != rhs.index;
}

bool operator==(const Destination& lhs, const ossia::net::address_base& rhs)
{
  return lhs.value == rhs && lhs.index.empty();
}

bool operator!=(const Destination& lhs, const ossia::net::address_base& rhs)
{
  return lhs.value != rhs || !lhs.index.empty();
}


std::string to_pretty_string(const destination_index& index)
{
  std::string str;

  const int n = index.size();
  if(n > 0)
  {
    str += "[";
    str += std::to_string(index[0]);

    for(int i = 1; i < n; i++)
    {
      str += ", " + std::to_string(index[i]);
    }
    str += "]";
  }

  return str;
}

std::string to_pretty_string(const Destination& d)
{
  using namespace std::literals;
  return
      ossia::net::address_string_from_node(d.value.get()) +
      to_pretty_string(d.index)
      + (d.unit ? ("["s + ossia::get_pretty_unit_text(d.unit) + "]"s) : ":"s);
}

template <typename Comparator>
struct value_comparison_visitor
{
  const value& rhs;
  template <typename T>
  bool operator()(const T& lhs)
  {
    return Comparator{}(lhs, rhs);
  }
};

template <typename Comparator>
struct value_comparison_visitor2;

// Definition is afterwards
template <typename Value, typename Comparator>
struct partial_lhs_value_comparison_visitor2
{
  const Value& lhs;

  template<typename T>
  bool operator()(const T& rhs);
};

template <typename Value, typename Comparator>
struct partial_rhs_value_comparison_visitor2
{
  const Value& rhs;
  template<typename T>
  bool operator()(const T& lhs);
};

template <typename Comparator>
struct value_comparison_visitor2
{
  template <typename T, typename U>
  bool operator()(const T& lhs, const U& rhs) const
  {
    return Comparator{}(lhs, rhs);
  }

  template <typename T>
  bool operator()(const T& lhs, const T& rhs) const
  {
    return Comparator{}(lhs, rhs);
  }

  // String - *
  bool operator()(const std::string& lhs, int32_t v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const std::string& lhs, float v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const std::string& lhs, bool v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const std::string& lhs, char v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }

  bool operator()(const std::string& lhs, const Vec2f& v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const std::string& lhs, const Vec3f& v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const std::string& lhs, const Vec4f& v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }


  // * - String
  bool operator()(int32_t v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(float v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(bool v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(char v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }

  bool operator()(const Vec2f& v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const Vec3f& v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const Vec4f& v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }

  // Destination
  template<typename T>
  bool operator()(const T& lhs, const Destination& d) const
  {
    return Comparator{}(lhs, d.value.get().cloneValue(d.index));
  }

  template<typename T>
  bool operator()(const Destination& d, const T& rhs) const
  {
    return Comparator{}(d.value.get().cloneValue(d.index), rhs);
  }

  bool operator()(const Impulse& lhs, const Destination& d) const
  {
    return Comparator{}(Impulse{}, Impulse{});
  }

  bool operator()(const Destination& d, const Impulse& rhs) const
  {
    return Comparator{}(Impulse{}, Impulse{});
  }

  bool operator()(const Destination& lhs, const Destination& d) const
  {
    return Comparator{}(lhs.value.get().cloneValue(d.index), d.value.get().cloneValue(d.index));
  }

  bool operator()(const std::vector<ossia::value>& lhs, const Destination& d) const
  {
    return Comparator{}(lhs, d.value.get().cloneValue(d.index));
  }

  bool operator()(const Destination& d, const std::vector<ossia::value>& rhs) const
  {
    return Comparator{}(d.value.get().cloneValue(d.index), rhs);
  }

  bool operator()(const std::array<float, 2>& lhs, const Destination& d) const
  { return Comparator{}(lhs, d.value.get().cloneValue(d.index)); }

  bool operator()(const Destination& d, const std::array<float, 2>& rhs) const
  { return Comparator{}(d.value.get().cloneValue(d.index), rhs); }

  bool operator()(const std::array<float, 3>& lhs, const Destination& d) const
  { return Comparator{}(lhs, d.value.get().cloneValue(d.index)); }

  bool operator()(const Destination& d, const std::array<float, 3>& rhs) const
  { return Comparator{}(d.value.get().cloneValue(d.index), rhs); }

  bool operator()(const std::array<float, 4>& lhs, const Destination& d) const
  { return Comparator{}(lhs, d.value.get().cloneValue(d.index)); }

  bool operator()(const Destination& d, const std::array<float, 4>& rhs) const
  { return Comparator{}(d.value.get().cloneValue(d.index), rhs); }




  // Tuple
  template<typename T>
  bool operator()(const T& lhs, const std::vector<ossia::value>& v) const
  {
    // Note : v.size == 1 only makes sense if comparator is ==...
    return (v.size() == 1) && v[0].v && (
          eggs::variants::apply(
            partial_lhs_value_comparison_visitor2<T, Comparator>{lhs},
            v[0].v));
  }

  template<typename T>
  bool operator()(const std::vector<ossia::value>& v, const T& rhs) const
  {
    // Note : v.size == 1 only makes sense if comparator is ==...
    return (v.size() == 1) && v[0].v && (
          eggs::variants::apply(
            partial_rhs_value_comparison_visitor2<T, Comparator>{rhs},
            v[0].v));
  }

  bool operator()(const Impulse& lhs, const std::vector<ossia::value>& v) const
  {
    return Comparator{}(v, lhs);
  }
  bool operator()(const std::vector<ossia::value>& lhs, const Impulse& v) const
  {
    return Comparator{}(v, lhs);
  }

  bool operator()(const std::vector<ossia::value>& lhs, const std::vector<ossia::value>& rhs) const
  {
    if (lhs.size() != rhs.size())
      return false;

    bool result = true;
    auto tit = rhs.begin();
    for (const auto& val : lhs)
    {
      if(val.valid() && tit->valid())
      {
        result &= eggs::variants::apply(*this, val.v, tit->v);
      }
      else // TODO handle case where !val && !tit
      {
        result = false;
        break;
      }
      if (!result)
        break;
      tit++;
    }

    return result;
  }


  template<std::size_t N>
  bool operator()(const std::vector<ossia::value>& lhs, const std::array<float, N>& v) const
  {
    // TODO
    return false;
  }

  template<std::size_t N>
  bool operator()(const std::array<float, N>& v, const std::vector<ossia::value>& rhs) const
  {
    // TODO
    return false;
  }

  template<typename T, std::size_t N>
  bool operator()(const T& lhs, const std::array<float, N>& v) const
  {
    return false;
  }

  template<typename T, std::size_t N>
  bool operator()(const std::array<float, N>& v, const T& rhs) const
  {
    return false;
  }

  template<std::size_t N>
  bool operator()(const std::array<float, N>& lhs, const std::array<float, N>& rhs) const
  {
    bool b = true;
    for(std::size_t i = 0; i < N; i++)
      b &= Comparator{}(lhs[i], rhs[i]);
    return b;
  }

  template<std::size_t N, std::size_t M>
  bool operator()(const std::array<float, N>& lhs, const std::array<float, M>& rhs) const
  {
    return false;
  }

};

  template<typename Value, typename Comparator> template<typename T>
  bool partial_lhs_value_comparison_visitor2<Value, Comparator>::operator()(const T& rhs)
  {
    return value_comparison_visitor2<Comparator>{}(lhs, rhs);
  }

  template<typename Value, typename Comparator> template<typename T>
  bool partial_rhs_value_comparison_visitor2<Value, Comparator>::operator()(const T& lhs)
  {
    return value_comparison_visitor2<Comparator>{}(lhs, rhs);
  }

bool operator==(const value& lhs, const value& rhs)
{
  if(lhs.v && rhs.v)
  {
    return eggs::variants::apply(
          value_comparison_visitor2<std::equal_to<>>{}, lhs.v, rhs.v);
  }
  else if(!lhs.v && !rhs.v)
  {
    return true;
  }
  return false;
}

bool operator!=(const value& lhs, const value& rhs)
{
  if(lhs.v && rhs.v)
  {
    return !eggs::variants::apply(
        value_comparison_visitor2<std::equal_to<>>{}, lhs.v, rhs.v);
  }
  else if(!lhs.v && !rhs.v)
  {
    return false;
  }
  return true;
}

bool operator>(const value& lhs, const value& rhs)
{
  if(lhs.v && rhs.v)
  {
    return eggs::variants::apply(
          value_comparison_visitor2<std::greater<>>{}, lhs.v, rhs.v);
  }
  else
  {
    return false;
  }
}

bool operator>=(const value& lhs, const value& rhs)
{
  if(lhs.v && rhs.v)
  {
    return eggs::variants::apply(
          value_comparison_visitor2<std::greater_equal<>>{}, lhs.v, rhs.v);
  }
  else if(!lhs.v && !rhs.v)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool operator<(const value& lhs, const value& rhs)
{
  if(lhs.v && rhs.v)
  {
    return eggs::variants::apply(value_comparison_visitor2<std::less<>>{}, lhs.v, rhs.v);;
  }
  else
  {
    return false;
  }
}

bool operator<=(const value& lhs, const value& rhs)
{
  if(lhs.v && rhs.v)
  {
    return eggs::variants::apply(
          value_comparison_visitor2<std::less_equal<>>{}, lhs.v, rhs.v);
  }
  else if(!lhs.v && !rhs.v)
  {
    return true;
  }
  else
  {
    return false;
  }
}

namespace
{

static void getTupleAsString(const std::vector<ossia::value>& tuple, fmt::MemoryWriter&);
struct value_prettyprint_visitor
{
  fmt::MemoryWriter& s;

  void operator()(Impulse) const
  {
    s << "impulse";
  }
  void operator()(int32_t i) const
  {
    s << "int: " << i;
  }
  void operator()(float f) const
  {
    s << "float: " << f;
  }
  void operator()(bool b) const
  {
    s << "bool: " << b;
  }
  void operator()(char c) const
  {
    s << "char: " << c;
  }
  void operator()(const std::string& str) const
  {
    s << "string: " << str;
  }
  void operator()(Vec2f vec) const
  {
    s << "vec2f: [" << vec[0] << " " << vec[1] << "]";
  }
  void operator()(Vec3f vec) const
  {
    s << "vec3f: [" << vec[0] << " " << vec[1] << " " << vec[2] << "]";
  }
  void operator()(Vec4f vec) const
  {
    s << "vec4f: [" << vec[0] << " " << vec[1] << " " << vec[2] << " " << vec[3] << "]";
  }
  void operator()(const Destination& d) const
  {
    s << "destination" << ossia::net::address_string_from_node(d.value);
    if(d.unit)
    {
      s << " " << ossia::get_pretty_unit_text(d.unit);
    }
  }
  void operator()(const std::vector<ossia::value>& t) const
  {
    s << "tuple:";
    getTupleAsString(t, s);
  }
  void operator()() const
  {
    s << "invalid";
  }
};

static void getTupleAsString(const std::vector<ossia::value>& tuple, fmt::MemoryWriter& s)
{
  value_prettyprint_visitor vis{s};

  int n = tuple.size();

  s << "[";
  for (int i = 0; i < n; i++)
  {
    const auto& val = tuple[i];

    if (val.valid())
      eggs::variants::apply(vis, val.v);
    if (i < n - 1)
      s << ", ";
  }
  s << "]";
}
}

std::string value_to_pretty_string(const ossia::value& val)
{
  fmt::MemoryWriter s;
  val.apply(value_prettyprint_visitor{s});
  return s.str();
}


ossia::value get_value_at_index(
    const ossia::value& val,
    const ossia::destination_index& idx)
{
  if(idx.empty())
    return val;
  return val.apply(detail::destination_index_retriever{idx, idx.cbegin()});
}

namespace detail
{
struct is_numeric_helper
{
  template<typename T>
  bool operator()(const T&)
  {
    return ossia::value_trait<T>::is_numeric;
  }
};

struct is_array_helper
{
  template<typename T>
  bool operator()(const T&)
  {
    return ossia::value_trait<T>::is_array;
  }
};
}

bool is_numeric(const ossia::value& val)
{
  if(val.valid())
    return eggs::variants::apply(detail::is_numeric_helper{}, val.v);
  return false;
}

bool is_array(const ossia::value& val)
{
  if(val.valid())
    return eggs::variants::apply(detail::is_array_helper{}, val.v);
  return false;
}

ossia::value convert(const ossia::value& val, ossia::val_type newtype)
{
  return lift(newtype, [&] (auto t) -> ossia::value {
    using ossia_type = typename decltype(t)::ossia_type;
    return convert<ossia_type>(val);
  });
}

value::~value() noexcept
{

}

}
