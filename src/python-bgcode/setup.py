"""setup.py for python-bgcode."""
from setuptools import setup, Extension

from distutils.sysconfig import get_config_vars
# from subprocess import Popen, PIPE, CalledProcessError
from sys import version_info

import bgcode

# removing cc1plus waring about Wstrict-prototypes
cfg_vars = get_config_vars()
for key, value in cfg_vars.items():
    if type(value) == str:
        cfg_vars[key] = value.replace("-Wstrict-prototypes", "")


"""
def pkgconfig(package, what):
    pp = Popen(('pkg-config', what, package), stdout=PIPE)
    rv = pp.wait()
    if not rv == 0:
        raise CalledProcessError(
            rv, "pkg-config %s %s" % (what, package))
    output = pp.stdout.readline().strip()
    if isinstance(output, bytes):
        output = output.decode()    # python3 supported ;-)
    return output.split()


extra_compile_args = pkgconfig("libmapmath",  '--cflags') +\
    ["-std=c++17"]
extra_link_args = pkgconfig("libmapmath",  '--libs') +\
    ["-lboost_python-py%d%d" % (version_info[0], version_info[1])]
"""

extra_compile_args = ["-I/usr/local/include/LibBGCode", "-std=c++17"]
extra_link_args = [f"-lboost_python{version_info[0]}{version_info[1]}"]


setup(
    name="python-gcode",
    version=bgcode.__version__,
    author=bgcode.__author_name__,
    author_email=bgcode.__author_email__,
    maintainer=bgcode.__author_name__,
    maintainer_email=bgcode.__author_email__,
    description="libbgcode python wrapper",
    packages=['bgcode'],
    ext_modules=[
        Extension('bgcode.core', ['core.cc'],
                  extra_compile_args=extra_compile_args,
                  extra_link_args=["-lbgcode_core"] + extra_link_args),
        #Extension('bgcode.binarize', ['binarize.cc'],
        #          extra_compile_args=extra_compile_args,
        #          extra_link_args=["-lbgcode_binarize"] + extra_link_args),
        #Extension('bgcode.convert', ['convert.cc'],
        #          extra_compile_args=extra_compile_args,
        #          extra_link_args=["-lbgcode_convert"] + extra_link_args),
    ],
    test_suite='tests',
    tests_require=['pytest']
)
