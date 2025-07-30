import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class LibFooTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    requires = "bitmap_allocator/1.0"
    test_type = "explicit"

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        self.run(os.path.join(self.cpp.build.bindir, "test_bitmap_allocator"), env="conanrun")
