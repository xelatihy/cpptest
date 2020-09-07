#ifdef USE_ABSEIL
#include <absl/container/flat_hash_map.h>
#include <absl/container/node_hash_map.h>
#endif

#include <array>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "ext/robin_hood.h"

using std::array;
using std::string;
using std::vector;

template <typename K, typename V>
using unordered_map = std::unordered_map<K, V>;
template <typename K, typename V>
using unordered_flat_map = robin_hood::unordered_flat_map<K, V>;
template <typename K, typename V>
using unordered_node_map = robin_hood::unordered_node_map<K, V>;
#ifdef USE_ABSEIL
template <typename K, typename V>
using flat_hash_map = absl::flat_hash_map<K, V>;
template <typename K, typename V>
using node_hash_map = absl::node_hash_map<K, V>;
#endif

struct timer {
  timer(string msg) : start{get_time()}, msg{msg} {}
  int64_t elapsed() { return get_time() - start; }
  string  elapsedf() {
    auto duration = get_time() - start;
    auto elapsed  = duration / 1000000;  // milliseconds
    auto hours    = (int)(elapsed / 3600000);
    elapsed %= 3600000;
    auto mins = (int)(elapsed / 60000);
    elapsed %= 60000;
    auto secs  = (int)(elapsed / 1000);
    auto msecs = (int)(elapsed % 1000);
    char buffer[256];
    sprintf(buffer, "%02d:%02d:%02d.%03d", hours, mins, secs, msecs);
    return buffer;
  }
  ~timer() { printf("%s in %s\n", msg.c_str(), elapsedf().c_str()); }
  static int64_t get_time() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
  }

 private:
  int64_t start = 0;
  string  msg   = "";
};

using float3 = array<float, 3>;
inline float3& operator+=(float3& a, const float3& b) {
  a[0] += b[0];
  a[1] += b[1];
  a[2] += b[2];
  return a;
}

const auto repetitions = 100;

float3 test_raw_pointers(const string& name, const vector<float3>& positions,
    const vector<int>& shapes) {
  struct Shape {
    vector<float3> positions = {};
  };
  struct Instance {
    Shape* shape = nullptr;
  };
  struct Scene {
    vector<Shape*>    shapes    = {};
    vector<Instance*> instances = {};
    ~Scene() {
      for (auto shape : shapes) delete shape;
      for (auto instance : instances) delete instance;
    }
  };
  auto scene = (Scene*)nullptr;
  auto sum   = float3{0, 0, 0};
  {
    // create
    auto timer = ::timer{name + " new"};
    scene      = new Scene{};
    for (auto position : positions) {
      auto shape       = scene->shapes.emplace_back(new Shape{});
      shape->positions = {position};
    }
    for (auto shape : shapes) {
      auto instance   = scene->instances.emplace_back(new Instance{});
      instance->shape = scene->shapes[shape];
    }
  }
  {
    // lookup
    auto timer = ::timer{name + " sum"};
    for (auto count = 0; count < repetitions; count++) {
      sum = float3{0, 0, 0};
      for (auto instance : scene->instances) {
        sum += instance->shape->positions[0];
      }
    }
  }
  {
    // cleanup
    auto timer = ::timer{name + " del"};
    delete scene;
  }
  return sum;
}

float3 test_vector_pointers(const string& name, const vector<float3>& positions,
    const vector<int>& shapes) {
  struct Shape {
    vector<float3> positions = {};
  };
  struct Instance {
    int shape = -1;
  };
  struct Scene {
    vector<Shape*>    shapes    = {};
    vector<Instance*> instances = {};
    ~Scene() {
      for (auto shape : shapes) delete shape;
      for (auto instance : instances) delete instance;
    }
  };
  auto scene = (Scene*)nullptr;
  auto sum   = float3{0, 0, 0};
  {
    // create
    auto timer = ::timer{name + " new"};
    scene      = new Scene{};
    for (auto position : positions) {
      auto shape       = scene->shapes.emplace_back(new Shape{});
      shape->positions = {position};
    }
    for (auto shape : shapes) {
      auto instance   = scene->instances.emplace_back(new Instance{});
      instance->shape = shape;
    }
  }
  {
    // lookup
    auto timer = ::timer{name + " sum"};
    for (auto count = 0; count < repetitions; count++) {
      sum = float3{0, 0, 0};
      for (auto instance : scene->instances) {
        sum += scene->shapes[instance->shape]->positions[0];
      }
    }
  }
  {
    // cleanup
    auto timer = ::timer{name + " del"};
    delete scene;
  }
  return sum;
}

float3 test_vector_values(const string& name, const vector<float3>& positions,
    const vector<int>& shapes) {
  struct Shape {
    vector<float3> positions = {};
  };
  struct Instance {
    int shape = -1;
  };
  struct Scene {
    vector<Shape>    shapes    = {};
    vector<Instance> instances = {};
  };
  auto scene = (Scene*)nullptr;
  auto sum   = float3{0, 0, 0};
  {
    // create
    auto timer = ::timer{name + " new"};
    scene      = new Scene{};
    for (auto position : positions) {
      auto& shape     = scene->shapes.emplace_back();
      shape.positions = {position};
    }
    for (auto shape : shapes) {
      auto& instance = scene->instances.emplace_back();
      instance.shape = shape;
    }
  }
  {
    // lookup
    auto timer = ::timer{name + " sum"};
    for (auto count = 0; count < repetitions; count++) {
      sum = float3{0, 0, 0};
      for (auto& instance : scene->instances) {
        sum += scene->shapes[instance.shape].positions[0];
      }
    }
  }
  {
    // cleanup
    auto timer = ::timer{name + " del"};
    delete scene;
  }
  return sum;
}

