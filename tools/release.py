import argparse
import glob
import os
import setuptools.sandbox
import sys
import shutil
from twine.commands.upload import upload as twine_upload

root_folder = os.path.realpath(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def check_version(expected_version):
    """ Make sure the package version in setuptools matches what we expect it to be """

    with open(os.path.join(root_folder, 'VERSION'), 'r') as version_file:
        version = version_file.read().strip()

    if version != expected_version:
        raise EnvironmentError("Version mismatch during release, expected={}, found={}"
                               .format(expected_version, version))


def build_wheel(platform):
    """Create a wheel"""

    if platform in ['x86_64', 'i686']:
        system = 'manylinux1'
    else:
        system = 'linux'

    setuptools.sandbox.run_setup(
        'setup.py',
        ['-q', 'clean', '--all', 'bdist_wheel', '--plat-name', '{}_{}'.format(system, platform)]
    )


def build_sdist():
    """Create an sdist package"""

    setuptools.sandbox.run_setup('setup.py', ['-q', 'clean', '--all', 'sdist'])


def upload(dist_dir):
    """Upload a given component to pypi
    The pypi username and password must either be specified in a ~/.pypirc file
    or in environment variables PYPI_USER and PYPI_PASS
    """

    if 'PYPI_USER' in os.environ and 'PYPI_PASS' in os.environ:
        pypi_user = os.environ['PYPI_USER']
        pypi_pass = os.environ['PYPI_PASS']
    else:
        pypi_user = None
        pypi_pass = None
        print("No PYPI user information in environment")

    packages = glob.glob(dist_dir)

    # Invoke upload this way since subprocess call of twine cli has cross platform issues
    twine_upload(packages, 'pypi', False, None, pypi_user, pypi_pass, None, None, '~/.pypirc', False, None, None, None)


def get_release_notes(version):
    notes_path = os.path.join(root_folder, 'RELEASE.md')

    try:
        with open(notes_path, "r") as f:
            lines = f.readlines()
    except IOError:
        print("ERROR: Could not find release notes file RELEASE.md")
        sys.exit(1)

    release_lines = {y[2:].strip(): x for x, y in enumerate(lines) if y.startswith('##')}

    if version not in release_lines:
        print("ERROR: Could not find release notes for current release version")
        sys.exit(1)

    start_line = release_lines[version]
    past_releases = [x for x in release_lines.values() if x > start_line]

    if len(past_releases) == 0:
        release_string = "".join(lines[start_line:])
    else:
        release_string = "".join(lines[start_line:min(past_releases)])

    if len(release_string) == 0:
        print("ERROR: Empty release notes for current release version")
        sys.exit(1)

    return release_string


def main():
    parser = argparse.ArgumentParser(prog='release.py')
    parser.add_argument('--dry-run', action='store_true', help="check release without uploading it")
    parser.add_argument('--build_sdist', action='store_true', help="indicate to build a source package (.tar.gz)")
    parser.add_argument('version', action='store', help="version to release")
    parser.add_argument('src_dir', action='store', help="path to python source directory")
    parser.add_argument('binaries_dir', action='store', help="path to c++ binaries directory")

    args = parser.parse_args(sys.argv[1:])

    version = args.version
    src_folder = os.path.realpath(args.src_dir)
    binaries_dir = os.path.realpath(args.binaries_dir)

    os.chdir(src_folder)
    os.environ["VERSION"] = version

    check_version(version)
    release_notes = get_release_notes(version)

    archs = os.listdir(binaries_dir)
    if len(archs) == 0:
        raise Exception("No binaries found in {}".format(binaries_dir))

    for arch in archs:
        shutil.copy(
            os.path.join(binaries_dir, arch, 'bable-bridge-linux'),
            os.path.join(src_folder, 'bable_interface', 'bin', 'bable-bridge-linux')
        )
        build_wheel(arch)

    if args.build_sdist:
        build_sdist()

    if args.dry_run:
        print("Dry-run release\nVersion: {}".format(version))
        print("Release Notes:\n" + release_notes)
    else:
        upload(os.path.join(src_folder, 'dist', '*'))
        print('bable-interface version {} uploaded to pypi'.format(version))


if __name__ == '__main__':
    main()
