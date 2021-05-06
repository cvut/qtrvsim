# Support code for package handling.
# Mainly intended to generate inputs for Open Build Service.

# Keep file properties placeholder in configure phase, they are replaced in build phase
set(FILE_NAME "\@FILE_NAME\@")
set(FILE_SIZE "\@FILE_SIZE\@")
set(FILE_MD5 "\@FILE_MD5\@")
set(FILE_SHA1 "\@FILE_SHA1\@")
set(FILE_SHA256 "\@FILE_SHA256\@")
set(FILE2_NAME "\@FILE2_NAME\@")
set(FILE2_SIZE "\@FILE2_SIZE\@")
set(FILE2_MD5 "\@FILE2_MD5\@")
set(FILE2_SHA1 "\@FILE2_SHA1\@")
set(FILE2_SHA256 "\@FILE2_SHA256\@")


# Source file tar generation.

# Make sure that destination path exists as it wont be created automatically.
file(MAKE_DIRECTORY ${PACKAGE_OUTPUT_PATH})

find_package(Git REQUIRED)
# Register xz command for usage with git. (TODO: better cross platform or configurable)
execute_process(COMMAND ${GIT_EXECUTABLE} config --replace-all tar.tar.xz.command "xz -c"
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Command to build source archive from git HEAD.
# TODO: should uncommited files be included (https://unix.stackexchange.com/questions/124495/git-archive-including-uncommitted-modified-files)
add_custom_command(OUTPUT ${PACKAGE_SOURCE_ARCHIVE_FILE}
                   COMMAND ${GIT_EXECUTABLE} archive --format=tar.xz
                   --prefix=${PACKAGE_TOPLEVEL_DIR}/ HEAD
                   -o ${PACKAGE_SOURCE_ARCHIVE_PATH}
                   WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
add_custom_target(source_archive
                  DEPENDS ${PACKAGE_SOURCE_ARCHIVE_FILE})

# Procedure adding support for individual OS distributions.
#
# @param target_name            name of created target for simple reference (recommended: OS name)
# @param config_file_name       name of resulting config file
# @param template               path to cmake template file (relative from source dir)
#
# NOTE: ${PACKAGE_SOURCE_ARCHIVE_FILE} is assumed to exist in scope and be unique for project
#

macro(package_config_file target_name config_file_name template)
	# The @ONLY option disable replacement of ${} which may be used by shell as well.
	configure_file(${template} ${config_file_name}.in @ONLY)
	add_custom_command(OUTPUT ${config_file_name}
	                   COMMAND ${CMAKE_COMMAND} -DFILE="${PACKAGE_SOURCE_ARCHIVE_PATH}"
	                   -DTEMPLATE="${CMAKE_BINARY_DIR}/${config_file_name}.in"
	                   -DOUTPUT=${PACKAGE_OUTPUT_PATH}/${config_file_name}
	                   -P "${CMAKE_SOURCE_DIR}/cmake/AddFileHashes.cmake"
	                   DEPENDS source_archive)
	add_custom_target(${target_name}
	                  DEPENDS ${config_file_name})
endmacro()

macro(package_debian_quilt target_name config_file_name template debian_dir output_archive)
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/debian)
	# The @ONLY option disable replacement of ${} which may be used by shell as well.
	configure_file(${debian_dir}/control.in ${CMAKE_BINARY_DIR}/debian/control @ONLY)
	file(COPY ${CMAKE_BINARY_DIR}/debian/control DESTINATION ${CMAKE_BINARY_DIR}/debian
	     FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
	file(COPY ${debian_dir}/compat
	     DESTINATION ${CMAKE_BINARY_DIR}/debian
	     FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
	file(COPY ${debian_dir}/rules
	     DESTINATION ${CMAKE_BINARY_DIR}/debian
	     FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_EXECUTE GROUP_EXECUTE GROUP_EXECUTE)
	file(COPY ${debian_dir}/docs
	     DESTINATION ${CMAKE_BINARY_DIR}/debian
	     FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
	file(COPY ${debian_dir}/changelog
	     DESTINATION ${CMAKE_BINARY_DIR}/debian
	     FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
	file(COPY ${debian_dir}/source/format
	     DESTINATION ${CMAKE_BINARY_DIR}/debian/source
	     FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
	file(ARCHIVE_CREATE
	     OUTPUT ${PACKAGE_OUTPUT_PATH}/${output_archive}
	     PATHS ${CMAKE_BINARY_DIR}/debian
	     FORMAT gnutar
	     COMPRESSION XZ)
	set(DEBIAN_ARCHIVE_FILE ${output_archive})
	file(MD5 ${PACKAGE_OUTPUT_PATH}/${output_archive} DEBIAN_MD5)
	file(SHA1 ${PACKAGE_OUTPUT_PATH}/${output_archive} DEBIAN_SHA1)
	file(SHA256 ${PACKAGE_OUTPUT_PATH}/${output_archive} DEBIAN_SHA256)
	file(SIZE ${PACKAGE_OUTPUT_PATH}/${output_archive} DEBIAN_SIZE)
	configure_file(${template} ${config_file_name}.in @ONLY)
	add_custom_command(OUTPUT ${config_file_name}
	                   COMMAND ${CMAKE_COMMAND} -DFILE="${PACKAGE_SOURCE_ARCHIVE_PATH}"
	                   -DTEMPLATE="${CMAKE_BINARY_DIR}/${config_file_name}.in"
	                   -DOUTPUT=${PACKAGE_OUTPUT_PATH}/${config_file_name}
	                   -P "${CMAKE_SOURCE_DIR}/cmake/AddFileHashes.cmake"
	                   DEPENDS source_archive)
	add_custom_target(${target_name}
	                  DEPENDS ${config_file_name})

	message(STATUS "Debian archive created")
endmacro()