#include "keeper.h"
#include "translationinfo.h"
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QVector>

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
    obj.insert("speed wheel", static_cast<int>(settings["speed wheel"]));
    obj.insert("wheel up", static_cast<int>(settings["wheel up"]));
    obj.insert("wheel down", static_cast<int>(settings["wheel down"]));
    obj.insert("language", static_cast<Language>(settings["language"]));

    QJsonDocument jsonDoc;
    jsonDoc.setObject(obj);

    QFile file;
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

void Keeper::removeSettingsFile()
{
    QFile("settings.json").remove();
}

QVector<int> *Keeper::loadSettings(QMap<QString, unsigned int> &settings)
{
    QFile file;
    file.setFileName("settings.json");

    try
    {
        if(!file.open(QIODevice::ReadOnly))
            throw -1;
    }
    catch(int key)
    {
        std::vector<int> errors = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
        return new QVector<int>(QVector<int>::fromStdVector(errors)); // ошибка: нет файла
    }


    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if(jsonDoc.isEmpty() || jsonDoc.isNull() || jsonDoc.object().isEmpty())
    {
        std::vector<int> errors = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
        return new QVector<int>(QVector<int>::fromStdVector(errors)); // файл пуст или не содержит корректные настройки
    }

    QVector<int> *errors = new QVector<int>;

    if(jsonDoc.object().value("speed x") == QJsonValue::Undefined)
        errors->push_back(0);
    else settings.insert("speed x", static_cast<const unsigned int>(jsonDoc.object().value("speed x").toInt()));

    if(jsonDoc.object().value("speed y") == QJsonValue::Undefined)
        errors->push_back(1);
    else settings.insert("speed y", static_cast<const unsigned int>(jsonDoc.object().value("speed y").toInt()));

    if(jsonDoc.object().value("up") == QJsonValue::Undefined)
        errors->push_back(2);
    else settings.insert("up", static_cast<const unsigned int>(jsonDoc.object().value("up").toInt()));

    if(jsonDoc.object().value("down") == QJsonValue::Undefined)
        errors->push_back(3);
    else settings.insert("down", static_cast<const unsigned int>(jsonDoc.object().value("down").toInt()));

    if(jsonDoc.object().value("right") == QJsonValue::Undefined)
        errors->push_back(4);
    else settings.insert("right", static_cast<const unsigned int>(jsonDoc.object().value("right").toInt()));

    if(jsonDoc.object().value("left") == QJsonValue::Undefined)
        errors->push_back(5);
    else settings.insert("left", static_cast<const unsigned int>(jsonDoc.object().value("left").toInt()));

    if(jsonDoc.object().value("top-right") == QJsonValue::Undefined)
        errors->push_back(6);
    else settings.insert("top-right", static_cast<const unsigned int>(jsonDoc.object().value("top-right").toInt()));

    if(jsonDoc.object().value("top-left") == QJsonValue::Undefined)
        errors->push_back(7);
    else settings.insert("top-left", static_cast<const unsigned int>(jsonDoc.object().value("top-left").toInt()));

    if(jsonDoc.object().value("down-right") == QJsonValue::Undefined)
        errors->push_back(8);
    else settings.insert("down-right", static_cast<const unsigned int>(jsonDoc.object().value("down-right").toInt()));

    if(jsonDoc.object().value("down-left") == QJsonValue::Undefined)
        errors->push_back(9);
    else settings.insert("down-left", static_cast<const unsigned int>(jsonDoc.object().value("down-left").toInt()));

    if(jsonDoc.object().value("click") == QJsonValue::Undefined)
        errors->push_back(10);
    else settings.insert("click", static_cast<const unsigned int>(jsonDoc.object().value("click").toInt()));

    if(jsonDoc.object().value("right click") == QJsonValue::Undefined)
        errors->push_back(11);
    else settings.insert("right click", static_cast<const unsigned int>(jsonDoc.object().value("right click").toInt()));

    if(jsonDoc.object().value("autorun") == QJsonValue::Undefined)
        errors->push_back(12);
    else settings.insert("autorun", static_cast<Qt::CheckState>(jsonDoc.object().value("autorun").toInt()));

    if(jsonDoc.object().value("hot key") == QJsonValue::Undefined)
        errors->push_back(13);
    else settings.insert("hot key", static_cast<Qt::CheckState>(jsonDoc.object().value("hot key").toInt()));

    if(jsonDoc.object().value("Ctrl state") == QJsonValue::Undefined)
        errors->push_back(14);
    else settings.insert("Ctrl state", static_cast<const unsigned int>(jsonDoc.object().value("Ctrl state").toInt()));

    if(jsonDoc.object().value("Alt state") == QJsonValue::Undefined)
        errors->push_back(15);
    else settings.insert("Alt state", static_cast<const unsigned int>(jsonDoc.object().value("Alt state").toInt()));

    if(jsonDoc.object().value("another key state") == QJsonValue::Undefined)
        errors->push_back(16);
    else settings.insert("another key state", static_cast<const unsigned int>(jsonDoc.object().value("another key state").toInt()));

    if(jsonDoc.object().value("speed wheel") == QJsonValue::Undefined)
        errors->push_back(17);
    else settings.insert("speed wheel", static_cast<const unsigned int>(jsonDoc.object().value("speed wheel").toInt()));

    if(jsonDoc.object().value("wheel up") == QJsonValue::Undefined)
        errors->push_back(18);
    else settings.insert("wheel up", static_cast<const unsigned int>(jsonDoc.object().value("wheel up").toInt()));

    if(jsonDoc.object().value("wheel down") == QJsonValue::Undefined)
        errors->push_back(19);
    else settings.insert("wheel down", static_cast<const unsigned int>(jsonDoc.object().value("wheel down").toInt()));

    if(jsonDoc.object().value("language") == QJsonValue::Undefined)
        errors->push_back(20);
    else settings.insert("language", static_cast<Language>(jsonDoc.object().value("language").toInt()));

    return errors;
}
