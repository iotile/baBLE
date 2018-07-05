import re
from bable_interface.scripts import bable_script


def test_help(capsys):
    result = bable_script.main([])
    assert result == 0

    out, _ = capsys.readouterr()
    assert out.startswith('usage')


def test_version(capsys):
    result = bable_script.main(['--version'])
    assert result == 0

    out, _ = capsys.readouterr()
    assert re.match(r"^\d\.\d\.\d(\.dev0)?$", out)


def test_set_cap():
    result = bable_script.main(['--set-cap', 'does_not_exist'])
    assert result == 1
