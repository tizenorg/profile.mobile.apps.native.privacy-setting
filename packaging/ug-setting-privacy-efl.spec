Name:    ug-setting-privacy-efl
Summary: Privacy setting ui gadget
Version: 1.0.0
Release: 1
Group:   System/Libraries
License: Apache-2.0
Source0: %{name}-%{version}.tar.gz
Source1001: %{name}.manifest
%if "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif
BuildRequires: cmake
BuildRequires: gettext-tools
BuildRequires: edje-tools
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(ui-gadget-1)
BuildRequires: pkgconfig(icu-i18n)
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(security-privilege-manager)
BuildRequires: pkgconfig(security-manager)
BuildRequires: pkgconfig(pkgmgr-info)

%description
Privacy setting ui gadget

%prep
%setup -q
cp -a %SOURCE1001 .

%build
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"

export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"

%ifarch %{ix86}
export CFLAGS="$CFLAGS -DTIZEN_EMULATOR_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_EMULATOR_MODE"
export FFLAGS="$FFLAGS -DTIZEN_EMULATOR_MODE"
%endif

%{!?build_type:%define build_type "Release"}
%cmake . -DCMAKE_INSTALL_PREFIX=%TZ_SYS_RO_UG \
        -DTZ_SYS_RO_PACKAGES=%TZ_SYS_RO_PACKAGES \
        -DCMAKE_BUILD_TYPE=%{build_type} \
        -DVERSION=%version

make %{?_smp_mflags}

%install
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%manifest %name.manifest
%license LICENSE
%TZ_SYS_RO_PACKAGES/%name.xml
%TZ_SYS_RO_UG/lib/libug-setting-privacy-efl.so*
#%TZ_SYS_RO_UG/res/locale/*
