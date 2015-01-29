# - Find FFTW
# Find the native FFTW includes and library
#
#  FFTW_INCLUDES    - where to find fftw3.h
#  FFTW_LIBRARIES   - List of libraries when using FFTW.
#  FFTW_FOUND       - True if FFTW found.

find_path (FFTW_INCLUDE_DIR fftw3.h)
find_library (FFTW_LIBRARIES NAMES fftw3f)

mark_as_advanced (FFTW_LIBRARIES FFTW_INCLUDE_DIR)
