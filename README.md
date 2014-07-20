qt-file-manager
===============

A modern file manager based on C++/Qt.


Build instructions
------------------

### On Windows

In `qt-file-manager/qt-file-manager.pro` file, you should replace "G:\Biblioteci software\boost_1_55_0" with the path to the downloaded Boost directory which contains the "boost" directory. "G:\Proiecte\qt-file-manager\qt-file-manager" should be replaced with the absolute path to the `qt-file-manager` directory in the repository. But please, do not commit these replacements. I did not find a way to stop using these absolute paths. There are also two comments in `qt-file-manager.pro` talking about these two needed absolute paths.

Build the project in Qt Creator with the MSVC compiler (Qt 5.2 supports only MSVC2012, Qt 5.3 supports MSVC2013). Also read about this bug and how you can avoid it (build in Release mode if you want to see the program's UI): https://github.com/silviubogan/qt-file-manager/issues/3 .

### On Linux/Ubuntu

The project uses Boost Graph Libraries from the Boost Libraries. I tested the program with Boost Libraries 1.54 and 1.55. On Ubuntu, to install the Boost Graph Libraries install the package `libboost-graph-dev` by running the command `sudo apt-get install libboost-graph-dev` in the terminal. Then you can build the project with GCC or another compiler, but I can only guarantee that it works with GCC because this is what I currently use.


Notes for developers
--------------------

- The project uses [Doxygen](http://www.stack.nl/~dimitri/doxygen/) comments so you can generate a useful documentation in many formats for the C++ code. This will surely help you understand the code. On Ubuntu you can install Doxygen using the command `sudo apt-get install doxygen doxygen-gui` then you can create a configuration file and generate the documentation using the program opened by the command `doxywizard`.
- **flag_in_hole** image, used as the root icon in the breadcrumb, is took from here: http://www.fileformat.info/info/unicode/char/26f3/browsertest.htm and modified to be blue (before, it was black) and then resized to be easily observed in the button.
- **13.04.2013**: On Windows I didn't use MinGW because it didn't support the Windows API well enough for the necessities of the project. Not even the idea of building the source code of the last version, even unstable, of w32api, from the SourceForge page of the MinGW project doesn't work, because it doesn't contain the complete header files *shlobj.h* and *shobjidl.h*. What they miss, more precisely, at the moment when I analyzed them was the interface *IFileOperation*.
