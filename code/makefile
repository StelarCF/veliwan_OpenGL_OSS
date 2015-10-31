##########################################################################
# © Vlad-Alexandru Sănduleac                                             #
# The makefile for veliwan engine and game, including publishing.        #
# This makefile should prefferably be the same for all operating systems.#
##########################################################################

# Executable sources
# Please only include .cpps, no headers
game_src = GameCore.cpp GameObject.cpp # Game Core
render_src = RenderFactory.cpp RenderGroup.cpp RenderObject.cpp # Render sources
render_src += RenderCore.cpp GhostCamera.cpp RTSCamera.cpp # Render sources
io_src = IOCore.cpp # IO sources
external_src = lodepng.cpp # externals
sources = $(game_src) $(render_src) $(io_src) $(external_src) main.cpp

# Directories we need
# Note: everything that includes ".." might not work in Windows. Do check.
root_dir = $(shell pwd)
code_dir = core
asset_dir = ../assets
shader_dir = shaders
publish_dir = ../publish
build_dir = /tmp/veliwan-build
bin_dir = ../bin
bin = $(bin_dir)
bin_old = ../bin_old

# Executable names
linexec_name = _velexec
linexecdebug_name = _velexec_debug

# Compilers
lincc = clang++
maccc = :
wincc = : 
# i586-mingw32msvc-gcc

# Libraries, by OS
linlib = -lGL -lGLU -lGLEW -lSDL2

# Options for debugging and release
lindebugopt = -DVELIWAN_DEBUG -pg -g -O0
linreleaseopt = -O3

# Targets
# All is going to be our default, we don't want to move lots of files since it slows down quick compilation
all: executable

executable: linux_executable
	cp $(build_dir)/$(linexec_name) $(publish_dir); \
	cp $(build_dir)/$(linexecdebug_name) $(publish_dir)

shaders: publish_folder
	# Flatten the shaders and copy them into publish
	>&2 echo "Copying Shaders"; \
	find $(shader_dir) -type f -iname '*' -exec cp \{\} $(publish_dir) \;;

linux_executable: build_folder bin_folder
	find $(code_dir) -type f -iname '*' -exec cp \{\} $(build_dir) \;; \
	>&2 echo "Compiling Linux"; \
	(cd $(build_dir) && $(lincc) -o $(linexec_name) $(sources) $(linreleaseopt) $(linlib)); \
	(cd $(build_dir) && $(lincc) -o $(linexecdebug_name) $(sources) $(lindebugopt) $(linlib)); \
	cp $(build_dir)/$(linexec_name) $(bin); \
	cp $(build_dir)/$(linexecdebug_name) $(bin)

bin_folder:
	rm -R $(bin_old); \
	mv $(bin) $(bin_old); \
	mkdir -p $(bin)

build_folder:
	rm -R $(build_dir); \
	mkdir -p $(build_dir)

publish_folder:
	# Make sure all the directories we need for publish exist and are empty
	rm -R $(publish_dir); \
	mkdir -p $(publish_dir)

# Install is going to be an alias of publish
install: publish
publish: publish_folder assets executable shaders
	cp $(bin)/$(linexec_name) $(publish_dir); \
	cp $(bin)/$(linexecdebug_name) $(publish_dir)

assets:
	# Flatten asset_dir and copy everything into publish_dir TODO: Make it so the directory structure in publish isn't flat
	find $(asset_dir) -type f -iname '*' -exec cp \{\} $(publish_dir) \;;

