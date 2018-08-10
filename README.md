# netlib

The netlib is an object oriented platform independend interface to the C socket API.

**The current version of netlib is still a work in progress, and is not tested. Most functionality regarding IPv4 should work already though.**

## Cloning

Navigate to the directory you want to have the `netlib` folder in. Now execute:

	git clone https://github.com/RmbRT/netlib.git

## Installing / Compiling

Make sure you have CMake installed. Now navigate into the `netlib` directory and execute:

	cmake .

This generates project / make files (depending on your machine configuration). Use these to compile the library.
It also downloads any dependencies and creates an include directory that can be used to import netlib into other projects.

## Documentation

Makes sure to have doxygen installed, and navigate to the `netlib` directory, and execute:

	doxygen Doxyfile

This will generate a documentation in `netlib/html`. This file has been set as main page of the documentation. More documentation can be found in `netlib/docs/`.

### Existing documentation pages

For a better experience, generate documentation using doxygen, so that references to code entities will be converted to hyperlinks.

* [Beginner's information](docs/Guide.md)

## License

A copy of the license text the netlib is released under can be found in the text file `netlib/LICENSE`.
