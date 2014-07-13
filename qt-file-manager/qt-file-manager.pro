QT += core widgets
win32 {
    # For qDebug to work with cmd.exe. On Linux I don't need this.
    #debug: QT += console # qmake error "Project ERROR: Unknown module(s) in QT: console" on Qt 5.3
}
TARGET = qt-file-manager
TEMPLATE = app

# CONFIG += c++11 is a replacement for (at least):
#QMAKE_CXXFLAGS += "-std=c++11"
CONFIG += precompile_header c++11

SOURCES += main.cpp \
    misc.cpp \
    #views/_quickfilegraphview.cpp \
    views/ViewScrollArea.cpp \
    views/ViewWidget.cpp \
    views/ListViewItem.cpp \
    views/View.cpp \
    controllers/DirController.cpp \
    #_filesystemmodel.cpp \
    #views/_listview.cpp
    #views/_GraphViewWidget.cpp
    views/RootItem.cpp \
    views/ZoneItem.cpp \
    views/WorkspaceView.cpp \
    Application.cpp \
    FileInfo.cpp \
    views/FileNode.cpp \
    AsyncFileOperation.cpp \
    AsyncFileOperationWorker.cpp \
    controllers/DockController.cpp \
    models/DockModel.cpp \
    models/DirModel.cpp \
    views/Breadcrumb.cpp \
    views/Dock.cpp \
    views/LocationEdit.cpp \
    views/MainWindow.cpp \
    controllers/MainWindowController.cpp \
    views/SearchLineEdit.cpp \
    views/TabBarItem.cpp \
    views/TabLabelItem.cpp \
    BoostGraph.cpp \
    controllers/RootItemController.cpp \
    controllers/WorkspaceController.cpp \
    models/WorkspaceModel.cpp \
    views/FileOperationItem.cpp \
    views/FileOperationsMenu.cpp \
    models/FileOperationData.cpp \
    models/ViewSelectionModel.cpp

PRECOMPILED_HEADER = stable.h

# Otherwise, on clang, PRECOMPILED_HEADER is in conflict with TARGET.
PRECOMPILED_DIR = pch

HEADERS += \
    misc.h \
    stable.h \
    #views/_quickfilegraphview.h \
    views/ViewScrollArea.h \
    views/ViewWidget.h \
    views/ListViewItem.h \
    views/View.h \
    #_filesystemmodel.h \
    #views/_listview.h
    #views/_GraphViewWidget.h
    views/RootItem.h \
    views/ZoneItem.h \
    views/WorkspaceView.h \
    FileInfo.h \
    AsyncFileOperation.h \
    AsyncFileOperationWorker.h \
    views/FileNode.h \
    controllers/DirController.h \
    models/DirModel.h \
    BoostGraph.h \
    controllers/RootItemController.h \
    controllers/WorkspaceController.h \
    models/WorkspaceModel.h \
    views/Breadcrumb.h \
    views/Dock.h \
    controllers/DockController.h \
    models/DockModel.h \
    views/LocationEdit.h \
    views/MainWindow.h \
    controllers/MainWindowController.h \
    views/SearchLineEdit.h \
    views/TabBarItem.h \
    views/TabLabelItem.h \
    Application.h \
    views/FileOperationsMenu.h \
    views/FileOperationItem.h \
    models/FileOperationData.h \
    models/ViewSelectionModel.h

OTHER_FILES += \
    readme.txt \
#    views/filegraph.qml \
    style.css

RESOURCES += \
    resources.qrc

win32 {
    LIBS += -lshell32 -lole32 -lcomsuppwd
    RC_ICONS += \
        root.ico
    # on win32, boost is not by default in the include paths.
    # In any case, I should move it near the project directory.
    INCLUDEPATH += "G:\Biblioteci software\boost_1_55_0"
    # Including files from main dir, in files from subdirs causes error.
    INCLUDEPATH += "G:\Proiecte\qt-file-manager\qt-file-manager"
}

linux {
    # linux-only because MSVC doesn't recognise -Werror
    QMAKE_CXXFLAGS += "-Werror"

    # On Windows I handle the Trash concept in another place.
    #
    # In the future I will merge the Win implementation of
    # trash/recycle functionality in these files.
    SOURCES += \
        models/TrashModel.cpp
    HEADERS += \
        models/TrashModel.h
}

# On linux-clang -pipe is enabled by default.
# I am not sure if MSVC supports this.
#QMAKE_CXXFLAGS += "-pipe"
