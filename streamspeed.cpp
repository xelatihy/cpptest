#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <chrono>
#include <sstream>
#include <string_view>

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

const auto values_per_line = 4;
const auto num_lines = 131072;
const auto num_values = values_per_line * num_lines;
const auto repetitions = 1;

auto int_data = vector<int>(num_values);
auto flt_data = vector<float>(num_values);
auto int_check = vector<int>(num_values);
auto flt_check = vector<float>(num_values);

void gen_data() {
    int_data.resize(num_values);
    flt_data.resize(num_values);
    for(auto& v : int_data) v = rand();
    for(auto& v : flt_data) v = rand() / (float)RAND_MAX;
}

void print_data() {
    auto timer = ::timer{};
    auto fs = fopen("test/data.txt", "wt");
    for(auto i = 0; i < num_values; i ++) {
        fprintf(fs, "%d %g ", int_data[i], flt_data[i]);
        if(i && i % values_per_line == 0) fprintf(fs, "\n");
    }
    fflush(fs);
    fclose(fs);
    cout << "print_data: " << timer.elapsedf() << "\n";
}
void write_data() {
    auto timer = ::timer{};
    auto fs = fopen("test/data.bin", "wb");
    for(auto i = 0; i < num_values; i ++) {
        fwrite(&(int_data[i]), sizeof(int), 1, fs);
        fwrite(&(flt_data[i]), sizeof(float), 1, fs);
    }
    fflush(fs);
    fclose(fs);
    cout << "write_data: " << timer.elapsedf() << "\n";
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
    auto timer = ::timer{};
    auto fs = fopen("test/data.txt", "rt");
    for(auto i = 0; i < num_values; i ++) {
        fscanf(fs, "%d %g ", &(int_check[i]), &(flt_check[i]));
    }
    fclose(fs);
    cout << "parse_file_directly: " << timer.elapsedf() << "\n";
}
void parse_stream_directly() {
    auto timer = ::timer{};
    auto fs = ifstream("test/data.txt");
    for(auto i = 0; i < num_values; i ++) {
        fs >> int_check[i] >> flt_check[i];
    }
    fs.close();
    cout << "parse_stream_directly: " << timer.elapsedf() << "\n";    
}

void parse_file_lines() {
    auto timer = ::timer{};
    auto fs = fopen("test/data.txt", "rt");
    char line[4096];
    for(auto j = 0; j < num_lines; j ++) {
        fgets(line, sizeof(line), fs);
        auto scanner = line;
        for(auto i = 0; i < values_per_line; i ++) {
            auto idx = j * values_per_line + i;
            auto offset = 0;
            sscanf(scanner, "%d %g %n", &(int_check[idx]), &(flt_check[idx]), &offset);
            scanner += offset;
        }
    }
    fclose(fs);
    cout << "parse_file_lines: " << timer.elapsedf() << "\n";
}
void parse_stream_lines() {
    auto timer = ::timer{};
    auto fs = ifstream("test/data.txt");
    auto line = string{};
    auto scanner = stringstream{};
    for(auto j = 0; j < num_lines; j ++) {
        getline(fs, line);
        scanner.str(line);
        for(auto i = 0; i < values_per_line; i ++) {
            auto idx = j * values_per_line + i;
            scanner >> int_check[idx] >> flt_check[idx];
        }
    }
    fs.close();
    cout << "parse_stream_lines: " << timer.elapsedf() << "\n";    
}

void parse_file_fast() {
    auto timer = ::timer{};
    auto fs = fopen("test/data.txt", "rt");
    char line[4096];
    for(auto j = 0; j < num_lines; j ++) {
        fgets(line, sizeof(line), fs);
        auto scanner = line;
        for(auto i = 0; i < values_per_line; i ++) {
            auto idx = j * values_per_line + i;
            auto offset = (char*)nullptr;
            int_check[idx] = strtol(scanner, &offset, 10);
            scanner = offset;
            flt_check[idx] = strtof(scanner, &offset);
            scanner = offset;
        }
    }
    fclose(fs);
    cout << "parse_file_fast: " << timer.elapsedf() << "\n";
}
void parse_stream_fast() {
    // auto buffer = vector<char>(1048576);
    auto timer = ::timer{};
    auto fs = ifstream("test/data.txt");
    // fs.rdbuf()->pubsetbuf(buffer.data(), buffer.size());
    char line[4096];
    for(auto j = 0; j < num_lines; j ++) {
        fs.getline(line, sizeof(line));
        auto scanner = line;
        for(auto i = 0; i < values_per_line; i ++) {
            auto idx = j * values_per_line + i;
            auto offset = (char*)nullptr;
            int_check[idx] = strtol(scanner, &offset, 10);
            scanner = offset;
            flt_check[idx] = strtof(scanner, &offset);
            scanner = offset;
        }
    }
    fs.close();
    cout << "parse_stream_fast: " << timer.elapsedf() << "\n";    
}
inline string_view& operator>>(string_view& str, int& value) {
    auto offset = (char*)nullptr;
    value = strtol(str.data(), &offset, 10);
    str.remove_prefix(offset - str.data());
    return str;
}
inline string_view& operator>>(string_view& str, float& value) {
    auto offset = (char*)nullptr;
    value = strtof(str.data(), &offset);
    str.remove_prefix(offset - str.data());
    return str;
}
void parse_stream_fast1() {
    auto timer = ::timer{};
    auto fs = ifstream("test/data.txt");
    auto line = ""s;
    for(auto j = 0; j < num_lines; j ++) {
        getline(fs, line);
        auto scanner = string_view{line};
        for(auto i = 0; i < values_per_line; i ++) {
            auto idx = j * values_per_line + i;
            scanner >> int_check[idx] >> flt_check[idx];
        }
    }
    fs.close();
    cout << "parse_stream_fast1: " << timer.elapsedf() << "\n";    
}

void read_file_directly() {
    auto timer = ::timer{};
    auto fs = fopen("test/data.bin", "rb");
    for(auto i = 0; i < num_values; i ++) {
        fread(&(int_check[i]), sizeof(int), 1, fs);
        fread(&(flt_check[i]), sizeof(float), 1, fs);
    }
    fclose(fs);
    cout << "read_file_directly: " << timer.elapsedf() << "\n";
}
void read_stream_directly() {
    auto timer = ::timer{};
    auto fs = ifstream("test/data.bin", ios::binary);
    for(auto i = 0; i < num_values; i ++) {
        fs.read((char*)&(int_check[i]), sizeof(int));
        fs.read((char*)&(flt_check[i]), sizeof(float));
    }
    fs.close();
    cout << "read_stream_directly: " << timer.elapsedf() << "\n";    
}

int main(int argc, const char** argv) {
    std::ios_base::sync_with_stdio(false);
    gen_data();
    print_data();
    write_data();
    print_file_directly();
    print_stream_directly();
    for(auto i = 0; i < repetitions; i ++) parse_file_directly();
    for(auto i = 0; i < repetitions; i ++) parse_stream_directly();
    for(auto i = 0; i < repetitions; i ++) parse_file_lines();
    for(auto i = 0; i < repetitions; i ++) parse_stream_lines();
    for(auto i = 0; i < repetitions; i ++) parse_file_fast();
    for(auto i = 0; i < repetitions; i ++) parse_stream_fast();
    for(auto i = 0; i < repetitions; i ++) parse_stream_fast1();
    for(auto i = 0; i < repetitions; i ++) read_file_directly();
    for(auto i = 0; i < repetitions; i ++) read_stream_directly();
}