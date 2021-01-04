from setuptools import Extension, setup
from Cython.Build import cythonize

extensions = [
    Extension("*", ["*.pyx"],
        include_dirs=[],
        libraries=[],
        library_dirs=[],
        language="c++"),
]
setup(
    name='Hello world app',
    ext_modules=cythonize(extensions, language_level=3),
    zip_safe=False,
)
