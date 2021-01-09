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
    name='nba_queries',
    version='0.1',
    url='https://github.com/David-Durst/nba_queries',
    license='MIT',
    maintainer='David Durst',
    maintainer_email='davidbdurst@gmail.com',
    description='Implementations of queries on NBA data using cython',
    install_requires=[
        'Cython'
    ],
    python_requires='>=3.8',
    ext_modules=cythonize(extensions, language_level=3, build_dir="build"),
    zip_safe=False,
)
