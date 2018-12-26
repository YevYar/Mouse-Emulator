#ifndef KEEPER_H
#define KEEPER_H

/***************************************
 * Этот класс необходим для сохранения настроек или загрузки их из файла.
 * Настройки (если они есть) хранятся в файле settings.json.
 * Данный файл создается при первом изменении пользователем настроек.
 * При его повреждении или отсутствии настройки будут сконфигурированы автоматически на основе заранее запрограммированых настроек.
 * Автоматическая генерация настроек происходит в классе KeyBoardHooker.
 * Файл может быть удален программно, если пользователь нажал на кнопку сброса настроек к стандартным настройкам.
 * ************************************/

#include <QObject>

class Keeper : public QObject
{
    Q_OBJECT

public:
    Keeper(QObject *parent = nullptr);
    QVector<int> *loadSettings(QMap<QString, unsigned int> &settings);
    int saveSettings(const QMap<QString, unsigned int> &settings);
    void removeSettingsFile();

};

#endif // KEEPER_H
