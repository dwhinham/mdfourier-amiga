# CMake script to generate embedded version information from git description
# Copyright (C) 2020 Dale Whinham <daleyo@gmail.com>

# Generate version information - requires a git tag to be present in the format "vX.Y"
execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --dirty
	OUTPUT_VARIABLE GIT_DESCRIPTION
	RESULT_VARIABLE RESULT
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(RESULT EQUAL 0)
	execute_process(COMMAND ${GIT_EXECUTABLE} show -s --format=%ad --date=format:%-d.%-m.%y
		OUTPUT_VARIABLE GIT_DATE
		RESULT_VARIABLE RESULT
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	# Split vX.Y into separate version and revision variables
	string(REGEX REPLACE "^v([0-9]).([0-9]).*$" "\\1;\\2" GIT_FULL_VERSION ${GIT_DESCRIPTION})
	list(GET GIT_FULL_VERSION 0 GIT_VERSION)
	list(GET GIT_FULL_VERSION 1 GIT_REVISION)
else()
	# Fall back on v0.0 and current date
	execute_process(COMMAND date +%-d.%-m.%y
		OUTPUT_VARIABLE GIT_DATE
		RESULT_VARIABLE RESULT
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	set(GIT_VERSION 0)
	set(GIT_REVISION 0)
	set(GIT_FORMATTED_DATE)
	set(GIT_DESCRIPTION "<unknown version>")
endif()

# Generate version.c with correct version info
configure_file(../src/version.c.in version.c)
