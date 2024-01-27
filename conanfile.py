from conan import ConanFile


class Project(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def configure(self):
        self.options["soci"]. with_sqlite3=True
        self.options["soci"]. shared=False
        self.options["soci"]. with_boost=True
        self.options["boost"].header_only=True
        self.options["catch2"].with_main = True
        self.options["catch2"].with_benchmark = True

    def requirements(self):
        self.requires("soci/4.0.3")
        self.requires("catch2/2.13.7")
        self.requires("magic_enum/[>=0.9.5 <10]")
        self.requires("boost/1.83.0")

