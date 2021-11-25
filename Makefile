# Project Name
TARGET = MightySampler

# Sources
CPP_SOURCES = MightySampler.cpp WavStream.cpp Sample.cpp DisplayManager.cpp Patch.cpp

# Library Locations
LIBDAISY_DIR = ../../libdaisy
DAISYSP_DIR = ../../DaisySP

# Includes FatFS source files within project.
USE_FATFS = 1


# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile


