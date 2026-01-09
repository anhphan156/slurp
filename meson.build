add_project_arguments('-Wno-unused-parameter', language: 'c')

wayland_protos = dependency('wayland-protocols', version: '>=1.32')

subdir('protocol')

lib_slurp = static_library(
	'slurp',
	[
		'main.c',
		'lock.c',
		'pool-buffer.c',
		'render.c',
		'box.c',
		'sticker.c',
		protos_src,
	],
	include_directories: 'include',
)

scdoc = find_program('scdoc', required: get_option('man-pages'))

if scdoc.found()
	man_pages = ['slurp.1.scd']

	foreach src : man_pages
		topic = src.split('.')[0]
		section = src.split('.')[1]
		output = '@0@.@1@'.format(topic, section)

		custom_target(
			output,
			input: src,
			output: output,
			command: scdoc,
			feed: true,
			capture: true,
			install: true,
			install_dir: get_option('mandir') / 'man' + section,
		)
	endforeach
endif
