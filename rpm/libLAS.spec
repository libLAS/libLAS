Name:           libLAS
Version:        1.6.0
Release:        6%{?dist}
Summary:        Library and tools for the LAS LiDAR format

Group:          System Environment/Libraries
License:        BSD style and Boost
URL:            http://liblas.org/
#
#  The next release will be similar to:
#    http://download.osgeo.org/liblas/libLAS-1.6.0b3.tar.gz
#
#  For now, we use a snapshot generated on:
#    Thu Dec 30 01:54:04 UTC 2010
#  using:
#    hg hg clone http://hg.liblas.org/main libLAS
#    cd libLAS
#    hg archive ../libLAS-2657:06795bfa172d.tar.gz
#
#  and the above was done to obtain shared-library versioning.
#
Source0:        libLAS-2657:06795bfa172d.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  boost-devel, cmake
BuildRequires:  libxml2-devel, libgeotiff-devel
# Requires:     

%description
libLAS is a C/C++ library for reading and writing the very common LAS LiDAR
format.  The ASPRS LAS format is a sequential binary format used to store data
from LiDAR sensors and by LiDAR processing software for data interchange and
archival storage.

%package        devel
Summary:        Development files for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description    devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.


%prep
%setup -q -n 'libLAS-2657:06795bfa172d'


%build
#
#  Note: -DWITH_GDAL:BOOL=ON is currently disabled since
#    libLAS requires a not-yet-released GDAL 1.8.x version.
#
%cmake -DWITH_LIBXML2:BOOL=ON -DWITH_GEOTIFF:BOOL=ON  \
    -DGEOTIFF_INCLUDE_DIR:PATH="/usr/include/libgeotiff"  \
    -DWITH_GDAL:BOOL=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo  \
    .
make VERBOSE=1 %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'
#
#  The following triggers an error in rpmlint but I can think of no 
#  better way to fix it -- improvements are welcome!!!
#
%if "%{?_lib}" == "lib64"
  mv -f $RPM_BUILD_ROOT/usr/lib $RPM_BUILD_ROOT/usr/lib64
%endif
rm -rf $RPM_BUILD_ROOT/usr/share
chmod 755 $RPM_BUILD_ROOT/usr/bin/*

%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%doc AUTHORS COPYING LICENSE.txt README.txt
%{_bindir}/las2las
%{_bindir}/las2las-old
%{_bindir}/las2txt
%{_bindir}/las2txt-old
%{_bindir}/lasblock
%{_bindir}/lasinfo
%{_bindir}/lasinfo-old
%{_bindir}/lasmerge
%{_bindir}/ts2las
%{_bindir}/txt2las
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root,-)
# %doc 
%{_bindir}/liblas-config
%{_includedir}/*
%{_libdir}/*.so


%changelog
* Wed Dec 29 2010 Ed Hill <ed@eh3.com> - 1.6.0-6
- put liblas-config in the -devel package, fix permissions, and
  add RelWithDebInfo

* Wed Dec 29 2010 Ed Hill <ed@eh3.com> - 1.6.0-5
- use a snapshot with soversion changes and add comments

* Wed Dec 29 2010 Ed Hill <ed@eh3.com> - 1.6.0b3-4
- add SONAME patch

* Wed Dec 29 2010 Ed Hill <ed@eh3.com> - 1.6.0b3-3
- fix license tag

* Wed Dec 29 2010 Ed Hill <ed@eh3.com> - 1.6.0b3-2
- add comment about WITH_GDAL build errors

* Wed Dec 29 2010 Ed Hill <ed@eh3.com> - 1.6.0b3-1
- shared libraries are unversioned so put in main package for now

* Wed Dec 29 2010 Ed Hill <ed@eh3.com> - 1.6.0b3-0
- initial package creation

