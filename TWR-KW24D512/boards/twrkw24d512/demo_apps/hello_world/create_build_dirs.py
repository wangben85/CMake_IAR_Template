import os
import shlex
import subprocess
import contextlib
import collections


##  Classes  ##

class Command:
    cmd_helpers = collections.OrderedDict()

    def __init__(self, base_cmd, **config):
        self.config=config
        self.cmd_str = base_cmd

    @classmethod
    def register_cmd_helper(cls, key, cmd_helper):
        cls.cmd_helpers[key] = cmd_helper

    def getstr(self):
        for cmd, helper in Command.cmd_helpers.items():
            if cmd in self.config:
                config_value = self.config[cmd]
                self._addcmd(helper.getstr(config_value))
        return self.cmd_str

    def getarray(self):
        return shlex.split(self.getstr())

    def _addcmd(self, cmd_str):
        self.cmd_str += ' ' + cmd_str


class CommandHelper:
    def __init__(self):
        self.strings = {}

    def add_option(self, option, string):
        self.strings[option] = string

    def getstr(self, option):
        return self.strings[option]


class ListCommandHelper(CommandHelper):
    def __init__(self, join_char=''):
        super().__init__()
        self.empty_option = ''
        self.non_empty_option = ''
        self.join_char = join_char

    def add_empty_list_option(self, string):
        self.empty_option = string

    def add_nonempty_list_option(self, string):
        self.non_empty_option = string

    def getstr(self, option):
        if option:
            return self.non_empty_option.format(self.join_char.join(option))
        else:
            return self.empty_option.format(option)


##  Functions  ##

@contextlib.contextmanager
def working_directory(path):
    """A context manager which changes the working directory to the given
    path, and then changes it back to its previous value on exit.
    """
    prev_cwd = os.getcwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(prev_cwd)


def MakeDir(new_dir):
    if not os.path.exists(new_dir):
        print('**Creating directory "{}"'.format(new_dir))
        os.makedirs(new_dir)


def GetCmd(build_dir):
    if not os.path.exists('CMakeFiles'):
        cmd = cmds[build_dir].getstr()
        print('**Running initial config cmd ({}) on dir "{}"'.format(cmd, build_dir))
    else:
        cmd = 'cmake ..'
        print('**Refreshing config ({}) on dir "{}"'.format(cmd, build_dir))
    return cmd


def RunCmd(cmd):
    res = subprocess.run(shlex.split(cmd), stdout=subprocess.PIPE)
    return res.stdout.decode('utf-8')


##  Main  ##

if '__main__' == __name__:
    # Add generator helper
    helper = CommandHelper()
    helper.add_option('ninja', '-G "Ninja"')
    helper.add_option('msvc', '-G "Visual Studio 14 2015 Win64"')
    Command.register_cmd_helper('generator', helper)

    # Add build type helper
    helper = CommandHelper()
    helper.add_option('release','-DCMAKE_BUILD_TYPE=Release')
    helper.add_option('debug','-DCMAKE_BUILD_TYPE=Debug')
    Command.register_cmd_helper('build', helper)

    # Add toolchain helper
    helper = CommandHelper()
    helper.add_option('iar', '-DCMAKE_TOOLCHAIN_FILE=../toolchain_iar.cmake')
    helper.add_option('msvc', '')
    Command.register_cmd_helper('toolchain', helper)


    # Add CMake prefix path helper
    helper = ListCommandHelper()
    helper.add_empty_list_option('')
    helper.add_nonempty_list_option('-DCMAKE_PREFIX_PATH="{}"')
    Command.register_cmd_helper('paths', helper)

    cmds = {
        'debug'  :  Command('cmake ..', generator='ninja',  build='debug',      toolchain='iar',    unittests=False),
        'release':  Command('cmake ..', generator='ninja',  build='release',    toolchain='iar',    unittests=False),
    }

    for build_dir in cmds:
        MakeDir(build_dir)

        with working_directory(build_dir):
            cmd = GetCmd(build_dir)
            print(RunCmd(cmd))

