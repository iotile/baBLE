import os
import sys
from setuptools import setup, find_packages

# Make sure the system has the right Python version.
if sys.version_info < (2, 7):
    print("baBLE requires Python 2.7 or newer.")
    sys.exit(1)

# Check the OS
if sys.platform != 'linux' and sys.platform != 'linux2':
    print("baBLE only works on Linux for now.")
    sys.exit(1)

version = os.getenv("VERSION", "0.0.0.dev0")

with open('README.md', 'r') as readme_file:
    long_description = readme_file.read()

setup(
    name="bable-interface",
    install_requires=[
        'trollius;python_version<"3.4"',
        'future;python_version<"3.0"',
        'flatbuffers==1.9'
    ],
    packages=find_packages(exclude=('test',)),
    include_package_data=True,
    entry_points={
        'console_scripts': [
            'bable = bable_interface.scripts.bable_script:main'
        ]
    },
    version=version,
    license="MIT",
    author="Arch",
    author_email="info@arch-iot.com",
    description="Python interface for BaBLE",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/iotile/baBLE/interfaces/python",
    keywords=["bable", "arch", "bridge", "BLE", "Bluetooth", "cross-platform"],
    classifiers=(
        "Programming Language :: Python",
        "Programming Language :: Python :: 2",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Development Status :: 3 - Alpha",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    )
)
