#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/context.hpp>
#include <ossia/network/http/http.hpp>
#include <ossia/network/midi/midi.hpp>
#include <ossia/network/websocket-generic-client/ws_generic_client.hpp>
#include <iostream>
#include "../Editor/TestUtils.hpp"

using namespace ossia;
// TODO move this code where it is useful.
template<ossia::val_type>
struct matching_domain;
template<>
struct matching_domain<ossia::val_type::IMPULSE>
{ using type = ossia::net::domain_base<Impulse>; };
template<>
struct matching_domain<ossia::val_type::BOOL>
{ using type = ossia::net::domain_base<bool>; };
template<>
struct matching_domain<ossia::val_type::INT>
{ using type = ossia::net::domain_base<int>; };
template<>
struct matching_domain<ossia::val_type::FLOAT>
{ using type = ossia::net::domain_base<float>; };
template<>
struct matching_domain<ossia::val_type::CHAR>
{ using type = ossia::net::domain_base<char>; };
template<>
struct matching_domain<ossia::val_type::STRING>
{ using type = ossia::net::domain_base<std::string>; };
template<>
struct matching_domain<ossia::val_type::TUPLE>
{ using type = ossia::net::domain_base<std::vector<ossia::value>>; };
template<>
struct matching_domain<ossia::val_type::VEC2F>
{ using type = ossia::net::domain_base<Vec2f>; };
template<>
struct matching_domain<ossia::val_type::VEC3F>
{ using type = ossia::net::domain_base<Vec3f>; };
template<>
struct matching_domain<ossia::val_type::VEC4F>
{ using type = ossia::net::domain_base<Vec4f>; };

ossia::net::domain make_domain(ossia::val_type t)
{
  switch(t)
  {
    case ossia::val_type::IMPULSE:
      return ossia::net::domain_base<Impulse>{};
    case ossia::val_type::BOOL:
      return ossia::net::domain_base<bool>{};
    case ossia::val_type::INT:
      return ossia::net::domain_base<int>{};
    case ossia::val_type::FLOAT:
      return ossia::net::domain_base<float>{};
    case ossia::val_type::CHAR:
      return ossia::net::domain_base<char>{};
    case ossia::val_type::STRING:
      return ossia::net::domain_base<std::string>();
    case ossia::val_type::TUPLE:
      return ossia::net::domain_base<std::vector<ossia::value>>{};
    case ossia::val_type::VEC2F:
      return ossia::net::domain_base<Vec2f>();
    case ossia::val_type::VEC3F:
      return ossia::net::domain_base<Vec3f>();
    case ossia::val_type::VEC4F:
      return ossia::net::domain_base<Vec4f>();
    default:
      return {};
  }
}

struct remote_data
{
  remote_data(
      std::unique_ptr<ossia::net::protocol_base> local_proto,
      std::unique_ptr<ossia::net::protocol_base> remote_proto):
    local_device{std::make_unique<ossia::net::local_protocol>(), "i-score" },
    remote_device{std::move(remote_proto), "i-score-remote"}
  {
    int N = 10;

    for(int i = 0; i < N; i++)
    {
      auto cld = local_device.createChild(std::to_string(i));
      local_addr.push_back(cld->createAddress((ossia::val_type) i));
    }

    auto& proto_p = static_cast<ossia::net::local_protocol&>(local_device.getProtocol());
    proto_p.exposeTo(std::move(local_proto));

    for(int i = 0; i < N; i++)
    {
      auto cld = remote_device.createChild(std::to_string(i));
      remote_addr.push_back(cld->createAddress((ossia::val_type) i));
    }
  }

  ossia::net::generic_device local_device;
  ossia::net::generic_device remote_device;

  std::vector<ossia::net::address_base*> local_addr;
  std::vector<ossia::net::address_base*> remote_addr;

};

