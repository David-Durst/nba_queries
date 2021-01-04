from setuptools import Extension, setup
from Cython.Build import cythonize

extensions = [
    Extension("*", ["*.pyx"],
        include_dirs=[],
        libraries=[],
        library_dirs=[]),
]
setup(
    name='Hello world app',
    ext_modules=cythonize(extensions),
    zip_safe=False,
)
