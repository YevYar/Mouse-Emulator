#include "keeper.h"
#include "keyboardhooker.h"
#include "mainwindow.h"
#include "windows.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow *w;
    w = new MainWindow();
    QObject::connect(qApp, &QCoreApplication::aboutToQuit, w, [w] () {w->allowClose(true); w->close(); delete w;});
    w->show();

    KeyBoardHooker::instance();
    KeyBoardHooker::setParent(w);

    // конфигурируем настройки, считав сперва настройки из файла и получив код, указывающий на кол-во правильно считанных настроек
    KeyBoardHooker::configureSettings( w->load(KeyBoardHooker::getSettings()) );
    w->displaySettings(KeyBoardHooker::getSettings()); // отображаем настройки на форме

    // добавление программы в автозапуск каждый раз при запуске программы, если соответствующие настройки установлены
    if(Keeper::checkAppInAutorun() == -2 && static_cast<Qt::CheckState>(KeyBoardHooker::getSettings()->value("autorun")) == Qt::Checked)
        Keeper::addToAutorun();

    // создание ярлыка программы в небходимой папке происходит каждый раз при запуске программы с указанными клавишами или без, если данная настройка отключена
    // в любом случае, ярлык создается каждый раз при запуске программы
    WORD wHotKey = MAKEWORD(KeyBoardHooker::getSettings()->value("another key state"), KeyBoardHooker::getSettings()->value("Ctrl state") | KeyBoardHooker::getSettings()->value("Alt state"));
    Keeper::changeLnk(wHotKey);

    return a.exec();
}
