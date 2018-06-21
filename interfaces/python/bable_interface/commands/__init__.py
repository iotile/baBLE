import inspect
import sys
from .commands_manager import CommandsManager

if sys.version_info < (3, 4):
    from . import commands_py2 as commands_module
else:
    from . import commands_py3 as commands_module

commands = inspect.getmembers(commands_module, inspect.isfunction)
for command_name, command_fn in commands:
    setattr(CommandsManager, command_name, command_fn)
