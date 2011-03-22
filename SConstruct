## Copyright (C) 2007 Crestez Leonard - cdleonard@gmail.com
##
## This file is part of Caelum
##
## Caelum is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## Caelum is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

import os, sys, glob, exceptions
import SCons.Action, SCons.Builder, SCons.Scanner.C, SCons.Util, SCons.Script
from stat import *

# This line is supposed to ensure the proper scons versions is installed.
#
# The version required here might not actually be correct.
# Testing this requires installing multiple scons versions in parallel.
SCons.Script.EnsureSConsVersion(1,0,0)
SConsignFile()

# Add gch (gcc precompiled header) support.
def AddGchSupport(env):
    GchAction = SCons.Action.Action('$GCHCOM', '$GCHCOMSTR')

    def gen_suffix(env, sources):
        return sources[0].get_suffix() + env['GCHSUFFIX']

    GchBuilder = SCons.Builder.Builder(
            action = GchAction,
            source_scanner = SCons.Scanner.C.CScanner(),
            suffix = gen_suffix,
    )

    env.Append(BUILDERS = dict(
        gch = env.Builder(
            action = GchAction,
            target_factory = env.fs.File,
        ),
    ))

    def gch_emitter(target,source,env):
        SCons.Defaults.StaticObjectEmitter( target, source, env )

        scanner = SCons.Scanner.C.CScanner()
        path = scanner.path(env)
        deps = scanner(source[0], env, path)

        if env.has_key('Gch'):
            gch = env['Gch'][0]
            if gch.sources[0] in deps:
                env.Depends(target, gch)

        return (target, source)

    env['BUILDERS']['Gch'] = GchBuilder
    env['GCHCOM'] = '$CXX -o $TARGET -x c++-header -c $CXXFLAGS $CCFLAGS $_CCCOMCOM $SOURCE'
    env['GCHSUFFIX'] = '.gch'

    for suffix in SCons.Util.Split('.c .C .cc .cxx .cpp .c++'):
        env['BUILDERS']['StaticObject'].add_emitter(suffix, gch_emitter)
        env['BUILDERS']['SharedObject'].add_emitter(suffix, gch_emitter)
 
# Setup build variables (used to be options in older versions).
# These are specified as "scons debug=1" on the command line and are saved in SConstruct.config
def HandleVariables(env):
    # Create options
    vars = Variables('SConstruct.config')
    vars.AddVariables(
        BoolVariable('debug', 'Build with debugging options', 0),
        BoolVariable('precompiled_headers', 'Use precompiled headers', 1),
        BoolVariable('generate_cfg', 'Automatically generate resources.cfg and plugins.cfg', 1),
        ('extra_ccflags', 'Extra C/C++ flags; like -Wno-deprecated', ''),
    )

    # Create help
    Help(vars.GenerateHelpText(env))

    # Update env and save
    vars.Update(env);
    vars.Save('SConstruct.config', env);

# Search for external libraries. Returns a dict of extra flags
def AutoConfigure(env):

    # Get dependency flags from pkg-config.
    def TryParseFlags(env, cmdline):
        try:
            return env.ParseFlags(cmdline)
        except exceptions.OSError, e:
            return {}

    depflags = {
        'ogre': TryParseFlags(env, '!pkg-config OGRE --cflags --libs'),
        'ois': TryParseFlags(env, '!pkg-config OIS --cflags --libs'),
        'cegui': TryParseFlags(env, '!pkg-config CEGUI CEGUI-OGRE --cflags --libs'),
    }

    # Don't do checks on cleaning.
    if env.GetOption('clean'):
        return depflags

    # Check if the dependency flags work correctly. Don't add any.
    conf_env = env.Clone();
    map(conf_env.MergeFlags, depflags.values());
    conf = conf_env.Configure()

    if not conf.CheckCXXHeader('Ogre.h'):
        print 'Fatal error: Did not find Ogre library or headers.'
        Exit(1)

    if not conf.CheckCXXHeader('OIS.h'):
        print 'Warning: Did not find OIS library or headers, demos will be skipped.'
        if 'ois' in depflags: del depflags['ois']

    if depflags.has_key('ois') and not conf.CheckCXXHeader('CEGUI/CEGUI.h'):
        print 'Warning: Did not find CEGUI library, CEGUI demos will be skipped.'
        if 'cegui' in depflags: del depflags['cegui']

    # Don't replace env
    conf.Finish()
    return depflags

