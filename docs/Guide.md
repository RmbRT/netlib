# netlib guide

## prelude

The netlib is an attempt to create a C++ wrapper for the C socket APIs. Not only does it simplify the usage of the socket API, it also removes platform specific differences, so that platform independent socket code can be written comfortably. It does not -- however -- add additional behaviour that would differ from the socket api. So the user should know how the socket api works in order to be able to successfully use the netlib.

## program requirements

Any call to the native socket api requires the socket api to be initialised. This can be achieved by creating an object of the type netlib::Runtime. It is important, that if you create a multi-threaded application, you should have a runtime object in every thread that uses the netlib. You could also create a global / static netlib::Runtime instance, to have the netlib initialised over the whole lifetime of the program. This is pretty much the only requirement set by the netlib, the other requirements come directly from the socket api.