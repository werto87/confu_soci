from conan import ConanFile
from conan.tools.cmake import CMakeToolchain

class Project(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators =  "CMakeDeps"


    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False #workaround because this leads to useless options in cmake-tools configure
        tc.generate()

    def configure(self):
        self.options["soci"].with_sqlite3 = True
        self.options["soci"].with_boost = True
        self.options["catch2"].with_main = True
        self.options["catch2"].with_benchmark = True


    def requirements(self):
        self.requires("soci/4.0.3@modern-durak")
        self.requires("catch2/2.13.7")
        self.requires("magic_enum/[>=0.9.5 <10]")
        self.requires("boost/1.85.0",force=True)
        self.requires("sqlite3/3.44.2")