class DeviceTest : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  /*! test life cycle and accessors functions */
  void test_basic()
  {
    {
      ossia::net::generic_device local_device{
        std::make_unique<ossia::net::local_protocol>(), "test" };
    }
    {
      ossia::net::generic_device osc_device{
        std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997), "test_osc" };
    }
    {
      ossia::net::generic_device minuit_device{
        std::make_unique<ossia::net::minuit_protocol>("i-score", "127.0.0.1", 13579, 13580), "test_minuit" };
    }
  }

  void test_midi()
  {
    using namespace ossia::net::midi;
    try {
      auto proto = std::make_unique<midi_protocol>();
      auto res = proto->scan();
      for(auto e : res)
      {
        ossia::net::midi::midi_device dev(std::make_unique<midi_protocol>(e));
        dev.setName("dada");
        dev.updateNamespace();
      }

    }
    catch(...)
    {
    }
  }

  void test_comm_osc()
  {
    test_comm_generic(std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997),
                      std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9997, 9996));
  }

  void test_comm_minuit()
  {
    test_comm_generic(std::make_unique<ossia::net::minuit_protocol>("i-score-remote", "127.0.0.1", 13579, 13580),
                      std::make_unique<ossia::net::minuit_protocol>("i-score-remote", "127.0.0.1", 13580, 13579));


    int N = 10;
    auto proto = std::make_unique<ossia::net::local_protocol>();
    auto proto_p = proto.get();
    ossia::net::generic_device local_device{std::move(proto), "i-score"};

    for(int i = 0; i < N; i++)
    {
      auto cld = local_device.createChild(std::to_string(i));
      cld->createAddress((ossia::val_type) i);
    }

    proto_p->exposeTo(
          std::make_unique<ossia::net::minuit_protocol>("i-score-remote", "127.0.0.1", 13579, 13580));

    ossia::net::generic_device remote_device{
      std::make_unique<ossia::net::minuit_protocol>("i-score-remote", "127.0.0.1", 13580, 13579), "i-score-remote"};
    remote_device.getProtocol().update(remote_device);

    for(auto& n : remote_device.children())
    {
      if(auto a = n->getAddress())
      {
        a->pullValue();
      }
    }
  }

  void test_http()
  {
    int argc{}; char** argv{};
    QCoreApplication app(argc, argv);

    ossia::context context;

    QFile f("testdata/http/http_example.qml");
    f.open(QFile::ReadOnly);

    ossia::net::http_device http_device{
      std::make_unique<ossia::net::http_protocol>(
            f.readAll()),
          "test" };

    // For the sake of coverage...
    const auto& const_dev = http_device;
    QCOMPARE(&http_device.getRootNode(), &const_dev.getRootNode());

    // We have to wait a bit for the event loop to run.
    QTimer t;
    connect(&t, &QTimer::timeout, [&] () {
      auto node = ossia::net::find_node(http_device, "/tata/tutu");
      if(node)
      {
        node->getAddress()->pushValue(ossia::Impulse{});
      }
    });
    t.setInterval(1000);
    t.setSingleShot(true);
    t.start();

    QTimer::singleShot(3000, [&] () { app.exit(); });

    app.exec();
  }


private:
  const std::vector<ossia::value> value_to_test{
    ossia::Impulse{},
    int32_t{0},
    int32_t{-1000},
    int32_t{1000},
    float{0},
    float{-1000},
    float{1000},
    bool{true},
    bool{false},
    char{},
    char{'a'},
    std::string{""},
    std::string{"ossia"},
    std::vector<ossia::value>{},
    std::vector<ossia::value>{int32_t{0}},
    std::vector<ossia::value>{int32_t{0}, int32_t{1}},
    std::vector<ossia::value>{float{0}, int32_t{1}},
    std::vector<ossia::value>{float{0}, int32_t{1}, std::string{}, ossia::Impulse{}},
    std::vector<ossia::value>{float{0}, float{1000}},
    ossia::Vec2f{},
    ossia::Vec3f{},
    ossia::Vec4f{}
  };

  void test_comm_generic(
      std::unique_ptr<ossia::net::protocol_base> local_proto,
      std::unique_ptr<ossia::net::protocol_base> remote_proto)
  {
    int N = 10;
    remote_data rem{std::move(local_proto), std::move(remote_proto)};
    auto& local_addr = rem.local_addr;
    auto& remote_addr = rem.remote_addr;

    auto push_all_values = [&] {
      for(int i = 0; i < N; i++)
      {
        for(const ossia::value& val : value_to_test)
        {
          local_addr[i]->setValueType(val.getType());
          local_addr[i]->pushValue(val);
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(20));

      for(int i = 0; i < N; i++)
      {
        local_addr[i]->setValueType((ossia::val_type) i);
        local_addr[i]->pushValue(ossia::init_value((ossia::val_type) i));
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(20));

      for(int i = 0; i < N; i++)
      {
        for(const ossia::value& val : value_to_test)
        {
          remote_addr[i]->setValueType(val.getType());
          remote_addr[i]->pushValue(val);
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(20));

      for(int i = 0; i < N; i++)
      {
        remote_addr[i]->setValueType((ossia::val_type) i);
        remote_addr[i]->pushValue(ossia::init_value((ossia::val_type) i));
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    };

    push_all_values();


    for(auto val : local_addr)
    {
      rem.local_device.getProtocol().observe(*val, true);
    }
    for(auto val : remote_addr)
    {
      rem.remote_device.getProtocol().observe(*val, true);
    }
    push_all_values();


    auto test_all_values = [&] (std::vector<ossia::net::address_base*>& vec)
    {
      for(int access_i = 0; access_i < 3; access_i++)
      {
        for(int i = 0; i < N; i++)
        {
          vec[i]->setAccessMode(access_mode(access_i));
        }
        push_all_values();
        for(int bounding_i = 0 ; bounding_i < 6; bounding_i++)
        {
          for(int i = 0; i < N; i++)
          {
            vec[i]->setBoundingMode(bounding_mode(bounding_i));
          }
          push_all_values();

          for(int domain_i = 0; domain_i < N; domain_i++)
          {
            vec[domain_i]->setDomain(make_domain(ossia::val_type(domain_i)));
          }
          push_all_values();


          for(int domain_i = 0; domain_i < N; domain_i++)
          {
            auto val = ossia::init_value((ossia::val_type) domain_i);
            auto dom = ossia::net::make_domain(val, val);
            vec[domain_i]->setDomain(dom);
          }
          push_all_values();

        }
      }
    };

    test_all_values(local_addr);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    test_all_values(remote_addr);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
};


QTEST_APPLESS_MAIN(DeviceTest)

#include "DeviceTest.moc"

