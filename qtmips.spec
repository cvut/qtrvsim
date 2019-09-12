#
# spec file for package qtmips
#
# Copyright (c) 2019 Pavel Pisa <pisa@cmp.felk.cvut.cz>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Please submit bugfixes or comments via
#   https://github.com/cvut/QtMips/
# issues tracker.
#


Name:           qtmips
Version:        0.7.3
Release:        0
Summary:        MIPS CPU simulator for education purposes
License:        GPL-2.0-or-later
Group:          System/Emulators/Other
URL:            https://github.com/cvut/QtMips/
Source:         qtmips_%{version}.tar.xz
BuildRequires:  gcc-c++
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(Qt5PrintSupport)
BuildRequires:  hicolor-icon-theme

%if ! 0%{?suse_version}
BuildRequires:  pkgconfig(libelf)
BuildRequires:  desktop-file-utils
%endif

%if 0%{?suse_version}
BuildRequires:  libelf-devel
BuildRequires:  update-desktop-files
%endif

%if !0%{?suse_version}
%define  qmake5  /usr/bin/qmake-qt5
%endif

%description
MIPS CPU simulator for education purposes with pipeline and cache visualization.

%prep
%setup -q

%build
%qmake5 "CONFIG+=release" \
  CONFIG+="force_debug_info" \
  QMAKE_CFLAGS+="%optflags" \
  QMAKE_CXXFLAGS+="%optflags" \
  QMAKE_STRIP="/bin/true"
#make %{?_smp_mflags} - do not use SMP for now, there can be problem with generated makefiles
#%make_build          - fails on openSUSE_Leap_42.3 and openSUSE_Leap_42.3_Ports
make

%install
mkdir -p %{buildroot}/%{_bindir}
install -m755 qtmips_gui/qtmips_gui %{buildroot}/%{_bindir}
install -m755 qtmips_cli/qtmips_cli %{buildroot}/%{_bindir}
mkdir -p %{buildroot}/%{_datadir}/icons/hicolor/48x48/apps
install -m644 data/icons/qtmips_gui.png %{buildroot}/%{_datadir}/icons/hicolor/48x48/apps
mkdir -p %{buildroot}/%{_datadir}/icons/hicolor/scalable/apps
install -m644 data/icons/qtmips_gui.svg %{buildroot}/%{_datadir}/icons/hicolor/scalable/apps

#desktop icon
%if 0%{?suse_version}
install -m755 -d %{buildroot}%{_datadir}/applications
install -m644 data/qtmips.desktop %{buildroot}%{_datadir}/applications
%suse_update_desktop_file -r -i qtmips 'System Emulator'
%endif

%if 0%{?fedora} || 0%{?rhel} || 0%{?centos}
desktop-file-install --dir=%{buildroot}%{_datadir}/applications data/qtmips.desktop
desktop-file-validate %{buildroot}%{_datadir}/applications/qtmips.desktop
%endif

%files
%{_bindir}/qtmips_gui
%{_bindir}/qtmips_cli
%{_datadir}/applications/qtmips.desktop
%{_datadir}/icons/hicolor/48x48/apps/qtmips_gui.png
%{_datadir}/icons/hicolor/scalable/apps/qtmips_gui.svg
%license LICENSE
%doc README.md

%changelog
