from conans import ConanFile, CMake


class McapShrinkConan(ConanFile):
    name = "mcapshrink"
    version = "0.0.1"
    url = "https://github.com/jhurliman/mcapshrink"
    homepage = "https://github.com/jhurliman/mcapshrink"
    description = "Offline compression and decompression of MCAP files"
    license = "MIT"
    topics = ("mcap", "compression", "robotics")
    settings = "os", "compiler", "build_type", "arch"
    requires = "mcap/0.1.1", "draco/1.5.2", "libpng/1.6.37", "libwebp/1.2.2"
    build_requires = "catch2/2.13.8", "benchmark/1.6.1"
    generators = "cmake"

    def source(self):
        self.run("git clone https://github.com/jhurliman/mcapshrink.git")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="mcapshrink")
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.configure(source_folder=self._source_subfolder)
        cmake.install()

    def package_info(self):
        self.cpp_info.names["cmake_find_package"] = "mcapshrink"
        self.cpp_info.names["pkg_config"] = "mcapshrink"
