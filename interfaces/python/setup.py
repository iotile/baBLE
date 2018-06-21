from setuptools import setup, find_packages
import sys
import version

# Make sure the system has the right Python version.
if sys.version_info < (2, 7):
    print("baBLE requires Python 2.7 or newer.")
    sys.exit(1)

# Check the OS
if sys.platform == 'linux':
    # TODO: add possibility to use either the precompiled executable in the wheel or the user compiled (in PATH)
    pass
else:
    print("baBLE only works on Linux for now.")
    sys.exit(1)

with open("README.md", "r") as fh:
    long_description = fh.read()

setup(
    name="bable_interface",
    install_requires=[
        'trollius;python_version<"3.4"',
        'numpy',
        'flatbuffers==1.9'
    ],
    packages=find_packages(exclude=("test",)),
    include_package_data=True,
    version=version.version,
    license="MIT",
    author="Arch",
    author_email="info@arch-iot.com",
    description="Python interface for BaBLE",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/iotile/baBLE",
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
