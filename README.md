qt-file-manager
===============

A modern file manager based on C++/Qt.

Notes for developers
--------------------

In `qt-file-manager/qt-file-manager.pro` file, you should replace "G:\Biblioteci software\boost_1_55_0" with the path to the downloaded Boost directory which contains the "boost" directory. "G:\Proiecte\qt-file-manager\qt-file-manager" should be replaced with the absolute path to the `qt-file-manager` directory in the repository. But please, do not commit these replacements. I did not find a way to stop using these absolute paths. There are also two comments in `qt-file-manager.pro` talking about these two needed absolute paths.

**flag_in_hole** image, used as the root icon in the breadcrumb, is took from
here: http://www.fileformat.info/info/unicode/char/26f3/browsertest.htm and
modified to be blue (before, it was black) and then resized to be easily
observed in the button.

**13.04.2013**:

On Windows I didn't use MinGW because it didn't support the Windows API well
enough for the necessities of the project. Not even the idea of building the
source code of the last version, even unstable, of w32api, from the SourceForge
page of the MinGW project doesn't work, because it doesn't contain the complete
header files *shlobj.h* and *shobjidl.h*. What they miss, more precisely, at
the moment when I analyzed them was the interface *IFileOperation*.
