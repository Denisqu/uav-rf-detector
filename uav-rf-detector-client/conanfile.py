from conan import ConanFile
from conan.tools.cmake import cmake_layout

class ClientRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def configure(self):
        self.options["qt"].qtdeclarative = True
        self.options["qt"].qtnetworkauth = False
        self.options["qt"].openssl = False
        self.options["qt"].qttools = True

    def requirements(self):
        self.requires("boost/1.85.0")
        self.requires("qt/6.5.3")

    def layout(self):
        cmake_layout(self)