template <template <typename K, typename V> typename hash_map>
float3 test_map_pointers(const string& name, const vector<float3>& positions,
    const vector<int>& shapes) {
  struct Shape {
    vector<float3> positions = {};
  };
  struct Instance {
    int shape = -1;
  };
  struct Scene {
    hash_map<int, Shape*>    shapes;
    hash_map<int, Instance*> instances;
    ~Scene() {
      for (auto [_, shape] : shapes) delete shape;
      for (auto [_, instance] : instances) delete instance;
    }
  };
  auto scene = (Scene*)nullptr;
  auto sum   = float3{0, 0, 0};
  {
    // create
    auto timer = ::timer{name + " new"};
    scene      = new Scene();
    for (auto shape_id = 0; shape_id < positions.size(); shape_id++) {
      auto shape              = new Shape{};
      shape->positions        = {positions[shape_id]};
      scene->shapes[shape_id] = shape;
    }
    for (auto instance_id = 0; instance_id < shapes.size(); instance_id++) {
      auto instance                 = new Instance{};
      instance->shape               = shapes[instance_id];
      scene->instances[instance_id] = instance;
    }
  }
  {
    // lookup
    auto timer = ::timer{name + " sum"};
    for (auto count = 0; count < repetitions; count++) {
      sum = float3{0, 0, 0};
      for (auto [_, instance] : scene->instances) {
        sum += scene->shapes[instance->shape]->positions[0];
      }
    }
  }
  {
    // cleanup
    auto timer = ::timer{name + " del"};
    delete scene;
  }
  return sum;
}

template <template <typename...> typename hash_map>
float3 test_map_values(const string& name, const vector<float3>& positions,
    const vector<int>& shapes) {
  struct Shape {
    vector<float3> positions = {};
  };
  struct Instance {
    int shape = -1;
  };
  struct Scene {
    hash_map<int, Shape>    shapes;
    hash_map<int, Instance> instances;
  };
  auto scene = (Scene*)nullptr;
  auto sum   = float3{0, 0, 0};
  {
    // create
    auto timer = ::timer{name + " new"};
    scene      = new Scene();
    for (auto shape_id = 0; shape_id < positions.size(); shape_id++) {
      auto shape              = Shape{};
      shape.positions         = {positions[shape_id]};
      scene->shapes[shape_id] = shape;
    }
    for (auto instance_id = 0; instance_id < shapes.size(); instance_id++) {
      auto instance                 = Instance{};
      instance.shape                = shapes[instance_id];
      scene->instances[instance_id] = instance;
    }
  }
  {
    // lookup
    auto timer = ::timer{name + " sum"};
    for (auto count = 0; count < repetitions; count++) {
      sum = float3{0, 0, 0};
      for (auto& [_, instance] : scene->instances) {
        sum += scene->shapes[instance.shape].positions[0];
      }
    }
  }
  {
    // cleanup
    auto timer = ::timer{name + " del"};
    delete scene;
  }
  return sum;
}

int main(int argc, const char** argv) {
  auto num_shapes = 10000, num_instances = 10000;
  auto positions = vector<float3>(num_shapes);
  for (auto shape = 0; shape < num_shapes; shape++) {
    positions[shape] = {(float)shape, 0, 0};
  }
  auto shapes = vector<int>(num_instances);
  for (auto instance = 0; instance < num_instances; instance++) {
    shapes[instance] =
        (int)((9187981ull * (size_t)instance) % (size_t)num_shapes);
  }
  test_raw_pointers("raw           pointers", positions, shapes);
  test_vector_pointers("vector        pointers", positions, shapes);
  test_vector_values("vector        values  ", positions, shapes);
  test_map_pointers<unordered_map>("unordered_map pointers", positions, shapes);
  test_map_values<unordered_map>("unordered_map values  ", positions, shapes);
  test_map_pointers<unordered_flat_map>(
      "robinflat_map pointers", positions, shapes);
  test_map_values<unordered_flat_map>(
      "robinflat_map values  ", positions, shapes);
  test_map_pointers<unordered_node_map>(
      "robinnode_map pointers", positions, shapes);
  test_map_values<unordered_node_map>(
      "robinnode_map values  ", positions, shapes);
#ifdef USE_ABSEIL
  test_map_pointers<flat_hash_map>("absl_flat_map pointers", positions, shapes);
  test_map_values<flat_hash_map>("absl_flat_map values  ", positions, shapes);
  test_map_pointers<node_hash_map>("absl_node_map pointers", positions, shapes);
  test_map_values<node_hash_map>("absl_node_map values  ", positions, shapes);
#endif
}
