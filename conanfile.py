from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout

class LibBitmapAllocatorConan(ConanFile):
    name = "bitmap_allocator"
    version = "1.0"
    license = "MIT"
    topics = ("example", "c", "library")
    settings = "os", "compiler", "build_type"
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "test/*"
    
    requires = "test_utils/1.0"
    
    scm = {
        "type": "git",
        "url": "https://github.com/nick-a-schneider/bitmap_allocator.git",
        "revision": "conan"
    }

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        
    def generate(self):
        tc = CMakeToolchain(self)
        # Force generator:
        tc.generator = "Unix Makefiles"
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def package_info(self):
        self.cpp_info.libs = ["bitmap_allocator"]
        self.cpp_info.includedirs = ["include"]
        
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
