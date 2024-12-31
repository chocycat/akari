project('akari', 'cpp', default_options: ['cpp_std=c++23'])

enable_examples = get_option('examples')

sources = files(
  'src/logger/logger.cc',
)

executable('akari', sources: ['src/server/server.cc', sources])

# examples
if enable_examples
  # subdir('examples')
endif
