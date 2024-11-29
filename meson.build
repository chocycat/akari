project('nostrum', 'c')

enable_examples = get_option('examples')

# main
source = 'src/main.c'
executable('nostrum', source)

# examples
if enable_examples
  subdir('examples')
endif
