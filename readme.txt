b2 install toolset=msvc-14.1 address-model=64 variant=debug,release link=shared,static runtime-link=shared,static threading=multi
glslc shader.vert -o vert.spv
Visual Studio 15 2017 Win64
conan install -s build_type=Debug <path> --build=missing
-DCMAKE_BUILD_TYPE=Release