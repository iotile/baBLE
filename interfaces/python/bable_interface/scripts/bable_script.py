import argparse
import os
import sys
from pkg_resources import get_distribution


def main(argv=None):
    if argv is None:
        argv = sys.argv[1:]

    default_path = os.path.realpath(os.path.join(os.path.dirname(__file__), '..', 'bin', 'bable-bridge-linux'))

    parser = argparse.ArgumentParser(prog='bable')
    parser.add_argument('-v', '--version', action='store_true', help="display installed version")
    parser.add_argument('-s', '--set-cap', nargs='?', const=default_path, help="set the executable capability to use baBLE without sudo", metavar='PATH')

    if len(argv) == 0:
        parser.print_help()
        return 0

    args = parser.parse_args(argv)

    if args.version:
        if __package__ is not None:
            package_name = __package__.split('.')[0]
            package = get_distribution(package_name)
            version = package.version
        else:
            version = None

        print(version)
        return 0

    if args.set_cap:
        if not os.path.exists(args.set_cap):
            print("Wrong path given to --set-cap: file doesn't exist")
            return 1

        command = "setcap cap_net_raw,cap_net_admin+eip {}".format(args.set_cap)

        if os.geteuid() != 0:
            command = "sudo {}".format(command)

        print("Giving the capabilities to access the Bluetooth stack to the baBLE bridge binary...\n"
              "Running the following command (need sudo): \n\t{}".format(command))

        result = os.system(command)
        if result != 0:
            print("Error while setting capabilities.")
            return 1

        print("Capabilities set with success. You can now run baBLE without sudo.")

    return 0
