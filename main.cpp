#include <QtWidgets>
#include "MainWindowController.h"
#include "mainwindow.h"
#include "misc.h"
//#include <ctime>

/**
 * @mainpage
 *
 * @todo Subclass @q{core,QFileInfo} and all the classes
 * that use it till you get to your custom FileSystemModel and implement in
 * FileInfo a persistent caching mechanism. d_ptr is protected
 * so you will be able to acces it. The problem with the Qt implementation?
 * @q{core,QFileInfo} fi("A:"); fi.size(); queries the floppy disk and makes a
 * disappointing noise and slows down the application.
 * Windows File Explorer is always open and always monitors the drives so it
 * has an injust advantage which must be compensated by a lot of cool and
 * pleasant work.
 * @note If I see high CPU usage, a possible cause is painting outside the
 * bounding rectangle of an item in GraphView.
 *
 * Documentation notes
 * -------------------
 * - <strong>\@q{module,class}</strong> links to <a
 * href="https://qt-project.org/doc/qt-5.1/">Qt 5.1 reference</a> at the
 * specified class in the specified module. Example: \@q{core,QFileInfo}
 * produces this output: @q{core,QFileInfo}.
 * - Doxygen bug: **\\@** should produce a bold \@.
 * - Doxygen can't document lambdas (this is not that bad).
 * - When I don't want a reference when typing a symbol, I can use \% before the
 * symbol. This will prevent the creation of a reference.
 *
 * Programming tricks
 * ------------------
 * - Unblocks GUI: @q{core,QApplication}::%processEvents().
 *
 * Bugs
 * ----
 * @bug In RootItem::LIST layout,
 *  - Scroll until the first FileNode is partially hidden.
 *  - Resize the MainWindow or the Dock.
 *  - Try to scroll back to see the whole first FileNode.
 *
 */

/**
 * @brief Program entry function.
 * @param argc Number of CLI arguments.
 * @param argv CLI arguments.
 * @return Program return code.
 * @retval 0 succes
 */
int main (int argc, char *argv[])
{
    //qSetMessagePattern("[%{function}:%{line}] %{message}");
    qInstallMessageHandler(MessageHandler::msgHandler);

    QApplication a(argc, argv);
    QFile styleSheetFile(":/style.css");
    QString styleSheet;
    MainWindowController *main_win_ctrl;

    // cross-platform style (used because I develop for many platforms and I
    // don't have time for testing the tiny UI changes I make on all platforms)
    QApplication::setStyle(QStyleFactory::create("Fusion"));

#ifdef  Q_WS_X11
    qDebug() << "X11.";
#else
    qDebug() << "Not X11.";
#endif

    styleSheetFile.open(QFile::ReadOnly);
    styleSheet = styleSheetFile.readAll();
    a.setStyleSheet(styleSheet);

    a.setWindowIcon(QIcon(":/root.png"));

    //qsrand(time(NULL));

    auto args = a.arguments();
    if (args.size() > 1) {
        //! @todo open more than one folders (tabs, please) if size more than 2
        main_win_ctrl = MainWindowController::instance(args[1]);
    } else {
        main_win_ctrl = MainWindowController::instance();
    }
    main_win_ctrl->view->show();

    return a.exec();
}
