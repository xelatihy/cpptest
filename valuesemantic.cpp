#include <unistd.h>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

using float2   = array<float, 2>;
using float3   = array<float, 3>;
using int3     = array<int, 3>;
using float3x4 = array<float, 12>;

struct timer {
  timer() : start{get_time()} {}
  int64_t elapsed() { return get_time() - start; }
  string  elapsedfs() {
    auto duration = get_time() - start;
    auto elapsed  = duration / 1000000;  // milliseconds
    auto hours    = (int)(elapsed / 3600000);
    elapsed %= 3600000;
    auto mins = (int)(elapsed / 60000);
    elapsed %= 60000;
    auto secs = (int)std::round(elapsed / 1000.0);
    char buffer[256];
    sprintf(buffer, "%02d:%02d:%02d", hours, mins, secs);
    return buffer;
  }
  static int64_t get_time() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
  }

 private:
  int64_t start = 0;
};

#include <mach/mach.h>
pair<size_t, size_t> get_used_memory() {
  struct task_basic_info t_info;
  mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

  if (KERN_SUCCESS != task_info(mach_task_self(), TASK_BASIC_INFO,
                          (task_info_t)&t_info, &t_info_count))
    return {0, 0};
  return {((size_t)t_info.resident_size) / (1024 * 1024),
      ((size_t)t_info.virtual_size) / (1024 * 1024)};
}

struct value_model {
  struct shape {
    string         name      = "";
    vector<float3> positions = {};
    vector<float3> normals   = {};
    vector<int3>   triangles = {};
  };
  struct instance {
    string   name  = "";
    float3x4 frame = {};
    int      shape = -1;
  };
  vector<shape>    shapes    = {};
  vector<instance> instances = {};
};

struct unique_model {
  struct shape {
    string         name      = "";
    vector<float3> positions = {};
    vector<float3> normals   = {};
    vector<int3>   triangles = {};
  };
  struct instance {
    string   name  = "";
    float3x4 frame = {};
    shape*   shape = nullptr;
  };
  vector<unique_ptr<shape>>    shapes    = {};
  vector<unique_ptr<instance>> instances = {};
};

struct shared_model {
  struct shape {
    string         name      = "";
    vector<float3> positions = {};
    vector<float3> normals   = {};
    vector<int3>   triangles = {};
  };
  struct instance {
    string            name  = "";
    float3x4          frame = {};
    shared_ptr<shape> shape = nullptr;
  };
  vector<shared_ptr<shape>>    shapes    = {};
  vector<shared_ptr<instance>> instances = {};
};

struct raw_model {
  struct shape {
    string         name      = "";
    vector<float3> positions = {};
    vector<float3> normals   = {};
    vector<int3>   triangles = {};
  };
  struct instance {
    string   name  = "";
    float3x4 frame = {};
    shape*   shape = nullptr;
  };
  vector<shape*>    shapes    = {};
  vector<instance*> instances = {};

  ~raw_model() {
    for (auto shape : shapes) delete shape;
    for (auto instance : instances) delete instance;
  }
};

void init_scene(value_model& scene, int vertices, int triangles, int shapes,
    int instances) {
  for (auto i = 0; i < shapes; i++) {
    auto& shape = scene.shapes.emplace_back();
    shape.positions.resize(vertices);
    shape.normals.resize(vertices);
    shape.triangles.resize(triangles);
    for (auto& pos : shape.positions) pos = {1, 2, 3};
  }
  for (auto i = 0; i < instances; i++) {
    auto& instance = scene.instances.emplace_back();
    instance.shape = i % (int)scene.shapes.size();
  }
}

void init_scene(unique_ptr<unique_model>& scene, int vertices, int triangles,
    int shapes, int instances) {
  scene = make_unique<unique_model>();
  for (auto i = 0; i < shapes; i++) {
    auto shape = make_unique<unique_model::shape>();
    shape->positions.resize(vertices);
    shape->normals.resize(vertices);
    shape->triangles.resize(triangles);
    for (auto& pos : shape->positions) pos = {1, 2, 3};
    scene->shapes.push_back(std::move(shape));
  }
  for (auto i = 0; i < instances; i++) {
    auto instance   = make_unique<unique_model::instance>();
    instance->shape = scene->shapes[i % (int)scene->shapes.size()].get();
    scene->instances.push_back(std::move(instance));
  }
}

