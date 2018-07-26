import re
from bable_interface.scripts import bable_script


def test_help(capsys):
    """ Test if the bable script returns the help if no option is given. """
    result = bable_script.main([])
    assert result == 0

    # capsys is a fixture provided by pytest to read stdout, stderr
    out, _ = capsys.readouterr()
    assert out.startswith('usage')


def test_version(capsys):
    """ Test if version is displayed with the '--version' option. """
    result = bable_script.main(['--version'])
    assert result == 0

    out, _ = capsys.readouterr()
    assert re.match(r"^\d\.\d\.\d(\.dev0)?$", out)


def test_set_cap(capsys):
    """ Test if bable script returns error if called with wrong path when setting capabilities. """
    result = bable_script.main(['--set-cap', 'does_not_exist'])
    assert result == 1

    out, _ = capsys.readouterr()
    assert out != ""
