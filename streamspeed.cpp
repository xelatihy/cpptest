#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <chrono>

using namespace std;

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

const int values_per_line = 4;
const int num_values = 4 * 131072;

vector<int> int_data;
vector<float> flt_data;

void gen_data() {
    int_data.resize(num_values);
    flt_data.resize(num_values);
    for(auto& v : int_data) v = rand();
    for(auto& v : flt_data) v = rand() / (float)RAND_MAX;
}

void print_file_directly() {
    auto timer = ::timer{};
    auto fs = fopen("test/print_file_directly.txt", "wt");
    for(auto i = 0; i < num_values; i ++) {
        fprintf(fs, "%d %g ", int_data[i], flt_data[i]);
        if(i && i % values_per_line == 0) fprintf(fs, "\n");
    }
    fflush(fs);
    fclose(fs);
    cout << "print_file_directly: " << timer.elapsedf() << "\n";
}
void print_stream_directly() {
    auto timer = ::timer{};
    auto fs = ofstream("test/print_stream_directly.txt");
    fs.sync_with_stdio(false);
    for(auto i = 0; i < num_values; i ++) {
        fs << int_data[i] << " " << flt_data[i] << " ";
        if(i && i % values_per_line == 0) fs << "\n";
    }
    fs.flush();
    fs.close();
    cout << "print_stream_directly: " << timer.elapsedf() << "\n";
}

void parse_file_directly() {

}
void parse_stream_directly() {
    
}

int main(int argc, const char** argv) {
    gen_data();
    print_file_directly();
    print_stream_directly();
    parse_file_directly();
    parse_stream_directly();
}