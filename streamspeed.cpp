#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

using namespace std;

const int values_per_line = 8;
const int num_values = 8 * 4096;

vector<int> int_data;
vector<float> flt_data;

void gen_data() {
    int_data.resize(num_values);
    flt_data.resize(num_values);
    for(auto& v : int_data) v = rand();
    for(auto& v : flt_data) v = rand() / (float)RAND_MAX;
}

int main(int argc, const char** argv) {
    gen_data();
    
}