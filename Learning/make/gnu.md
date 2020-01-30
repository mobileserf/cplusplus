https://thoughtbot.com/blog/the-magic-behind-configure-make-make-install

**MAKE**

* Phony Targets (or Artificial Targets)

   - Phony target is always out-of-date and its command will be run. The standard phony targets are: all, clean, install.
* Variables

   - A variable begins with a $ and is enclosed within parentheses (...) or braces {...}. Single character variables do not need the parentheses. For example, $(CC), $(CC_FLAGS), $@, $^.

* Automatic Variables

  Automatic variables are set by make after a rule is matched. There include:

  * $@: the target filename.
  * $*: the target filename without the file extension.
  * $<: the first prerequisite filename.
  * $^: the filenames of all the prerequisites, separated by spaces, discard duplicates.
  * $+: similar to $^, but includes duplicates.
  * $?: the names of all prerequisites that are newer than the target, separated by spaces.
  
* Virtual Path - VPATH & vpath
  
     VPATH - Search for dependencies and targets from "src" and "include" directories
     vpath - more precise about the file type and its search directory
     e.g
     VPATH = src include
     vpath %.c src
     vpath %.h include
     
 **Configure -automake & auotconfig**
 
  * aclocal - Set up an m4 environment
  * autoconf - Generate configure from configure.ac
  * automake --add-missing  - Generate Makefile.in from Makefile.am
  * ./configure - Generate Makefile from Makefile.in
  * make distcheck - Use Makefile to build and test a tarball to distribute
  
  * Extra: autoreconf -if  # generate the configure script and Makefile.in files from configure.ac
  
  
