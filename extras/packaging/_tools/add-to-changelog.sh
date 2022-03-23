#!/usr/bin/env bash

CHANGELOG_FILE="extras/packaging/deb/debian/changelog"

if [ $# -lt 1 ]; then
  echo "Version has to be specified"
  exit 1
fi

V_TXT="$1"

cd "$(dirname "$0")/../.."

V_DATE_MDY="$(date '+%m/%d/%Y')"
V_DATE_YMD="$(date '+%Y-%m-%d')"
V_DATE_RFC="$(date -R)"

V_USER_NAME="$(git config user.name)"
V_USER_EMAIL="$(git config user.email)"

if grep -q "qtrvsim ($V_TXT)" $CHANGELOG_FILE; then
  sed --in-place \
    -e '1,/^ -- .*$/s/^ -- .*$/'" -- $V_USER_NAME <$V_USER_EMAIL>  $V_DATE_RFC/" \
    $CHANGELOG_FILE
else
  cat >$CHANGELOG_FILE.tmp <<EOF
qtrvsim ($V_TXT) unstable; urgency=medium

  * Debian package updated to version $V_TXT.

 -- $V_USER_NAME <$V_USER_EMAIL>  $V_DATE_RFC

EOF
  cat $CHANGELOG_FILE >> $CHANGELOG_FILE.tmp
  mv $CHANGELOG_FILE.tmp $CHANGELOG_FILE
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
#  /usr/lib/obs-build/changelog2spec debian/changelog >../qtrvsim.changes
#elif [ -x /usr/lib/build/changelog2spec ]; then
#  /usr/lib/build/changelog2spec debian/changelog >../qtrvsim.changes
#fi
