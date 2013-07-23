#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QSettings>

class MainWindowController;

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);
    ~Application();
    QSettings *settings;
    static Application *instance();
    
signals:
    
public slots:
    
private:
    static Application *m_instance;
    MainWindowController *m_main_win_ctrl;
};

#endif // APPLICATION_H
