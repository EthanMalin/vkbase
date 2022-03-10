import os

env = Environment(tools=['mingw'])

env['ROOT'] = GetLaunchDir()
env['SRC'] = os.path.join(env['ROOT'], 'src')
env['LIB'] = os.path.join(env['ROOT'], 'build', 'lib')

env = SConscript(os.path.join('platform', 'windows', 'SConscript'), 'env')

env.Append(LIBPATH=[
    os.path.join(env['VULKAN'], '1.2.135.0', 'Lib')
])

env.Append(CPPPATH=[
    os.path.join(env['VULKAN'], '1.2.135.0', 'Include')
])

source = env.Glob(os.path.join(env['SRC'], '*.c'))
headers = env.Glob(os.path.join(env['SRC'], '*.h'))

library = env.Library(os.path.join(env['LIB'], 'vkbase'), source, LIBS=['vulkan-1'])

env.Install('dist', library)
env.Install('dist/include', headers)
