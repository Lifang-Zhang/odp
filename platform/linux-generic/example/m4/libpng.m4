############################################################################
# If png is available and its version is 1.6.0 onward, set have_png to true
############################################################################
LIBPNG_REQUIRED_VERSION=1.6.0
png_version_satisfied=no
png_available=no

PKG_CHECK_MODULES([LIBPNG], [libpng >= $LIBPNG_REQUIRED_VERSION],
                  [png_version_satisfied=yes],
                  [AC_MSG_NOTICE([libpng >= $LIBPNG_REQUIRED_VERSION not found])])

if test x$png_version_satisfied = xyes; then
        AC_CHECK_HEADERS([png.h],
                         [AC_CHECK_LIB(png, png_create_read_struct, [png_available=yes], [], [])],
                         [])

fi

AC_CONFIG_COMMANDS_PRE([dnl
AM_CONDITIONAL([have_png], [test x$png_available = xyes])
])
