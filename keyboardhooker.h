#ifndef KEYBOARDHOOKER_H
#define KEYBOARDHOOKER_H

/***************************************
 * Этот класс выполняет основные функции программы - ловит события нажатия клавиш и выполняет необходимые функции.
 * В данном классе реализован петтерн "синглетон".
 * Метод класса, реализующий основную логику работы данного класса и программы в целом, - keyboardHookProc.
 * ************************************/

#include "windows.h"
#include <QObject>

class MainWindow;

class KeyBoardHooker : public QObject
{
    Q_OBJECT

    HINSTANCE hInstance;
    MainWindow *parent;
    static HHOOK keyboardHook;
    static QMap<QString, unsigned int> settings; // settingMap = 0 (SM0) - содержит настройки программы, полученные из файла или программно сгенерированные
    static QMap<QString, unsigned int> tempSettings; // settingMap = 1 (SM1) - содержит настройки, которые были изменены, но еще не сохранены

public:
    KeyBoardHooker(QObject *parent = nullptr);
    static bool isContainKey(unsigned int key, bool settingMap = 0); // проверка наличия в выбранном QMap указанного кода клавиши
    static bool isSM0ContainKeyWithoutCrossing(unsigned int key); // метод для проверки наличия кода клавиши в SM0 с обязательным отсутствием её ключа в SM1
    static bool setTempSetting(QString  key, unsigned int value);
    static KeyBoardHooker &instance(); // возвращает объект этого класса. Реализует паттерн синглетон
    static LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam); // основной метод (обрабатывает хук), получает управление при появлении в системе события от клавиатуры
    static QMap<QString, unsigned int> *getSettings();
    static QMap<QString, unsigned int> *getTempSettings();
    static QString getKeyNameByVirtualKey(unsigned int); // возвращает имя клавиши, которое соответствует переданному коду клавиши
    static QString getSettingNameByKeyName(QString, bool settingMap = 0); // возвращает название настройки (или пустую строку) из нужного SM по коду клавиши
    static void configureSettings(QVector<int> *); // генерирует SM0, получая на входе данные о несчитанных из файла настройках и генерирует их
    static void setNewKeyValue(QString  key, unsigned int value);
    static void unhookExit(); // снимает хук и завершает работу программы
    void setParent(MainWindow *);

};

#endif // KEYBOARDHOOKER_H