# A scons action which generates plugins.cfg from plugins.cfg.sample.
def GeneratePluginsCfg(target, source, env):
    try:
        import subprocess;
        plugin_dir = subprocess.Popen(['pkg-config', 'OGRE', '--variable=plugindir'], stdout=subprocess.PIPE).communicate()[0]
    except exceptions.OSError, e:
        print "Couldn''t determine ogre plugin dir using pkg-config:", e
        return
    plugin_dir_marker = '__OGRE_PLUGIN_PATH_HERE__'
    targetFile = sourceFile = None
    targetFileName, sourceFileName = str(target[0]), str(source[0])
    try:
        targetFile = open(targetFileName, "w+")
        sourceFile = open(sourceFileName, "r+")
        changed = False
        while True:
            srcLine = sourceFile.readline();
            if len(srcLine) == 0:
                break
            tgtLine = srcLine.replace(plugin_dir_marker, plugin_dir)
            if (srcLine != tgtLine):
                changed = True
            targetFile.write(tgtLine)
        if not changed:
            print("Warning: didn't find marker '" + plugin_dir_marker + "' in '" + sourceFileName + "'")
    finally:
        if targetFile != None:
            targetFile.close()
        if sourceFile != None:
            sourceFile.close()


# Create the actual projects.
def CreateProjects(env, depflags):
    # configure debug in standard env.
    if env.get('debug', 0):
        env.Append(CPPDEFINES = '_DEBUG')
        env.Append(CCFLAGS = Split('-g -Wsign-compare -Wall'))
    else:
        env.Append(CPPDEFINES = 'NDEBUG')
        env.Append(CCFLAGS = Split('-O2'))

    env.AppendUnique(CCFLAGS=Split(env.get('extra_ccflags', '')))

    if (env.get('generate_cfg', 1)):
        env.Command('resources.cfg', 'resources.cfg.sample', Copy("$TARGET", "$SOURCE"))
        env.Command('plugins.cfg', ['plugins.cfg.sample'], GeneratePluginsCfg)
        env.AlwaysBuild('plugins.cfg')

    # Main caelum lib
    ogreEnv = env.Clone()
    ogreEnv.Append(**depflags['ogre'])
    ogreEnv.Append(CPPPATH = 'main/include')

    caelumArgs = {
        'target': 'Caelum',
        'source': Glob('main/src/*.cpp'),
    }
    if (env.get('precompiled_headers', 1)):
        caelumArgs['Gch'] = ogreEnv.Gch('main/include/CaelumPrecompiled.h')
    ogreEnv.Library(**caelumArgs)

    # Environment for samples
    demoEnv = ogreEnv.Clone()
    demoEnv.AppendUnique(LIBPATH = '.')
    demoEnv.AppendUnique(LIBS = 'Caelum')
    demoEnv.AppendUnique(CPPPATH = 'samples/include');

    # OIS-only demos
    if depflags.has_key('ois'):
        oisDemoEnv = demoEnv.Clone()
        oisDemoEnv.AppendUnique(**depflags['ois'])
    else:
        oisDemoEnv = None

    # CEGUI-based demos
    if depflags.has_key('cegui'):
        ceguiDemoEnv = oisDemoEnv.Clone()
        ceguiDemoEnv.AppendUnique(**depflags['cegui'])
    else:
        ceguiDemoEnv = None

    if oisDemoEnv:
        oisDemoEnv.Program('CaelumDemo', 'samples/src/CaelumDemo.cpp')
    if ceguiDemoEnv:
        ceguiDemoEnv.Program('CaelumLab', 'samples/src/CaelumLab.cpp')
    # This is how you add an ogre scratch exe:
    #demoEnv.Program('CaelumUnitTest', 'samples/src/CaelumUnitTest.cpp')

env = Environment()
HandleVariables(env)
# Starting in Scons 1.2 PKG_CONFIG_PATH must be forwarded explicitly
env["ENV"]["PKG_CONFIG_PATH"] = os.environ.get("PKG_CONFIG_PATH")
if (env.get('precompiled_headers', 1)):
    AddGchSupport(env)
depflags = AutoConfigure(env)
CreateProjects(env, depflags)
