from conan import ConanFile
from conan.tools.cmake import cmake_layout

class ClientRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def configure(self):
        self.options["qt"].qtdeclarative = True
        self.options["qt"].gui           = True
        self.options["qt"].openssl       = False
        self.options["qt"].qttools       = True
        self.options["qt"].qtdoc         = True
        self.options["qt"].qtsvg         = True
        self.options["qt"].shared        = True
        self.options["qt"].qtshadertools = True
        self.options["qt"].qtwebsockets  = True

    def requirements(self):
        self.requires("boost/1.83.0")
        self.requires("qt/6.6.3")
        self.requires("grpc/1.54.3")
        self.requires("asio-grpc/2.9.2")
        self.requires("spdlog/1.14.1")

    def layout(self):
        cmake_layout(self)