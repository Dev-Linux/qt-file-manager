// C includes

// Windows includes (not using GCC on Linux)
#ifndef __GNUC__

#define NOMINMAX
#include <windows.h>
#include <objbase.h>
#include <shellapi.h>
#include <ShObjIdl.h>
#include <ShlObj.h>
#include <comdef.h>

#endif

// C++ includes
#ifdef __cplusplus

#include <QtCore>
#include <QtWidgets>

// http://qt-project.org/forums/viewthread/22993/
// https://bugreports.qt-project.org/browse/QTBUG-29331
// http://stackoverflow.com/questions/15056834/qt5-migration-and-boost-if-hpp-macro-argument-mismatch-bug
#ifndef Q_MOC_RUN

#include <boost/property_map/property_map.hpp>
#include <boost/config.hpp>

#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/point_traits.hpp>
#include <boost/graph/gursoy_atun_layout.hpp>

#endif // Q_MOC_RUN

#endif
