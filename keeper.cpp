#include "keeper.h"
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>

Keeper::Keeper(QObject *parent) : QObject(parent)
{

}

int Keeper::saveSettings(const QMap<QString, unsigned int> &settings)
{
    if(settings.isEmpty())
        return 0; // пустой массив кнопок

    QJsonObject obj;
    obj.insert("speed x", static_cast<int>(settings["speed x"]));
    obj.insert("speed y", static_cast<int>(settings["speed y"]));
    obj.insert("up", static_cast<int>(settings["up"]));
    obj.insert("down", static_cast<int>(settings["down"]));
    obj.insert("right", static_cast<int>(settings["right"]));
    obj.insert("left", static_cast<int>(settings["left"]));
    obj.insert("top-right", static_cast<int>(settings["top-right"]));
    obj.insert("top-left", static_cast<int>(settings["top-left"]));
    obj.insert("down-right", static_cast<int>(settings["down-right"]));
    obj.insert("down-left", static_cast<int>(settings["down-left"]));
    obj.insert("click", static_cast<int>(settings["click"]));
    obj.insert("right click", static_cast<int>(settings["right click"]));
    obj.insert("autorun", static_cast<Qt::CheckState>(settings["autorun"]));
    obj.insert("hot key", static_cast<Qt::CheckState>(settings["hot key"]));
    obj.insert("Ctrl state", static_cast<int>(settings["Ctrl state"]));
    obj.insert("Alt state", static_cast<int>(settings["Alt state"]));
    obj.insert("another key state", static_cast<int>(settings["another key state"]));

    QJsonDocument jsonDoc;
    jsonDoc.setObject(obj);

    QFile file;
    QDir::setCurrent(QApplication::applicationDirPath());
    file.setFileName("settings.json");

    try
    {
        if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
            throw -1;
    }
    catch(int key)
    {
        return key; // ошибка создания файла
    }

    file.write(jsonDoc.toJson());
    file.close();

    /* 100 - это признак удачного завершения функции */
    return 100;
}

int Keeper::loadSettings(QMap<QString, unsigned int> &settings)
{
    QFile file;
    QDir::setCurrent(QApplication::applicationDirPath());
    file.setFileName("settings.json");

    try
    {
        if(!file.open(QIODevice::ReadOnly))
            throw -1;
    }
    catch(int key)
    {
        return key; // ошибка: нет файла
    }


    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if(jsonDoc.isEmpty() || jsonDoc.isNull())
        return 0; // файл пуст или не содержит корректные настройки

    if(jsonDoc.object().isEmpty())
        return 0;

    if(jsonDoc.object().value("speed x") == QJsonValue::Undefined)
        return 0;
    else settings.insert("speed x", static_cast<const unsigned int>(jsonDoc.object().value("speed x").toInt()));

    if(jsonDoc.object().value("speed y") == QJsonValue::Undefined)
        return 1;
    else settings.insert("speed y", static_cast<const unsigned int>(jsonDoc.object().value("speed y").toInt()));

    if(jsonDoc.object().value("up") == QJsonValue::Undefined)
        return 2; // смогли загрузить только 1 настройку в указанном порядке
    else settings.insert("up", static_cast<const unsigned int>(jsonDoc.object().value("up").toInt()));

    if(jsonDoc.object().value("down") == QJsonValue::Undefined)
        return 3; // смогли загрузить только 2 настройку в указанном порядке
    else settings.insert("down", static_cast<const unsigned int>(jsonDoc.object().value("down").toInt()));

    if(jsonDoc.object().value("right") == QJsonValue::Undefined)
        return 4;
    else settings.insert("right", static_cast<const unsigned int>(jsonDoc.object().value("right").toInt()));

    if(jsonDoc.object().value("left") == QJsonValue::Undefined)
        return 5;
    else settings.insert("left", static_cast<const unsigned int>(jsonDoc.object().value("left").toInt()));

    if(jsonDoc.object().value("top-right") == QJsonValue::Undefined)
        return 6;
    else settings.insert("top-right", static_cast<const unsigned int>(jsonDoc.object().value("top-right").toInt()));

    if(jsonDoc.object().value("top-left") == QJsonValue::Undefined)
        return 7;
    else settings.insert("top-left", static_cast<const unsigned int>(jsonDoc.object().value("top-left").toInt()));

    if(jsonDoc.object().value("down-right") == QJsonValue::Undefined)
        return 8;
    else settings.insert("down-right", static_cast<const unsigned int>(jsonDoc.object().value("down-right").toInt()));

    if(jsonDoc.object().value("down-left") == QJsonValue::Undefined)
        return 9;
    else settings.insert("down-left", static_cast<const unsigned int>(jsonDoc.object().value("down-left").toInt()));

    if(jsonDoc.object().value("click") == QJsonValue::Undefined)
        return 10;
    else settings.insert("click", static_cast<const unsigned int>(jsonDoc.object().value("click").toInt()));

    if(jsonDoc.object().value("right click") == QJsonValue::Undefined)
        return 11;
    else settings.insert("right click", static_cast<const unsigned int>(jsonDoc.object().value("right click").toInt()));

    if(jsonDoc.object().value("autorun") == QJsonValue::Undefined)
        return 12;
    else settings.insert("autorun", static_cast<Qt::CheckState>(jsonDoc.object().value("autorun").toInt()));

    if(jsonDoc.object().value("hot key") == QJsonValue::Undefined)
        return 13;
    else settings.insert("hot key", static_cast<Qt::CheckState>(jsonDoc.object().value("hot key").toInt()));

    if(jsonDoc.object().value("Ctrl state") == QJsonValue::Undefined)
        return 14;
    else settings.insert("Ctrl state", static_cast<const unsigned int>(jsonDoc.object().value("Ctrl state").toInt()));

    if(jsonDoc.object().value("Alt state") == QJsonValue::Undefined)
        return 15;
    else settings.insert("Alt state", static_cast<const unsigned int>(jsonDoc.object().value("Alt state").toInt()));

    if(jsonDoc.object().value("another key state") == QJsonValue::Undefined)
        return 16;
    else settings.insert("another key state", static_cast<const unsigned int>(jsonDoc.object().value("another key state").toInt()));

    /* 100 - это признак удачного завершения функции */
    return 100; // все хорошо, все настройки были считаны верно
}
