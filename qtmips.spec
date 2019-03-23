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
Version:        0.6.6
Release:        0
Summary:        MIPS CPU simulator for education purposes with pipeline and cache visualization
License:        GPL-2.0-or-later
Group:          System/Emulators/Other
URL:            https://github.com/cvut/QtMips/
Source:         qtmips-%{version}.tar.gz
BuildRequires:  gcc-c++
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5Test)

%if ! 0%{?suse_version}
BuildRequires:  pkgconfig(libelf)
%endif

%if 0%{?suse_version}
BuildRequires: libelf-devel
%endif

%if !0%{?suse_version}
%define  qmake5  /usr/bin/qmake-qt5
%endif

%description
MIPS CPU simulator for education purposes with pipeline and cache visualization.

%prep
%setup -q

%build
%qmake5 "CONFIG+=release" "CONFIG+=force_debug_info"
#make %{?_smp_mflags} - do not use SMP for now, there can be problem with generated makefiles
make


%install
mkdir -p %{buildroot}/%{_bindir}
install -m755 qtmips_gui/qtmips_gui %{buildroot}/%{_bindir}
install -m755 qtmips_cli/qtmips_cli %{buildroot}/%{_bindir}

%files
%{_bindir}/qtmips_gui
%{_bindir}/qtmips_cli
%license LICENSE
%doc README.md

%changelog
