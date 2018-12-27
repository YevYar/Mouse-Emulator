#ifndef KEEPER_H
#define KEEPER_H

/***************************************
 * Этот класс необходим для сохранения настроек или загрузки их из файла.
 * Также он отвечает за добавление/удаление программы в/из автозапуск(а) и работу с ярлыком программы.
 * Настройки (если они есть) хранятся в файле settings.json.
 * Данный файл создается при первом изменении пользователем настроек.
 * При его повреждении или отсутствии настройки будут сконфигурированы автоматически на основе заранее запрограммированых настроек.
 * Автоматическая генерация настроек происходит в классе KeyBoardHooker.
 * Файл может быть удален программно, если пользователь нажал на кнопку сброса настроек к стандартным настройкам.
 * ************************************/

#include <QObject>
using WORD = unsigned short;
using LONG = long;
using HRESULT = LONG;

class Keeper : public QObject
{
    Q_OBJECT

    QString settingDirPath;
    QString settingFilePath;

public:
    Keeper(QObject *parent = nullptr);
    QVector<int> *loadSettings(QMap<QString, unsigned int> &settings);
    int saveSettings(const QMap<QString, unsigned int> &settings);

    // ярлык нужен для того, чтобы можно было запустить программу по нажатии комбинации клавиш
    static HRESULT changeLnk(WORD); // создает ярлык с заданными настройками  в папке <USERPROFILE>/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/
    static int checkAppInAutorun();
    static void addToAutorun();
    static void removeFromAutorun();
    void removeSettingsFile();

};

#endif // KEEPER_H
