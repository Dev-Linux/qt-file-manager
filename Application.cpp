#include "Application.h"

#include "MainWindowController.h"
#include "mainwindow.h"
//#include <ctime>

/**
 * @class Application
 * @brief The main controller of the application.
 */

Application *Application::m_instance = nullptr;

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv)
{
    m_instance = this;

    settings = new QSettings("settings.ini",
                             QSettings::IniFormat);

    // cross-platform style (used because I develop for many platforms and I
    // don't have time for testing the tiny UI changes I make on all platforms)
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QFile styleSheetFile(":/style.css");
    QString styleSheet;
    styleSheetFile.open(QFile::ReadOnly);
    styleSheet = styleSheetFile.readAll();
    this->setStyleSheet(styleSheet);

    this->setWindowIcon(QIcon(":/root.png"));

    MainWindowController *main_win_ctrl;

    //qsrand(time(NULL));

    auto args = this->arguments();
    if (args.size() > 1) {
        //! @todo open more than one folders (tabs, please) if size more than 2
        main_win_ctrl = MainWindowController::instance(args[1]);
    } else {
        main_win_ctrl = MainWindowController::instance();
    }
    main_win_ctrl->view->show();

#ifdef Q_WS_X11
    qDebug() << "X11.";
#else
    qDebug() << "Not X11.";
#endif
}

Application::~Application()
{
    delete settings;
}

Application *Application::instance()
{
    Q_ASSERT(m_instance != nullptr);
    return m_instance;
}
