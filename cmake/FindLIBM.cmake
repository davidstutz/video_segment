# - Find LIBM
# Find the native LIBM includes and library
#
#  LIBM_INCLUDES    - where to find math.h
#  LIBM_LIBRARIES   - List of libraries when using LIBM.
#  LIBM_FOUND       - True if LIBM found.

find_path (LIBM_INCLUDE_DIR math.h)
find_library (LIBM_LIBRARIES NAMES m)

mark_as_advanced (LIBM_LIBRARIES LIBM_INCLUDE_DIR)
