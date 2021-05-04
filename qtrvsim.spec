#
# spec file for package qtrvsim
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
#   https://github.com/cvut/qtrvsim/
# issues tracker.
#


Name:           qtrvsim
Version:        0.8.0
Release:        0
Summary:        RISC-V CPU simulator for education purposes
License:        GPL-2.0-or-later
Group:          System/Emulators/Other
URL:            https://github.com/cvut/qtrvsim/
Source:         qtrvsim_%{version}.tar.xz
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

%description
RISC-V CPU simulator for education purposes with pipeline and cache visualization.

%prep
%setup -q

%build
%cmake
%cmake_build

%install
%cmake_install

#desktop icon
%if 0%{?suse_version}
install -m755 -d %{buildroot}%{_datadir}/applications
install -m644 data/qtrvsim.desktop %{buildroot}%{_datadir}/applications
%suse_update_desktop_file -r -i qtrvsim 'System Emulator'
%endif

%if 0%{?fedora} || 0%{?rhel} || 0%{?centos}
desktop-file-install --dir=%{buildroot}%{_datadir}/applications data/qtrvsim.desktop
desktop-file-validate %{buildroot}%{_datadir}/applications/qtrvsim.desktop
%endif

%files
%{_bindir}/qtrvsim_gui
%{_bindir}/qtrvsim_cli
%{_datadir}/applications/qtrvsim.desktop
%{_datadir}/icons/hicolor/48x48/apps/qtrvsim_gui.png
%{_datadir}/icons/hicolor/scalable/apps/qtrvsim_gui.svg
%license LICENSE
%doc README.md

%changelog
