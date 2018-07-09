# Pytest configuration file: will be run before tests to define fixtures

import pytest
import subprocess
import uuid
from tests.mocks.mock_subprocess import MockSubprocess


@pytest.fixture(scope='function')
def bridge_subprocess(monkeypatch):
    """ Mock the subprocess.Popen() function to run a class representing the bable bridge instead. """
    mocked_subprocess = MockSubprocess()

    def on_popen(cmd, *args, **kwargs):
        mocked_subprocess.start()
        return mocked_subprocess

    monkeypatch.setattr(subprocess, 'Popen', on_popen)

    return mocked_subprocess


@pytest.fixture(scope='function')
def mock_uuid(monkeypatch, request):
    """ Mock the uuid.uuid4() function to return a known and constant value (given as parameter) """
    value = request.param
    monkeypatch.setattr(uuid, 'uuid4', lambda: value)

    return value
