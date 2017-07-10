# @(#) $Id$
"""
Installation script for the GSI module
"""
#import ez_setup
#ez_setup.use_setuptools()

from distutils.core import setup, Extension
import ConfigParser

import os

here = os.path.realpath(os.path.dirname(__file__))
srcDir = os.path.join(here, "src")

config = ConfigParser.SafeConfigParser()
config.read(os.path.join(here, "setup.cfg"))

def findFiles(baseDir, validFileExts):
    files = []
    for t in os.walk(baseDir):
        for fileInDir in t[2]:
            for fext in validFileExts:
                fPos = len(fileInDir) - len(fext)
                if fileInDir.find(fext, fPos) == fPos:
                    files.append(os.path.join(baseDir, fileInDir))
    return files

def createExtension(extName):
    extDir = os.path.join(srcDir, extName.lower())
    cFiles = [os.path.join(srcDir, "util.c")] + findFiles(extDir, ".c")
    hFiles = [os.path.join(srcDir, "util.h")] + findFiles(extDir, ".h")
    extraArgs = {}
    if 'Extensions' in config.sections():
        for k in config.options('Extensions'):
            extraArgs[k] = [v.strip() for v in config.get('Extensions', k).split(" ") if v.strip()]
            for i in range(len(extraArgs[k])):
                if os.path.isfile(extraArgs[k][i]):
                    extraArgs[k][i] = os.path.realpath(extraArgs[k][i])
    return Extension("GSI.%s" % extName,
                     cFiles,
                     depends=hFiles,
                     libraries=['ssl', 'crypto'],
                     extra_compile_args=["-Wno-deprecated-declarations", "-std=c99"],
                     ** extraArgs
                    )

setup(
    name="GSI",
    version='0.6.5',
    author="Adrian Casajus",
    author_email="adria@ecm.ub.es",
    description="Python wrapper module around the OpenSSL library (including hack to accept GSI SSL proxies)",
    license="GPLv3",
    # See https://pypi.python.org/pypi?%3Aaction=list_classifiers
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Topic :: GSI to python :: Grid proxy certificates',

        # Pick your license as you wish (should match "license" above)
        'License :: OSI Approved :: GPLv3 License',

        # Specify the Python versions you support here. In particular, ensure
        # that you indicate whether you support Python 2, Python 3 or both.
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
    ],
    zip_safe=False,
    #install_requires = ["distribute>0.6", "pip"],
    py_modules=['GSI.__init__', 'GSI.tsafe', 'GSI.version'],
    ext_modules=[createExtension(extName) for extName in ("crypto", "rand", "SSL")]
)
