#!/usr/bin/env python

#~ Copyright 2013, 2014 Mark Geelen.
#~ Copyright 2014, 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from popen import Popen, MemoryExceededError, TimeExceededError
from subprocess import  PIPE
import os.path
import re
from text_utility import read_text, write_text

def is_list_of(l, types):
    if not isinstance(l, list):
        return False
    for x in l:
        if not isinstance(x, types):
            return False
    return True

class Node:
    def __init__(self, label, type, value):
        self.label = label
        self.type = type
        self.value = value
        return

    def __str__(self):
        return 'Node(label = {0}, type = {1}, value = {2})'.format(self.label, self.type, self.value)

    def filename(self):
        return '{}.{}'.format(self.label, self.type.lower())

class Tool(object):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert is_list_of(input_nodes, Node)
        assert is_list_of(output_nodes, Node)
        import platform
        self.label = label
        self.name = name
        self.toolpath = toolpath
        self.input_nodes = input_nodes
        self.output_nodes = output_nodes
        self.args = args
        self.executed = False
        if platform.system() == 'Windows':
            # Don't display the Windows GPF dialog if the invoked program dies.
            # See comp.os.ms-windows.programmer.win32
            # How to suppress crash notification dialog?, Raymond Chen Jan 14,2004 -
            import ctypes
            SEM_NOGPFAULTERRORBOX = 0x0002 # From MSDN
            ctypes.windll.kernel32.SetErrorMode(SEM_NOGPFAULTERRORBOX);
            self.subprocess_flags = 0x8000000 #win32con.CREATE_NO_WINDOW?
        else:
            self.subprocess_flags = 0

    def can_execute(self):
        if self.executed:
            return False
        for i in self.input_nodes:
            if i.value == None:
                return False
        return True

    # Raises an exception if the execution was aborted or produced an error
    def check_execution(self, process, timeout, memlimit):
        if process.maxVirtualMem > memlimit:
            raise MemoryExceededError(process.maxVirtualMem)
        if process.userTime > timeout:
            raise TimeExceededError(process.userTime)
        if self.stderr and 'error' in self.stderr:
            raise RuntimeError('Tool {} failed: {}'.format(self.name, self.stderr))

    def arguments(self, runpath = None):
        if not runpath:
            runpath = os.getcwd()
        args = [os.path.join(runpath, node.filename()) for node in self.input_nodes]
        args = args + [os.path.join(runpath, node.filename()) for node in self.output_nodes if node.type != 'Bool']
        return args

    def assign_outputs(self):
        for node in self.output_nodes:
            node.value = self.stdout
            if node.value == '':
                node.value = self.stderr
            if node.value == '':
                node.value = 'executed'

    def command(self, runpath):
        args = self.arguments(runpath)
        name = os.path.join(self.toolpath, self.name)
        return ' '.join([name] + args + self.args)

    def execute(self, timeout, memlimit, verbose):
        args = self.arguments()
        name = os.path.join(self.toolpath, self.name)
        if verbose:
            print 'Executing ' + ' '.join([name] + args + self.args)
        process = Popen([name] + args + self.args, stdout=PIPE, stdin=PIPE, stderr=PIPE, creationflags=self.subprocess_flags, maxVirtLimit=memlimit, usrTimeLimit=timeout)

        input = None
        self.stdout, self.stderr = process.communicate(input)
        self.assign_outputs()
        self.executed = True
        self.userTime = process.userTime
        self.maxVirtualMem = process.maxVirtualMem
        self.check_execution(process, timeout, memlimit)

    def __str__(self):
        import StringIO
        out = StringIO.StringIO()
        out.write('label    = ' + str(self.label)    + '\n')
        out.write('name     = ' + str(self.name)     + '\n')
        out.write('input    = [{0}]\n'.format(', '.join([str(x) for x in self.input_nodes])))
        out.write('output   = [{0}]\n'.format(', '.join([str(x) for x in self.output_nodes])))
        out.write('args     = ' + str(self.args)     + '\n')
        out.write('stderr   = ' + str(self.stderr)    + '\n')
        out.write('executed = ' + str(self.executed) + '\n')
        return out.getvalue()

class PrintTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert len(input_nodes) == 1
        assert len(output_nodes) == 1
        super(PrintTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def assign_outputs(self):
        text = read_text(self.output_nodes[0].filename())
        self.output_nodes[0].value = text

class Pbes2BoolTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert len(input_nodes) == 1
        assert len(output_nodes) == 1
        super(Pbes2BoolTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def assign_outputs(self):
        text = self.stdout.strip()
        if text.endswith('true'):
            value = True
        elif text.endswith('false'):
            value = False
        else:
            value = None
        self.output_nodes[0].value = value
        write_text(self.output_nodes[0].filename(), str(value))

class PbesPgSolveTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert len(input_nodes) == 1
        assert len(output_nodes) == 1
        super(PbesPgSolveTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def assign_outputs(self):
        text = self.stdout.strip()
        if text.endswith('true'):
            value = True
        elif text.endswith('false'):
            value = False
        else:
            value = None
        self.output_nodes[0].value = value
        write_text(self.output_nodes[0].filename(), str(value))

class BesSolveTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert len(input_nodes) == 1
        assert len(output_nodes) == 1
        super(BesSolveTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def assign_outputs(self):
        text = self.stdout.strip()
        if text.endswith('true'):
            value = True
        elif text.endswith('false'):
            value = False
        else:
            value = None
        self.output_nodes[0].value = value
        write_text(self.output_nodes[0].filename(), str(value))

class LtsInfoTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert len(input_nodes) == 1
        assert len(output_nodes) == 1
        super(LtsInfoTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def arguments(self, runpath = None):
        if not runpath:
            runpath = os.getcwd()
        return [os.path.join(runpath, node.filename()) for node in self.input_nodes]

    def assign_outputs(self):
        node = self.output_nodes[0]
        result = {}
        lines = self.stdout.splitlines()
        m = re.search('Number of states: (\d+)', lines[0])
        result['states'] = int(m.group(1))
        node.value = result
        write_text(node.filename(), str(result))

class Lps2PbesTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert len(input_nodes) == 2
        assert len(output_nodes) == 1
        super(Lps2PbesTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def arguments(self, runpath = None):
        if not runpath:
            runpath = os.getcwd()
        return [os.path.join(runpath, self.input_nodes[0].filename()),
                '-f' + os.path.join(runpath, self.input_nodes[1].filename()),
                os.path.join(runpath, self.output_nodes[0].filename())
               ]

class Lps2LtsTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert len(input_nodes) == 1
        assert len(output_nodes) in [1, 2]
        super(Lps2LtsTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def assign_outputs(self):
        if '-D' in self.args and len(self.output_nodes) > 1:
            value = { 'deadlock': re.search('deadlock-detect: deadlock found', self.stderr) != None }
            self.output_nodes[1].value = value
            write_text(self.output_nodes[1].filename(), str(value))
        self.output_nodes[0].value = 'executed'

    def arguments(self, runpath = None):
        if not runpath:
            runpath = os.getcwd()
        return [os.path.join(runpath, self.input_nodes[0].filename()), os.path.join(runpath, self.output_nodes[0].filename())]

class LpsConfcheckTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert len(input_nodes) == 1
        assert len(output_nodes) in [1, 2]
        super(LpsConfcheckTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def assign_outputs(self):
        super(LpsConfcheckTool, self).assign_outputs()
        if len(self.output_nodes) > 1:
            text = self.stderr
            value = {}
            m = re.search(r'(\d+) of (\d+) tau summands were found to be confluent', text)
            if m:
                value['confluent_tau_summands'] = (int(m.group(1)), int(m.group(2)))
            self.output_nodes[1].value = value
            write_text(self.output_nodes[1].filename(), str(value))

    def arguments(self, runpath = None):
        if not runpath:
            runpath = os.getcwd()
        return [os.path.join(runpath, self.input_nodes[0].filename()), os.path.join(runpath, self.output_nodes[0].filename())]

class LtsCompareTool(Tool):
    def __init__(self, label, name, toolpath, input_nodes, output_nodes, args):
        assert len(input_nodes) == 2
        assert len(output_nodes) == 1
        super(LtsCompareTool, self).__init__(label, name, toolpath, input_nodes, output_nodes, args)

    def assign_outputs(self):
        value = not 'not' in self.stdout
        self.output_nodes[0].value = value
        write_text(self.output_nodes[0].filename(), str(value))

class ToolFactory(object):
    def create_tool(self, label, name, toolpath, input_nodes, output_nodes, args):
        if name == 'lps2pbes':
            return Lps2PbesTool(label, name, toolpath, input_nodes, output_nodes, args)
        elif name == 'pbespgsolve':
            return PbesPgSolveTool(label, name, toolpath, input_nodes, output_nodes, args)
        elif name == 'lps2lts':
            return Lps2LtsTool(label, name, toolpath, input_nodes, output_nodes, args)
        elif name == 'lpsconfcheck':
            return LpsConfcheckTool(label, name, toolpath, input_nodes, output_nodes, args)
        elif name == 'ltsinfo':
            return LtsInfoTool(label, name, toolpath, input_nodes, output_nodes, args)
        elif name == 'pbes2bool':
            return Pbes2BoolTool(label, name, toolpath, input_nodes, output_nodes, args)
        elif name == 'bessolve':
            return BesSolveTool(label, name, toolpath, input_nodes, output_nodes, args)
        elif name == 'ltscompare':
            return LtsCompareTool(label, name, toolpath, input_nodes, output_nodes, args)
        elif name in ['bespp', 'lpspp', 'pbespp']:
            return PrintTool(label, name, toolpath, input_nodes, output_nodes, args)
        return Tool(label, name, toolpath, input_nodes, output_nodes, args)

