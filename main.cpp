#include "Application.h"
#include "misc.h"

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
 * See RootItem::selRectChanged, lines `if (!items.isEmpty())` + 3.
 */

/**
 * @brief Program entry function.
 * @param argc Number of CLI arguments.
 * @param argv CLI arguments.
 * @return Program return code.
 * @retval 0 success
 */
int main(int argc, char *argv[])
{
    //qSetMessagePattern("[%{function}:%{line}] %{message}");
    qInstallMessageHandler(MessageHandler::msgHandler);

    Application app(argc, argv);

    return app.exec();
}
