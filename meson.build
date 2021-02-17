project(
  'bench',
  'cpp',
  default_options: [
    'buildtype=debugoptimized',
    'warning_level=3',
    'werror=true',
    'cpp_std=c++20',
  ]
)

threads = dependency('threads')

executable(
  'bench',
  [
    'bench.cc',
    'common/src/tcp.cc',
    'common/src/timer.cc',
  ],
  include_directories: [
    'common/include',
  ],
  dependencies: [
    threads,
  ],
)
