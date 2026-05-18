project('rl-map-loader', 'cpp', 'c',
  version: '0.3.0',
  default_options: [
    'cpp_std=c++23',
    'buildtype=release',
    'warning_level=3',
  ],
  license: 'MIT'
)

if host_machine.system() == 'windows'
  add_project_link_arguments(
    [
      '-static',
      '-static-libgcc',
      '-static-libstdc++',
    ],
    language: 'cpp'
  )
endif

if get_option('buildtype') == 'release'
  add_project_arguments('-flto', language: 'cpp')
endif

cmake = import('cmake')

glfw_opts = cmake.subproject_options()
glfw_opts.add_cmake_defines({
  'GLFW_BUILD_TESTS': 'OFF',
  'GLFW_BUILD_EXAMPLES': 'OFF',
  'GLFW_BUILD_DOCS': 'OFF',
  'BUILD_SHARED_LIBS': 'OFF',
})

glfw_proj = cmake.subproject('glfw', options: glfw_opts)
glfw_dep = glfw_proj.dependency('glfw')

cc = meson.get_compiler('cpp')
if host_machine.system() == 'windows'
  gl_dep = cc.find_library('opengl32')
else
  gl_dep = dependency('gl')
endif

inc_dirs = include_directories(
  'include',
  'third_party/imgui',
  'third_party/imgui/backends',
  'third_party/json/include',
)

sources = files(
  'src/main.cpp',
  'src/app.cpp',
  'src/config.cpp',
  'src/map_manager.cpp',
  'src/platform.cpp',

  'third_party/imgui/imgui.cpp',
  'third_party/imgui/imgui_demo.cpp',
  'third_party/imgui/imgui_draw.cpp',
  'third_party/imgui/imgui_widgets.cpp',
  'third_party/imgui/imgui_tables.cpp',
  'third_party/imgui/backends/imgui_impl_glfw.cpp',
  'third_party/imgui/backends/imgui_impl_opengl3.cpp',
)

link_args = []

if host_machine.system() != 'windows'
  link_args += ['-ldl']
endif

executable(
  'rl-map-loader',
  sources,
  include_directories: inc_dirs,
  dependencies: [gl_dep, glfw_dep],
  link_args: link_args,
  install: true,
  win_subsystem: 'windows',
)
