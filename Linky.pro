QT += core widgets
win32 { # for qDebug to work with cmd.exe
    debug: QT += console
}
TARGET = linky
CONFIG += precompile_header c++11
TEMPLATE = app

SOURCES += main.cpp \
    misc.cpp \
    #view/_quickfilegraphview.cpp \
    view/ViewScrollArea.cpp \
    view/ViewWidget.cpp \
    view/ListViewItem.cpp \
    view/View.cpp \
    view/ViewSelectionModel.cpp \
    view/DirModel.cpp \
    Dock.cpp \
    view/DirController.cpp \
    SearchLineEdit.cpp \
    #_filesystemmodel.cpp \
    #view/_listview.cpp
    #view/_GraphViewWidget.cpp
    DockModel.cpp \
    view/RootItem.cpp \
    view/ZoneItem.cpp \
    view/BoostGraph.cpp \
    TabBarItem.cpp \
    TabLabelItem.cpp \
    view/WorkspaceView.cpp \
    view/WorkspaceController.cpp \
    MainWindowController.cpp \
    Application.cpp \
    DockController.cpp \
    MainWindow.cpp \
    FileInfo.cpp \
    view/FileNode.cpp \
    AsyncFileOperation.cpp \
    AsyncFileOperationWorker.cpp \
    Breadcrumb.cpp \
    FileOperationData.cpp \
    FileOperationItem.cpp \
    FileOperationsMenu.cpp \
    LocationEdit.cpp \
    view/WorkspaceModel.cpp \
    view/RootItemController.cpp

PRECOMPILED_DIR = pch # otherwise, with clang: conflict with TARGET
PRECOMPILED_HEADER = stable.h

HEADERS += \
    misc.h \
    stable.h \
    #view/_quickfilegraphview.h \
    view/ViewScrollArea.h \
    view/ViewWidget.h \
    view/ListViewItem.h \
    view/View.h \
    view/ViewSelectionModel.h \
    view/DirModel.h \
    Dock.h \
    view/DirController.h \
    SearchLineEdit.h \
    #_filesystemmodel.h \
    #view/_listview.h
    #view/_GraphViewWidget.h
    DockModel.h \
    view/RootItem.h \
    view/ZoneItem.h \
    view/BoostGraph.h \
    TabBarItem.h \
    TabLabelItem.h \
    view/WorkspaceView.h \
    view/WorkspaceController.h \
    MainWindowController.h \
    Application.h \
    DockController.h \
    MainWindow.h \
    FileInfo.h \
    AsyncFileOperation.h \
    AsyncFileOperationWorker.h \
    Breadcrumb.h \
    FileOperationData.h \
    FileOperationItem.h \
    FileOperationsMenu.h \
    LocationEdit.h \
    view/FileNode.h \
    view/WorkspaceModel.h \
    view/RootItemController.h

# FORMS =

linux {
    # on windows I handle the trash in other place
    # I am sure I will merge Win & Linux implementations
    # of trash/recycle functionality in this file, in future
    SOURCES += TrashModel.cpp
    HEADERS += TrashModel.h
}

OTHER_FILES += \
    readme.txt \
#    view/filegraph.qml \
    style.css

RESOURCES += \
    resources.qrc

win32 {
    LIBS += -lshell32 -lole32 -lcomsuppwd
    RC_ICONS += \
        root.ico
    # on win32, boost is not by default in the include paths
    INCLUDEPATH += "C:\Users\Silviu\Downloads\boost_1_54_0\boost_1_54_0"
    # from folder view including a file from Linky folder => error...
    INCLUDEPATH += "D:\infoedu\Linky"
}

#QMAKE_CXXFLAGS += "-std=c++11 -Werror"
linux { # MSVC doesn't recognise this...
    QMAKE_CXXFLAGS += "-Werror"
}
linux { # I am not sure if MSVC supports this
    #QMAKE_CXXFLAGS += "-pipe"
    # on linux-clang it's the default.. so I commented it
}

# this script must be rewritten from scratch to use clang lib
# only then it will be truly useful:
#linux {
    #QMAKE_POST_LINK = (cd /home/penguin/qtvis && ./dotty.sh)
#}
