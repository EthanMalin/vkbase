import os
import sys

# static variables
platforms = [
  'windows',
  'mac'
]
apps = [
  'pyramid'
]
natives = [
  'cocoa',
  'glfw',
]

# create environment using mingw compiler
env = Environment(tools=['mingw'])

# determine platform from target (e.g. "scons mac")
platform = next((t for t in COMMAND_LINE_TARGETS if t in platforms), None)
if platform is None:
  raise Exception('ERROR: platform target required. Must be one of {0}'.format(platforms))

# native windowing backend — defaults to glfw; override with native=cocoa on mac
native = ARGUMENTS.get('native', 'glfw')
if native not in natives:
  raise Exception('ERROR: native must be one of {0}'.format(natives))
if native == 'cocoa' and platform != 'mac':
  raise Exception('ERROR: native=cocoa requires platform=mac')

# set up base environment variables
env['ROOT'] = GetLaunchDir()
env['SRC'] = os.path.join(env['ROOT'], 'src')
env['OBJ'] = os.path.join(env['ROOT'], 'build', 'obj')
env['LIB'] = os.path.join(env['ROOT'], 'build', 'lib')

# set up platform environment
env = SConscript(os.path.join('platform', platform, 'SConscript'), 'env')

# add Vulkan headers to include path
env.Append(CPPPATH=[
    os.path.join(env['VULKAN_SDK'], 'Include')
])

# glob core sources — exclude vkbnative_* backends (selected via native=)
sources = [f for f in env.Glob(os.path.join(env['SRC'], '*.c'))
           if 'vkbnative_' not in os.path.basename(str(f))]
headers = env.Glob(os.path.join(env['SRC'], '*.h'))

# compile core object files
objs = []
for source in sources:
  name, _ = os.path.splitext(os.path.split(str(source))[1])
  objs.append(env.Object(os.path.join(env['OBJ'], name + '.o'), source))

# compile native backend
native_extra_libs      = []
native_extra_linkflags = []

if native == 'cocoa':
    objc_action = 'clang $CCFLAGS $_CPPDEFFLAGS $_CPPINCFLAGS -x objective-c -fobjc-arc -c $SOURCE -o $TARGET'
    env.Append(BUILDERS={'ObjCObject': Builder(action=objc_action, suffix='.o', src_suffix='.m')})
    m_src = os.path.join(env['SRC'], 'vkbnative_cocoa.m')
    m_obj = os.path.join(env['OBJ'], 'vkbnative_cocoa.o')
    objs.append(env.ObjCObject(m_obj, m_src))
    native_extra_linkflags = [
        '-framework', 'Cocoa',
        '-framework', 'QuartzCore',
        '-framework', 'Metal',
    ]
elif native == 'glfw':
    glfw_src = os.path.join(env['SRC'], 'vkbnative_glfw.c')
    glfw_obj = os.path.join(env['OBJ'], 'vkbnative_glfw.o')
    objs.append(env.Object(glfw_obj, glfw_src))
    native_extra_libs = ['glfw']

# compile library
lib = env.Library(os.path.join(env['LIB'], 'vkbase'), objs)

# install library and alias to platform target
installed = env.Install(os.path.join('dist', 'lib'), lib) + env.Install(os.path.join('dist', 'include'), headers)
env.Alias(platform, installed)

# shader source extensions to compile
SHADER_EXTS = ['.vert', '.frag', '.comp', '.geom', '.tesc', '.tese']

# build app targets
for app in [a for a in apps if a in COMMAND_LINE_TARGETS]:
    app_env = env.Clone()
    app_env.Append(CPPPATH=[
        env['SRC'],
        os.path.join(env['ROOT'], 'apps', app),  # resolves #include "shaders/shaders.h"
    ])
    if platform == 'mac':
        vulkan_lib_dir = os.path.join(env['VULKAN_SDK'], 'macOS', 'lib')
        app_env.Append(LINKFLAGS=['-Wl,-rpath,' + vulkan_lib_dir])
    app_env.Append(LINKFLAGS=native_extra_linkflags)

    # --- compile GLSL shaders to SPIR-V ---
    shader_src_dir   = os.path.join(env['ROOT'], 'apps', app, 'shaders')
    shader_build_dir = os.path.join(env['ROOT'], 'build', 'shaders', app)
    spv_targets = []
    for ext in SHADER_EXTS:
        for src in app_env.Glob(os.path.join(shader_src_dir, '*' + ext)):
            spv_name = os.path.basename(str(src)) + '.spv'
            spv_out  = os.path.join(shader_build_dir, spv_name)
            spv_targets += app_env.Command(
                spv_out, str(src),
                '$SHADER_COMPILER -V $SOURCE -o $TARGET')

    # --- generate shaders.h from all .spv outputs ---
    shaders_h = os.path.join(shader_src_dir, 'shaders.h')
    header = app_env.Command(
        shaders_h, spv_targets,
        f'"{sys.executable}" tools/spv_to_header.py -o $TARGET $SOURCES'
    ) if spv_targets else []

    # --- compile app C sources ---
    app_sources = app_env.Glob(os.path.join('apps', app, '*.c'))
    app_objs = []
    for source in app_sources:
        name, _ = os.path.splitext(os.path.split(str(source))[1])
        obj = app_env.Object(os.path.join(env['OBJ'], 'apps', app, name + '.o'), source)
        if header:
            app_env.Depends(obj, header)
        app_objs.append(obj)

    app_exe = app_env.Program(
        os.path.join('build', 'apps', app, app),
        app_objs,
        LIBS=['vkbase', 'vulkan'] + native_extra_libs,
        LIBPATH=[env['LIB']]
    )
    app_env.Depends(app_exe, lib)
    env.Alias(app, app_exe)
