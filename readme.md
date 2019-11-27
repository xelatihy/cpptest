# CppTest: A Collection of C++ tests for low-level implementation

- `valuesemantic.cpp` compares both speed and peak memory of using purely value
  semantic and purely reference semantic for large objects. It simulates
  allocaitons in 3D scenes for path tracing.

  ```
     mode    shapes instances  vertices      time      mem1      mem2     check
    value      5000      5000     50000  00:00:04      8617     14168   1.5e+09
   unique      5000      5000     50000  00:00:03      8625     14059   1.5e+09
   shared      5000      5000     50000  00:00:03      8662     14067   1.5e+09
      raw      5000      5000     50000  00:00:03      8719     14203   1.5e+09
    value      5000     25000     50000  00:00:04      8889     14587   7.5e+09
   unique      5000     25000     50000  00:00:04      8854     14332   7.5e+09
   shared      5000     25000     50000  00:00:04      8903     14334   7.5e+09
      raw      5000     25000     50000  00:00:04      8801     14333   7.5e+09
    value     10000     10000     50000  00:00:07     17490     23164     3e+09
   unique     10000     10000     50000  00:00:07     17463     23164     3e+09
   shared     10000     10000     50000  00:00:07     17509     23420     3e+09
      raw     10000     10000     50000  00:00:07     17523     23292     3e+09
    value     10000     50000     50000  00:00:09     17502     23164   1.5e+10
   unique     10000     50000     50000  00:00:10     17565     23423   1.5e+10
   shared     10000     50000     50000  00:00:09     17500     23168   1.5e+10
      raw     10000     50000     50000  00:00:09     17471     23167   1.5e+10
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
