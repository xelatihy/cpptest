#include <array>
#include <chrono>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>

using namespace std;

using float2 = array<float, 2>;
using float3 = array<float, 3>;
using int3   = array<int, 3>;

struct timer {
  timer() : start{get_time()} {}
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
  static int64_t get_time() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
  }

 private:
  int64_t start = 0;
};

struct value_model {
  struct shape {
    string name = "";
    vector<float3> positions = {};
    vector<float3> normals   = {};
    vector<int3>   triangles = {};
  };
  struct instance {
    string name = "";
    int shape = -1;
  };
  vector<shape> shapes = {};
  vector<instance> instances = {};
};

struct unique_model {
  struct shape {
    string name = "";
    vector<float3> positions = {};
    vector<float3> normals   = {};
    vector<int3>   triangles = {};
  };
  struct instance {
    string name = "";
    shape* shape = nullptr;
  };
  vector<unique_ptr<shape>> shapes = {};
  vector<unique_ptr<instance>> instances = {};
};

struct shared_model {
  struct shape {
    string name = "";
    vector<float3> positions = {};
    vector<float3> normals   = {};
    vector<int3>   triangles = {};
  };
  struct instance {
    string name = "";
    shared_ptr<shape> shape = nullptr;
  };
  vector<shared_ptr<shape>> shapes = {};
  vector<shared_ptr<instance>> instances = {};
};

struct raw_model {
  struct shape {
    string name = "";
    vector<float3> positions = {};
    vector<float3> normals   = {};
    vector<int3>   triangles = {};
  };
  struct instance {
    string name = "";
    shape* shape = nullptr;
  };
  vector<shape*> shapes = {};
  vector<instance*> instances = {};
  
  ~raw_model() {
    for(auto shape : shapes) delete shape;
    for(auto instance : instances) delete instance;
  }
};

int vertices  = 100000;
int triangles = 100000;
int shapes    = 10000;
int instances = shapes * 5;
int erases    = shapes / 100;

void make_scene(value_model& scene) {
  for(auto i = 0; i < shapes; i ++) {
    auto& shape = scene.shapes.emplace_back();
    shape.positions.resize(vertices);
    shape.normals.resize(vertices);
    shape.triangles.resize(triangles);
    for(auto& pos : shape.positions) pos = {1, 2, 3};
  }
  for(auto i = 0; i < instances; i ++) {
    auto& instance = scene.instances.emplace_back();
    instance.shape = i % (int)scene.shapes.size();
  }
}

void make_scene(unique_ptr<unique_model>& scene) {
  scene = make_unique<unique_model>();
  for(auto i = 0; i < shapes; i ++) {
    auto shape = make_unique<unique_model::shape>();
    shape->positions.resize(vertices);
    shape->normals.resize(vertices);
    shape->triangles.resize(triangles);
    for(auto& pos : shape->positions) pos = {1, 2, 3};
    scene->shapes.push_back(std::move(shape));
  }
  for(auto i = 0; i < instances; i ++) {
    auto instance = make_unique<unique_model::instance>();
    instance->shape = scene->shapes[i % (int)scene->shapes.size()].get();
    scene->instances.push_back(std::move(instance));
  }
}

void make_scene(shared_ptr<shared_model>& scene) {
  scene = make_shared<shared_model>();
  for(auto i = 0; i < shapes; i ++) {
    auto shape = make_shared<shared_model::shape>();
    shape->positions.resize(vertices);
    shape->normals.resize(vertices);
    shape->triangles.resize(triangles);
    for(auto& pos : shape->positions) pos = {1, 2, 3};
    scene->shapes.push_back(shape);
  }
  for(auto i = 0; i < instances; i ++) {
    auto instance = make_shared<shared_model::instance>();
    instance->shape = scene->shapes[i % (int)scene->shapes.size()];
    scene->instances.push_back(instance);
  }
}

void make_scene(raw_model*& scene) {
  scene = new raw_model{};
  for(auto i = 0; i < shapes; i ++) {
    auto shape = new raw_model::shape{};
    shape->positions.resize(vertices);
    shape->normals.resize(vertices);
    shape->triangles.resize(triangles);
    for(auto& pos : shape->positions) pos = {1, 2, 3};
    scene->shapes.push_back(shape);
  }
  for(auto i = 0; i < instances; i ++) {
    auto instance = new raw_model::instance();
    instance->shape = scene->shapes[i % (int)scene->shapes.size()];
    scene->instances.push_back(instance);
  }
}

size_t count_vertices(value_model& scene) {
  auto vertices = (size_t)0;
  for(auto& shape : scene.shapes) vertices += shape.positions.size();
  return vertices;
}
size_t count_vertices(unique_ptr<unique_model>& scene) {
  auto vertices = (size_t)0;
  for(auto& shape : scene->shapes) vertices += shape->positions.size();
  return vertices;
}
template<typename ptr_model>
size_t count_vertices(const ptr_model& scene) {
  auto vertices = (size_t)0;
  for(auto& shape : scene->shapes) vertices += shape->positions.size();
  return vertices;
}

double sum_vertices(value_model& scene) {
  auto sum = (double)0;
  for(auto& shape : scene.shapes) {
    for(auto& pos : shape.positions)
      sum += pos[0] + pos[1] + pos[2];
  }
  return sum;
}
template<typename ptr_model>
double sum_vertices(const ptr_model& scene) {
  auto sum = (double)0;
  for(auto& shape : scene->shapes) {
    for(auto& pos : shape->positions)
      sum += pos[0] + pos[1] + pos[2];
  }
  return sum;
}

void erase_shapes(value_model& scene) {
  for(auto i = 0; i < erases; i ++) {
    scene.shapes.erase(scene.shapes.begin() + 10);
  }
}
template<typename ptr_model>
void erase_shapes(ptr_model& scene) {
  for(auto i = 0; i < erases; i ++) {
    scene->shapes.erase(scene->shapes.begin() + 10);
  }
}
void erase_shapes(raw_model* scene) {
  for(auto i = 0; i < erases; i ++) {
    delete scene->shapes[10];
    scene->shapes.erase(scene->shapes.begin() + 10);
  }
}

template<typename Scene>
void run_test(const string& message) {
  auto timer = ::timer{};
  auto scene = Scene{};
  make_scene(scene);
  erase_shapes(scene);
  auto vertices = count_vertices(scene);
  auto sum = sum_vertices(scene);
  scene = {};
  cout << message << ": " << timer.elapsedf() << " " << vertices << " " << sum << "\n";
}

int main(int argc, const char** argv) {
  run_test<value_model>("value ");
  run_test<unique_ptr<unique_model>>("unique");
  run_test<shared_ptr<shared_model>>("shared");
  run_test<raw_model*>("raw   ");
}
