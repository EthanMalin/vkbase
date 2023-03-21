import os

# static variables
platforms = [
  'windows',
  'mac'
]

# create environment using mingw compiler
env = Environment(tools=['mingw'])

# check build script arguments
platform = ARGUMENTS.get('platform', 0)
if platform not in platforms:
  raise Exception('ERROR: platform argument required. Must be one of {0}'.format(platforms))

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

# glob source code
source = env.Glob(os.path.join(env['SRC'], '*.c'))
headers = env.Glob(os.path.join(env['SRC'], '*.h'))

# compile object files
objs = []
for s in source:
  name, _ = os.path.splitext(os.path.split(str(s))[1])
  objs.append(env.Object(os.path.join(env['OBJ'], name + '.o'), s))

# compile library
lib = env.Library(os.path.join(env['LIB'], 'vkbase'), objs, LIBS=['vulkan'])

# install library
env.Install('dist', lib)
env.Install('dist/include', headers)
