project('random-ayah-cli', 'c')

curldep = dependency('libcurl')
jsondep = dependency('json-c')
incdir = include_directories('/usr/include/json-c/')

executable('rand-ayah', 'main.c', dependencies: [curldep, jsondep], include_directories: incdir)
