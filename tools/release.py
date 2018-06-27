import argparse
import glob
import os
import setuptools.sandbox
import sys
from twine.commands.upload import upload as twine_upload

root_folder = os.path.realpath(os.path.join(os.path.dirname(__file__), '..'))
src_folder = os.path.join(root_folder, 'interfaces', 'python')


def check_version(expected_version):
    """ Make sure the package version in setuptools matches what we expect it to be """

    with open(os.path.join(root_folder, 'VERSION'), 'r') as version_file:
        version = version_file.read().strip()

    if version != expected_version:
        raise EnvironmentError("Version mismatch during release, expected={}, found={}"
                               .format(expected_version, version))


def build(system, platform):
    """Create a wheel"""

    init_dir = os.getcwd()
    os.chdir(src_folder)

    try:
        setuptools.sandbox.run_setup(
            'setup.py',
            ['-q', 'clean', 'bdist_wheel', '--plat-name', '{}_{}'.format(system, platform)]
        )
    finally:
        os.chdir(init_dir)

def upload():
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

    distpath = os.path.realpath(os.path.join(src_folder, 'dist', '*'))
    packages = glob.glob(distpath)

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
    parser.add_argument('--arch', action='store', help="architecture to release")
    parser.add_argument('version', action='store', help="version to release")

    args = parser.parse_args(sys.argv[1:])

    version = args.version
    check_version(version)
    release_notes = get_release_notes(version)

    if args.arch:
        build('linux', args.arch)
    else:
        archs = os.listdir('./tmp')
        if len(archs) == 0:
            raise Exception("No binaries found in /tmp")

        for arch in archs:
            build('linux', arch)

    if args.dry_run:
        print("Dry-run release\nVersion: {}".format(version))
        print("Release Notes:\n" + release_notes)
    else:
        upload()
        print('bable_interface version {} uploaded to pypi')


if __name__ == '__main__':
    main()
