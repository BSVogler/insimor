from setuptools import setup, Extension
modul = Extension("insimou", sources=["main.cpp"])
setup(name= "PyInsimou",
    version="1.3",
    description="bla",
    ext_modules=[modul],
    extra_compile_args=['-std=c++17']
)
