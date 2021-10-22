## CS-364 Reason Advanced Makefile

# How to use this Makefile...
###################
###################
##               ##
##  $ make help  ##
##               ##
###################
###################

# IMPORTANT NOTES:
#   1. Set EXECUTABLE to the command name from the project specification.
#   2. To enable automatic creation of unit test rules, your program logic
#      (where entry-point is) should be in a file named EXECUTABLE.re where
#      EXECUTABLE is filename portion of that variable.
#   3. Files you want to include in your final submission cannot match the
#      test*.re pattern.

#######################
# TODO (begin) #
#######################

# Change IDENTIFIER to match the project identifier given in the project spec.
IDENTIFIER  = 139ba267302516bcc9e9e23141d94ac04cd56a91

# Change 'executable' to match the command name given in the project spec.
EXECUTABLE  = interpreter.exe

#######################
# TODO (end) #
#######################

# list of test drivers (with main()) for development
TESTSOURCES = $(wildcard test*.re) parsetab.py
# names of test executables
TESTS       = $(TESTSOURCES:%.re=%)

# list of sources used in project
SOURCES     = $(wildcard *.re)
SOURCES     := $(filter-out $(TESTSOURCES), $(SOURCES))

# name of the tarball created for submission
FULL_SUBMITFILE = fullsubmit.tar.gz
PARTIAL_SUBMITFILE = partialsubmit.tar.gz

release: $(EXECUTABLE)

# make identifier - will check to ensure that all source code and header files
#                   include the project identifier; skip subdirectories;
#                   also removes old submit tarballs, they are outdated
identifier:
	@if [ $$(grep --include=*.{h,hpp,c,cpp,re} --exclude=xcode_redirect.hpp --directories=skip -L $(IDENTIFIER) * | wc -l) -ne 0 ]; then \
		printf "Missing project identifier in file(s): "; \
		echo `grep --include=*.{h,hpp,c,cpp,re} --directories=skip -L $(IDENTIFIER) *`; \
		rm -f $(PARTIAL_SUBMITFILE) $(FULL_SUBMITFILE); \
		exit 1; \
	fi

all: release

$(EXECUTABLE): $(SOURCES)
ifeq ($(EXECUTABLE), executable.exe)
	@echo Edit EXECUTABLE variable in Makefile.
	@exit 1;
else
	dune build $(EXECUTABLE)
endif

clean:
	dune clean
	rm -f $(PARTIAL_SUBMITFILE) $(FULL_SUBMITFILE)


# get a list of all files that might be included in a submit
# different submit types can do additional filtering to remove unwanted files
FULL_SUBMITFILES=$(filter-out $(TESTSOURCES), \
                   $(wildcard Makefile *.re *.py *.cl *.cl-input *.java *.flex *.js test*.txt dune readme.txt references.txt team.txt))

# make fullsubmit.tar.gz - cleans, runs dos2unix, creates tarball
# including test files
$(FULL_SUBMITFILE): $(FULL_SUBMITFILES)
	rm -f $(PARTIAL_SUBMITFILE) $(FULL_SUBMITFILE)
	COPYFILE_DISABLE=true tar -vczf $(FULL_SUBMITFILE) $(FULL_SUBMITFILES)
	@echo !!! Final submission prepared, test files included... READY FOR GRADING !!!

# make partialsubmit.tar.gz - cleans, creates tarball
# omitting test files
PARTIAL_SUBMITFILES=$(filter-out $(wildcard test*.txt), $(FULL_SUBMITFILES))
$(PARTIAL_SUBMITFILE): $(PARTIAL_SUBMITFILES)
	rm -f $(PARTIAL_SUBMITFILE) $(FULL_SUBMITFILE)
	COPYFILE_DISABLE=true tar -vczf $(PARTIAL_SUBMITFILE) \
      $(PARTIAL_SUBMITFILES)
	@echo !!! WARNING: No test files included. Use 'make fullsubmit' to include test files. !!!

# shortcut for make submit tarballs
fullsubmit: identifier $(FULL_SUBMITFILE)
partialsubmit: identifier $(PARTIAL_SUBMITFILE)

define MAKEFILE_HELP
CS-364 Reason Advanced Makefile Help
* This Makefile uses advanced techniques, for more information:
    $$ man make

* General usage
    1. Follow directions at each "TODO" in this file.
       a. Set EXECUTABLE equal to the name from the project specification.
	   b. Set the IDENTIFIER equal to the string from the project specification.
    2. Build, test, submit... repeat as necessary.

* Preparing submissions
    A) To build 'partialsubmit.tar.gz', a tarball without tests used to
       find buggy solutions in the autograder.

           *** USE THIS ONLY FOR TESTING YOUR SOLUTION! ***

       This is useful for faster autograder runs during development and
       free submissions if the project does not build.
           $$ make partialsubmit
    B) Build 'fullsubmit.tar.gz' a tarball complete with autograder test
       files.

           *** ALWAYS USE THIS FOR FINAL GRADING! ***

       It is also useful when trying to find buggy solutions in the
       autograder.
           $$ make fullsubmit
endef
export MAKEFILE_HELP

help:
	@echo "$$MAKEFILE_HELP"


# these targest do not create any files
.phony: all release clean partialsubmit \
		fullsubmit help identifier

# disable built-in rules
.SUFFIXES: