//
// Created by koncord on 06.01.17.
//

#ifndef NEWLAUNCHER_MAIN_HPP
#define NEWLAUNCHER_MAIN_HPP


#include "ui_Main.h"
#include "ServerModel.hpp"
#include <QSortFilterProxyModel>
#include <components/process/processinvoker.hpp>

class Main : public QMainWindow,  private Ui::MainWindow
{
    Q_OBJECT
public:
    explicit Main(QWidget *parent = 0);
    virtual ~Main();
protected:
protected slots:
    void tabSwitched(int index);
    void addServer();
    void addServerByIP();
    void deleteServer();
    void refresh();
    void play();
    void serverSelected();
private:
    Process::ProcessInvoker *mGameInvoker;
    ServerModel *browser, *favorites;
    QSortFilterProxyModel *proxyModel;
};


#endif //NEWLAUNCHER_MAIN_HPP
