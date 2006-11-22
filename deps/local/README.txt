All packages are installed to this directory.  Nothing in here but this file
should be checked into SVN.  On windows unpack dependencies here, on Linux/Mac
just run 'source scripts/setenv; build-deps' to compile all dependency packages.  After that there will be a <name>.depend file for every package that successfully built, that just tells build-deps not to build the package again.
