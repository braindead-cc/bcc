project(
	'lbf',
	'c',
	version: '0.1.0',
	license: 'MIT',
	default_options: [
		'c_std=c99',
		'warning_level=3',
	],
)

memtype = 'u8'
builddate = run_command('date', '+%Y-%m-%d %H%M').stdout().strip()

cc = meson.get_compiler('c')
ncurses = dependency('ncurses')

add_global_arguments(cc.get_supported_arguments([
	'-Wold-style-definition',
	'-Wmissing-prototypes',

	'-Winit-self',
	'-Wfloat-equal',
	'-Wstrict-prototypes',
	'-Wredundant-decls',
	'-Wimplicit-fallthrough=2',
	'-Wendif-labels',
	'-Wstrict-aliasing=2',
	'-Woverflow',
	'-Wformat=2',
	'-Wmissing-include-dirs',

	# trigraphs are fun!
	# "if IsErr(err) ??!??! panic(err);" :^)
	'-Wno-trigraphs',

	# sheesh
	# see src/util.c
	'-Wno-format-nonliteral',
]), language: 'c')

add_global_arguments([
        '-D_GNU_SOURCE',
        '-DBUILDDATE="' + builddate + '"',
        '-DVERSION="' + meson.project_version() + '"',
        '-DSPINNER_FANCY',
        '-DMEMTYPE=' + memtype,
], language: 'c')

inc = include_directories(['include', 'include/ccommon'])
lbf_sources = ['src/emitc.c',       'src/emitc.h',
		'src/emit_rust.c',  'src/emit_rust.h',
		'src/lbf.c',        'src/lbf.h',
		'src/lbfc.c',       'src/lbfc.h',
		'src/lbfd.c',       'src/lbfd.h',
		'src/lbfi.c',       'src/lbfi.h',
		'src/opt-nloops.c', 'src/opt-nloops.h',
		'src/opt-sloops.c', 'src/opt-sloops.h',
		'src/opt-squash.c', 'src/opt-squash.h',
		'src/parser.c',     'src/parser.h',
		'src/prepare.c',    'src/prepare.h',
		'src/status.c',     'src/status.h',
		'src/terminfo.c',   'src/terminfo.h',
		'src/util.c',       'src/util.h',
		'src/warn.c',       'src/warn.h']

lbf = executable(
	'lbf', lbf_sources,
	include_directories: inc,
	install: true,
	dependencies: ncurses
)

meson.add_install_script('scripts/post-install')
