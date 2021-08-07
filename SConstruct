env = Environment()
env.VariantDir('build', 'src', duplicate=0)

source = env.Glob('build/*.c')
headers = env.Glob('build/*.h')

library = env.Library('vkbase', source, LIBS=['glfw', 'vulkan'])

env.Install('dist', library)
env.Install('dist/include', headers)
