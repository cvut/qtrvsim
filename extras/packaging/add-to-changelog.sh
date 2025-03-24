#!/usr/bin/env bash

CHANGELOG_FILE="extras/packaging/deb/debian/changelog"
MAIN_PROJECT_NAME_LOWER="qtrvsim"

if false ; then
  CMAKELISTS_DIR="$( old_pwd="" ;  while [ ! -e CMakeLists.txt ] ; do if [ "$old_pwd" = `pwd`  ] ; then exit 1 ; else old_pwd="$(pwd)" ; cd -L .. 2>/dev/null ; fi ; done ; pwd )"
  if [ $? -ne 0 ] ; then
    echo 'Can not found top level project directory with "CMakeLists.txt" file'
    exit 1
  fi
  cd "$CMAKELISTS_DIR"
else
  cd "$(dirname "$0")/../.."
  CMAKELISTS_DIR="$(pwd)"
fi

if [ $# -ge 1 ]; then
  V_TXT="$1"
else
  V_TXT="$(sed -n '/^[ \t]*project *(/{s///; :1; /) *$/!{N; b1;}; s///; s/^.*VERSION[\t ]\+\([^ ]*\)\b.*$/\1/p};' "$CMAKELISTS_DIR/CMakeLists.txt")"
fi

if [ -z "$V_TXT" ] ; then
  echo 'Version is not specified as argument neither determined from "CMakeLists.txt" file'
  exit 1
fi

V_TXT_W_SUF="$( echo "$V_TXT" | sed -n 's/\(.*-.*\)/\1/p')"
if [ -z "$V_TXT_W_SUF" ] ; then
  V_TXT_W_SUF="$V_TXT-1"
fi

V_DATE_MDY="$(date '+%m/%d/%Y')"
V_DATE_YMD="$(date '+%Y-%m-%d')"
V_DATE_RFC="$(date -R)"

V_USER_NAME="$(git config user.name)"
V_USER_EMAIL="$(git config user.email)"

if grep -q "$MAIN_PROJECT_NAME_LOWER ($V_TXT_W_SUF)" $CHANGELOG_FILE; then
  sed --in-place \
    -e '1,/^ -- .*$/s/^ -- .*$/'" -- $V_USER_NAME <$V_USER_EMAIL>  $V_DATE_RFC/" \
    $CHANGELOG_FILE
else
  cat >$CHANGELOG_FILE.tmp <<EOF
$MAIN_PROJECT_NAME_LOWER ($V_TXT_W_SUF) unstable; urgency=medium

  * Debian package updated to version $V_TXT.

 -- $V_USER_NAME <$V_USER_EMAIL>  $V_DATE_RFC

EOF
  cat $CHANGELOG_FILE >> $CHANGELOG_FILE.tmp
  mv $CHANGELOG_FILE.tmp $CHANGELOG_FILE
fi

if [ -z "$EDITOR" ] ; then
  EDITOR=nano
fi
$EDITOR $CHANGELOG_FILE

echo Press enter to continue
read x

git add $CHANGELOG_FILE

echo >.git/GITGUI_MSG "Version updated to $V_TXT"

git gui

git tag -d v$V_TXT

git tag -s v$V_TXT

# TODO
#if [ -x /usr/lib/obs-build/changelog2spec ]; then
#  /usr/lib/obs-build/changelog2spec debian/changelog >../$MAIN_PROJECT_NAME_LOWER.changes
#elif [ -x /usr/lib/build/changelog2spec ]; then
#  /usr/lib/build/changelog2spec debian/changelog >../$MAIN_PROJECT_NAME_LOWER.changes
#fi
