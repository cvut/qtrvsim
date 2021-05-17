# Add file hashes to placeholder location in a template file.
# Used for packaging.
#
# Example:
# cmake -DTEMPLATE=PKGBUILD.in -DOUTPUT=PKGBUILD -DFILE=qtrvsim_0.8.0.tar.xz -P AddFileHashes.cmake
#
# See extras/packaging/arch/PKGBUILD.in for template examples.
# Note that most files are configured (injected with information) twice:
# First, during configure phase, package information is added and FILE_*
# placeholders are left intact. Second, after source packing, FILE_*
# placeholders are resolved.

file(MD5 ${FILE} FILE_MD5)
file(SHA1 ${FILE} FILE_SHA1)
file(SHA256 ${FILE} FILE_SHA256)
file(SIZE ${FILE} FILE_SIZE)
configure_file(${TEMPLATE} ${OUTPUT})