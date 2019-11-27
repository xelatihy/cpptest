# CppTest: A Collection of C++ tests for low-level implementation

- `valuesemantic.cpp` compares both speed and peak memory of using purely value
  semantic and purely reference semantic for large objects. It simulates
  allocaitons in 3D scenes for path tracing.

- `streamspeed.cpp` compares the speed of C `FILE` and C++ `iostreams`.
  Short conclusion is that C streams are just faster.
  Here are some timing results for a MacBook Pro with SSD and OSX 10.14.
  See the code to check what the functions do.

  ```
  print_data: 00:00:00.142
  write_data: 00:00:00.051
  print_file_directly: 00:00:00.128
  print_stream_directly: 00:00:00.219
  parse_file_directly: 00:00:00.162
  parse_stream_directly: 00:00:00.299
  parse_file_lines: 00:00:00.156
  parse_stream_lines: 00:00:00.352
  parse_file_fast: 00:00:00.071
  parse_stream_fast: 00:00:00.094
  parse_stream_fast1: 00:00:00.120
  read_file_directly: 00:00:00.052
  read_stream_directly: 00:00:00.014
  ```
