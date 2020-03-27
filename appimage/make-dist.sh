#!/bin/bash

#APP_VER=0.0.1
APP_NAME=qtmips
SRC_DIR=/home/fanda/proj/qtmips
QT_DIR=/home/fanda/programs/qt5/5.14.1/gcc_64
WORK_DIR=/tmp/_distro/qtmips

APP_IMAGE_TOOL=/home/fanda/programs/appimagetool-x86_64.AppImage

help() {
	echo "Usage: make-dist.sh [ options ... ]"
	echo "required options: src-dir, qt-dir, work-dir, image-tool"
	echo -e "\n"
	echo "avaible options"
	echo "    --app-name <name>        custom application name, ie: my-qe-test"
	echo "    --app-version <version>  application version, ie: 1.0.0"
	echo "    --src-dir <path>         quickbox project root dir, *.pro file is located, ie: /home/me/quickbox"
	echo "    --qt-dir <path>          QT dir, ie: /home/me/qt5/5.13.1/gcc_64"
	echo "    --work-dir <path>        directory where build files and AppImage will be created, ie: /home/me/quickevent/AppImage"
	echo "    --image-tool <path>      path to AppImageTool, ie: /home/me/appimagetool-x86_64.AppImage"
	echo "    --no-clean               do not rebuild whole project when set to 1"
	echo -e "\n"
	echo "example: make-dist.sh --src-dir /home/me/qtmips --qt-dir /home/me/qt5/5.13.1/gcc_64 --work-dir /home/me/qtmips/AppImage --image-tool /home/me/appimagetool-x86_64.AppImage"
	exit 0
}

error() {
	echo -e "\e[31m${1}\e[0m"
}

while [[ $# -gt 0 ]]
do
key="$1"
# echo key: $key
case $key in
	--app-name)
	APP_NAME="$2"
	shift # past argument
	shift # past value
	;;
	--app-version)
	APP_VER="$2"
	shift # past argument
	shift # past value
	;;
	--qt-dir)
	QT_DIR="$2"
	shift # past argument
	shift # past value
	;;
	--src-dir)
	SRC_DIR="$2"
	shift # past argument
	shift # past value
	;;
	--work-dir)
	WORK_DIR="$2"
	shift # past argument
	shift # past value
	;;
	--image-tool)
	APP_IMAGE_TOOL="$2"
	shift # past argument
	shift # past value
	;;
	--no-clean)
	NO_CLEAN=1
	shift # past value
	;;
	-h|--help)
	shift # past value
	help
	;;
	*)    # unknown option
	echo "ignoring argument $1"
	shift # past argument
	;;
esac
done

if [ ! -d $SRC_DIR ]; then
   	error "invalid source dir, use --src-dir <path> to specify it\n"
	help
fi
if [ ! -d $QT_DIR ]; then
	error "invalid QT dir, use --qt-dir <path> to specify it\n"
	help
fi
if [ ! -f $APP_IMAGE_TOOL ]; then
	error "invalid path to AppImageTool, use --image=tool <path> to specify it\n"
	help
fi
if [ ! -x $APP_IMAGE_TOOL ]; then
	error "AppImageTool file must be executable, use chmod +x $APP_IMAGE_TOOL\n"
	help
fi


if [ -z $APP_VER ]; then
	APP_VER=`grep APP_VERSION $SRC_DIR/quickevent/app/quickevent/src/appversion.h | cut -d\" -f2`
	echo "Distro version not specified, deduced from source code: $APP_VER" >&2
	#exit 1
fi

echo APP_VER: $APP_VER
echo APP_NAME: $APP_NAME
echo SRC_DIR: $SRC_DIR
echo WORK_DIR: $WORK_DIR
echo NO_CLEAN: $NO_CLEAN

if [ -z $USE_SYSTEM_QT ]; then
	QT_LIB_DIR=$QT_DIR/lib
	QMAKE=$QT_DIR/bin/qmake
	DISTRO_NAME=$APP_NAME-$APP_VER-linux64
