# CppTest: A Collection of C++ tests for low-level implementation

- `valuesemantic.cpp` compares both speed and peak memory of using purely value
  semantic and purely reference semantic for large objects. It simulates
  allocaitons in 3D scenes for path tracing.

  ```
     mode    shapes instances  vertices      time      mem1      mem2     check
    value      5000      5000     50000  00:00:04      8617      9873   1.5e+09
   unique      5000      5000     50000  00:00:04      7662      7563   1.5e+09
   shared      5000      5000     50000  00:00:04      7284      7937   1.5e+09
      raw      5000      5000     50000  00:00:04      7536      7826   1.5e+09
    value      5000     25000     50000  00:00:05      8155      8576   7.5e+09
   unique      5000     25000     50000  00:00:05      7682      7681   7.5e+09
   shared      5000     25000     50000  00:00:05      7281      7689   7.5e+09
      raw      5000     25000     50000  00:00:05      7418      7681   7.5e+09
    value     15000     15000     50000  00:00:13     24744     26368   4.5e+09
   unique     15000     15000     50000  00:00:13     24668     25729   4.5e+09
   shared     15000     15000     50000  00:00:13     24688     25473   4.5e+09
      raw     15000     15000     50000  00:00:12     24437     25472   4.5e+09
    value     15000     75000     50000  00:00:16     25211     26122  2.25e+10
   unique     15000     75000     50000  00:00:16     24296     25477  2.25e+10
   shared     15000     75000     50000  00:00:16     24630     25479  2.25e+10
      raw     15000     75000     50000  00:00:15     24550     25475  2.25e+10
  ```

- `streamspeed.cpp` compares the speed of C `FILE` and C++ `fstream`.
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
