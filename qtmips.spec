#
# spec file for package qtmips
#
# -- Copyright omitted --

Name:           qtmips


Version:        0.6.6


Release:        0 


License:        GPL-2.0+


Group:          Emulators


Summary:        MIPS CPU simulator for education purposes with pipeline and cache visualization.


Url:            https://github.com/cvut/QtMips/


Source:         qtmips-%{version}.tar.gz 

%if 0%{?suse_version}
BuildRequires: libqt5-qtbase-devel
BuildRequires: libelf-devel
%endif

%if 0%{?fedora} || 0%{?centos_version} || 0%{?rhel_version} || 0%{?scientificlinux_version}
BuildRequires: gcc-c++
BuildRequires: qt5-qtbase-devel >= 5.3
BuildRequires: elfutils-libelf-devel
Requires: qt5-qtbase
%endif


# Qt devel packages
%define  qmake  /usr/bin/qmake-qt5
BuildRequires:  cmake 


BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description 

MIPS CPU simulator for education purposes with pipeline and cache visualization.

%prep 


%setup -q -n %{name}-%{version}

%build 
%{qmake} "CONFIG+=release" "CONFIG+=force_debug_info"

make


%install 
mkdir -p %{buildroot}/%{_bindir}
install -m755 qtmips_gui/qtmips_gui %{buildroot}/%{_bindir}
install -m755 qtmips_cli/qtmips_cli %{buildroot}/%{_bindir}

%files 
%{_bindir}/qtmips_gui
%{_bindir}/qtmips_cli
%doc README.md LICENSE

%changelog 
* Thu 21 Mar 2019 Pavel Pisa <pisa@cmp.felk.cvut.cz - 1.0-1
- Initial attemp to package QtMips by Open Build Service.