void init_scene(shared_ptr<shared_model>& scene, int vertices, int triangles,
    int shapes, int instances) {
  scene = make_shared<shared_model>();
  for (auto i = 0; i < shapes; i++) {
    auto shape = make_shared<shared_model::shape>();
    shape->positions.resize(vertices);
    shape->normals.resize(vertices);
    shape->triangles.resize(triangles);
    for (auto& pos : shape->positions) pos = {1, 2, 3};
    scene->shapes.push_back(shape);
  }
  for (auto i = 0; i < instances; i++) {
    auto instance   = make_shared<shared_model::instance>();
    instance->shape = scene->shapes[i % (int)scene->shapes.size()];
    scene->instances.push_back(instance);
  }
}

void init_scene(
    raw_model*& scene, int vertices, int triangles, int shapes, int instances) {
  scene = new raw_model{};
  for (auto i = 0; i < shapes; i++) {
    auto shape = new raw_model::shape{};
    shape->positions.resize(vertices);
    shape->normals.resize(vertices);
    shape->triangles.resize(triangles);
    for (auto& pos : shape->positions) pos = {1, 2, 3};
    scene->shapes.push_back(shape);
  }
  for (auto i = 0; i < instances; i++) {
    auto instance   = new raw_model::instance();
    instance->shape = scene->shapes[i % (int)scene->shapes.size()];
    scene->instances.push_back(instance);
  }
}

template<typename any_model>
any_model make_scene(int vertices, int triangles, int shapes, int instances) {
  auto scene = any_model{};
  init_scene(scene, vertices, triangles, shapes, instances);
  return scene;
}

void clear_scene(value_model& scene) { scene = {}; }
void clear_scene(unique_ptr<unique_model>& scene) { scene = {}; }
void clear_scene(shared_ptr<shared_model>& scene) { scene = {}; }
void clear_scene(raw_model* scene) {
  delete scene;
  scene = {};
}

double sum_vertices(value_model& scene) {
  auto sum = (double)0;
  for (auto& instance : scene.instances) {
    auto& shape = scene.shapes[instance.shape];
    for (auto& pos : shape.positions) sum += pos[0] + pos[1] + pos[2];
  }
  return sum;
}
template <typename ptr_model>
double sum_vertices(const ptr_model& scene) {
  auto sum = (double)0;
  for (auto& instance : scene->instances) {
    auto& shape = instance->shape;
    for (auto& pos : shape->positions) sum += pos[0] + pos[1] + pos[2];
  }
  return sum;
}

void erase_shapes(value_model& scene, int erases) {
  for (auto i = 0; i < erases; i++) {
    scene.shapes.erase(scene.shapes.begin() + 10);
  }
}
template <typename ptr_model>
void erase_shapes(ptr_model& scene, int erases) {
  for (auto i = 0; i < erases; i++) {
    scene->shapes.erase(scene->shapes.begin() + 10);
  }
}
void erase_shapes(raw_model* scene, int erases) {
  for (auto i = 0; i < erases; i++) {
    delete scene->shapes[10];
    scene->shapes.erase(scene->shapes.begin() + 10);
  }
}

template <typename any_scene>
void run_test(const string& message, int vertices, int triangles, int shapes,
    int instances, int erases) {
  auto [mem0s, mem1s] = get_used_memory();
  auto timer          = ::timer{};
  // auto scene          = Scene{};
  // init_scene(scene, vertices, triangles, shapes, instances);
  auto scene          = any_scene{};
  scene          = make_scene<any_scene>(vertices, triangles, shapes, instances);
  auto sum            = sum_vertices(scene);
  auto [mem0e, mem1e] = get_used_memory();
  erase_shapes(scene, erases);
  clear_scene(scene);
  auto elapsed = timer.elapsedfs();
  printf("%9s %9d %9d %9d %9s %9d %9d %9g\n", message.c_str(), shapes,
      instances, vertices, elapsed.c_str(), (int)(mem0e - mem0s),
      (int)(mem1e - mem1s), sum);
}

int main(int argc, const char** argv) {
  printf("%9s %9s %9s %9s %9s %9s %9s %9s\n", "mode", "shapes", "instances",
      "vertices", "time", "mem1", "mem2", "check");
  for (auto shapes : {5000, 15000}) {
    for (auto instance_ratio : {1, 5}) {
      for (auto vertices : {50000}) {
        for (auto triangles : {50000}) {
          run_test<value_model>("value", vertices, triangles, shapes,
              shapes * instance_ratio, shapes / 100);
          run_test<unique_ptr<unique_model>>("unique", vertices, triangles,
              shapes, shapes * instance_ratio, shapes / 100);
          run_test<shared_ptr<shared_model>>("shared", vertices, triangles,
              shapes, shapes * instance_ratio, shapes / 100);
          run_test<raw_model*>("raw", vertices, triangles, shapes,
              shapes * instance_ratio, shapes / 100);
        }
      }
    }
  }
}
