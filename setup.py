from distutils.core import setup, Extension

module1 = Extension('demo',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = [],
                    libraries = ["python39"],
                    library_dirs = [],
                    sources = ['src/c_db_module.c'])

setup (name = 'PackageName',
       version = '1.0',
       description = 'This is a demo package',
       author = 'Pirate Edward',
       author_email = 'pirateedward1999@gmail.com',
       url = '',
       long_description = '''
This is really just a demo package.
''',
       ext_modules = [module1])