else
	QT_DIR=/usr/lib/i386-linux-gnu/qt5
	QT_LIB_DIR=/usr/lib/i386-linux-gnu
	QMAKE=/usr/bin/qmake
	DISTRO_NAME=$APP_NAME-$APP_VER-linux32
fi

echo QT_DIR: $QT_DIR

BUILD_DIR=$WORK_DIR/_build
DIST_DIR=$WORK_DIR/$DISTRO_NAME
DIST_LIB_DIR=$DIST_DIR/lib
DIST_BIN_DIR=$DIST_DIR/bin

if [ -z $NO_CLEAN ]; then
	echo removing directory $WORK_DIR
	rm -r $BUILD_DIR
	mkdir -p $BUILD_DIR
fi

cd $BUILD_DIR
$QMAKE $SRC_DIR/qtmips.pro CONFIG+=release CONFIG+=force_debug_info CONFIG+=separate_debug_info -r -spec linux-g++
make -j8
if [ $? -ne 0 ]; then
	echo "Make Error" >&2
	exit 1
fi

rm -r $DIST_DIR
mkdir -p $DIST_DIR

# rsync -av --exclude '*.debug' $BUILD_DIR/lib/ $DIST_LIB_DIR
# echo rsync -av $BUILD_DIR/qtmips_gui/qtmips_gui $DIST_BIN_DIR
rsync -av $BUILD_DIR/qtmips_gui/qtmips_gui $DIST_BIN_DIR/

#rsync -a --exclude '*.debug'v $QT_DIR/lib/libicu* $DIST_LIB_DIR

rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Core.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Gui.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Widgets.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5XmlPatterns.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Network.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Sql.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Xml.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Qml.so* $DIST_LIB_DIR
# rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Quick.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Svg.so* $DIST_LIB_DIR
# rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Script.so* $DIST_LIB_DIR
# rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5ScriptTools.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5PrintSupport.so* $DIST_LIB_DIR
# rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5SerialPort.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5DBus.so* $DIST_LIB_DIR
# rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Multimedia.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5XcbQpa.so* $DIST_LIB_DIR

rsync -av --exclude '*.debug' $QT_LIB_DIR/libicu*.so* $DIST_LIB_DIR

rsync -av --exclude '*.debug' $QT_DIR/plugins/platforms/ $DIST_BIN_DIR/platforms
rsync -av --exclude '*.debug' $QT_DIR/plugins/printsupport/ $DIST_BIN_DIR/printsupport

mkdir -p $DIST_BIN_DIR/imageformats
rsync -av --exclude '*.debug' $QT_DIR/plugins/imageformats/libqjpeg.so $DIST_BIN_DIR/imageformats/
rsync -av --exclude '*.debug' $QT_DIR/plugins/imageformats/libqsvg.so $DIST_BIN_DIR/imageformats/

# mkdir -p $DIST_BIN_DIR/sqldrivers
# rsync -av --exclude '*.debug' $QT_DIR/plugins/sqldrivers/libqsqlite.so $DIST_BIN_DIR/sqldrivers/
# rsync -av --exclude '*.debug' $QT_DIR/plugins/sqldrivers/libqsqlpsql.so $DIST_BIN_DIR/sqldrivers/

# mkdir -p $DIST_BIN_DIR/audio
# rsync -av --exclude '*.debug' $QT_DIR/plugins/audio/ $DIST_BIN_DIR/audio/

# mkdir -p $DIST_BIN_DIR/QtQuick/Window.2
# rsync -av --exclude '*.debug' $QT_DIR/qml/QtQuick/Window.2/ $DIST_BIN_DIR/QtQuick/Window.2
# rsync -av --exclude '*.debug' $QT_DIR/qml/QtQuick.2/ $DIST_BIN_DIR/QtQuick.2

# tar -cvzf $WORK_DIR/$DISTRO_NAME.tgz  -C $WORK_DIR ./$DISTRO_NAME

rsync -av $SRC_DIR/appimage/QtMips.AppDir/* $DIST_DIR/

ARCH=x86_64 $APP_IMAGE_TOOL $DIST_DIR $WORK_DIR/$APP_NAME-${APP_VER}-x86_64.AppImage
