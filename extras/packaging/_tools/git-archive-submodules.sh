#!/usr/bin/env bash

set -eu

# Tool to crate source archive including submodules
# based on (MIT): https://github.com/nzanepro/git-archive-submodules

# Parameters: project-root destination file_name module-prefix version [git-command] [tar-command] [xz-command]

export TMPDIR=`realpath ${2}`/pack/

export SOURCE=`realpath ${1}`
export DESTINATION=`realpath ${2}/${3}`
export TARMODULE=${4}
export TARVERSION=${5}
export TARPREFIX="${TARMODULE}-${TARVERSION}"
export GIT=${6:-"git"}
export TAR=${7:-"tar"}
export XZ=${8:-"xz"}

mkdir -p ${TMPDIR}

pushd ${SOURCE} || exit

# create module archive
${GIT} archive --prefix=${TARPREFIX}/ -o ${TMPDIR}${TARPREFIX}.tar HEAD
if [[ ! -f "${TMPDIR}${TARPREFIX}.tar" ]]; then
  echo "ERROR: base sourcecode archive was not created. check git output in log above."
  exit 1
fi

# Makefile is only helper for development and should be excluded in release.
# The command is allowed to fail if Makefile is not found.
${TAR} -f "${TMPDIR}${TARPREFIX}.tar" --delete ${TARPREFIX}/Makefile 2>/dev/null || true

# force init submodules
${GIT} submodule update --init --recursive

# tar each submodule recursively
${GIT} submodule foreach --recursive 'git archive --prefix=${TARPREFIX}/${displaypath}/ HEAD > ${TMPDIR}tmp.tar &&
${TAR} --concatenate --file=${TMPDIR}${TARPREFIX}.tar ${TMPDIR}tmp.tar'

popd || exit

# compress tar file
${XZ} ${TMPDIR}${TARPREFIX}.tar
if [[ ! -f "${TMPDIR}${TARPREFIX}.tar.xz" ]]; then
  echo "ERROR: xzipped archive was not created. check git output in log above."
  exit 1
fi


cp ${TMPDIR}${TARPREFIX}.tar.xz ${DESTINATION}
if [[ -f "${TMPDIR}${TARPREFIX}.tar.xz" ]]; then
  rm ${TMPDIR}${TARPREFIX}.tar.xz
  echo "created ${DESTINATION}"
else
  echo "ERROR copying ${TMPDIR}${TARPREFIX}.tar.xz to ${DESTINATION}"
  usage
  exit 1
fi

# cleanup
rm -r ${TMPDIR}
