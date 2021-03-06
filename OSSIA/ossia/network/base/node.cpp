#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <boost/optional.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <iostream>
namespace ossia
{
namespace net
{
std::string sanitize_name(std::string ret)
{
  for(auto& c : ret)
  {
    if(is_valid_character_for_name(c))
      continue;
    else
      c = '_';
  }
  return ret;
}

std::string sanitize_name(std::string name_base, const std::vector<std::string>& brethren)
{
  auto name = sanitize_name(name_base);
  bool is_here = false;
  boost::optional<int> name_instance;
  std::vector<int> instance_num;
  instance_num.reserve(brethren.size());

  // First get the root name : the first part of the "a.b"
  std::string root_name = name;
  {
    auto pos = name.find_last_of('.');
    if(pos != std::string::npos)
    {
      try
      {
        name_instance = std::stoi(name.substr(pos + 1)); // OPTIMIZEME
        root_name = name.substr(0, pos);
      }
      catch (...)
      {
      }
    }
  }

  auto root_len = root_name.length();
  for (const std::string& n_name : brethren)
  {
    if (n_name == name)
    {
      is_here = true;
    }

    if(n_name.size() < root_len)
      continue;

    bool same_root = (n_name.compare(0, root_len, root_name) == 0);
    if (same_root && (n_name[root_len] == '.'))
    {
      // Instance
      try
      {
        int n = std::stoi(n_name.substr(root_len + 1)); // OPTIMIZEME
        instance_num.push_back(n);
      }
      catch (...)
      {
        continue;
      }
    }
    // case where we have the "default" instance without .0
    else if(same_root && root_len == n_name.length())
    {
      instance_num.push_back(0);
    }
  }

  if (!is_here)
  {
    return name;
  }
  else
  {
    auto n = instance_num.size();
    if ((n == 0) || ((n == 1) && (instance_num[0] == 0)))
    {
      return root_name + ".1";
    }
    else
    {
      std::sort(instance_num.begin(), instance_num.end());
      return root_name + "." + std::to_string(instance_num.back() + 1);
    }
  }
}

node_base::~node_base() = default;

node_base* node_base::createChild(const std::string& name)
{
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return nullptr;

  auto res = makeChild(sanitize_name(name));

  auto ptr = res.get();
  if (res)
  {
    mChildren.push_back(std::move(res));
    dev.onNodeCreated(*ptr);
  }

  return ptr;
}

node_base* node_base::findChild(const std::string& name)
{
  for(auto& node : mChildren)
  {
    if(node->getName() == name)
      return node.get();
  }

  return nullptr;
}

bool node_base::removeChild(const std::string& name)
{
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return false;

  auto it = find_if(
              mChildren, [&](const auto& c) { return c->getName() == sanitize_name(name); });

  if (it != mChildren.end())
  {
    dev.onNodeRemoving(**it);
    removingChild(**it);
    mChildren.erase(it);

    return true;
  }
  else
  {
    return false;
  }
}

bool node_base::removeChild(const node_base& n)
{
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return false;
  auto it = find_if(mChildren, [&](const auto& c) { return c.get() == &n; });

  if (it != mChildren.end())
  {
    dev.onNodeRemoving(**it);
    removingChild(**it);
    mChildren.erase(it);

    return true;
  }
  else
  {
    return false;
  }
}

void node_base::clearChildren()
{
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return;

  for (auto& child : mChildren)
    removingChild(*child);
  mChildren.clear();
}
}
}
