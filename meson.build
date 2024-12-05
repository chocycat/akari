project('nostrum', 'c')

enable_examples = get_option('examples')

sources = files(
  'src/client/client.c',

  'src/common/buffer.c',
  'src/common/message.c',

  'src/server/connection.c',
)

executable('nostrum', sources: ['src/server/server.c', sources])

# examples
if enable_examples
  subdir('examples')
endif
