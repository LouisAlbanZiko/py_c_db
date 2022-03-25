from distutils.core import setup, Extension

def main():
    setup(name="py_c_db",
          version="1.0.0",
          description="Python interface for c_db library",
          author="peTheProgrammer",
          author_email="peTheProgrammer@gmail.com",
          ext_modules=[
            Extension(
                name="py_c_db",
                sources=[
                    "src/py_c_db_database.c",
                    "src/py_c_db_module.c",
                    "src/py_c_db_table.c"
                ],
                include_dirs=[ "_vendor/", "src/" ],
                define_macros=[],
                undef_macros=[],
                library_dirs=[
                    "_vendor/_bin/c_db/Release",
                    "_vendor/_bin/c_core/Release",
                    "_vendor/_bin/c_file/Release",
                    "_vendor/_bin/c_log/Release"
                ],
                libraries=[
                    "python39",
                    "c_log",
                    "c_core",
                    "c_file",
                    "c_db"
                ]
            )
        ]
    )

if __name__ == "__main__":
    main()
