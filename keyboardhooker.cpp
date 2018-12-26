#include "keyboardhooker.h"
#include "mainwindow.h"
#include "translationinfo.h"
#include "shlobj.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QMessageBox>
#include <QTranslator>

extern MainWindow *w;
HHOOK KeyBoardHooker::keyboardHook;
QMap<QString, unsigned int> KeyBoardHooker::settings;
QMap<QString, unsigned int> KeyBoardHooker::tempSettings;

KeyBoardHooker::KeyBoardHooker(QObject *parent) : QObject(parent)
{
    hInstance = GetModuleHandle(nullptr);
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookProc, hInstance, 0);

    if(keyboardHook == nullptr)
        QMessageBox::critical(nullptr, tr("Критична помилка"), tr("<p style='font-size:12pt'>Програма не може виконувати основну функцію (керувати мишкою за допомогою клавіатури) через помилку під'єднання до системних подій.</p>"));
}

QMap<QString, unsigned int> *KeyBoardHooker::getSettings()
{
    return &settings;
}

QMap<QString, unsigned int> *KeyBoardHooker::getTempSettings()
{
    return &tempSettings;
}

QString KeyBoardHooker::getKeyNameByVirtualKey(unsigned int vk)
{
    CHAR szName[128];
    UINT scanCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    int result = 0;
    switch (vk)
    {
        case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
        case VK_RCONTROL: case VK_RMENU:
        case VK_LWIN: case VK_RWIN: case VK_APPS:
        case VK_PRIOR: case VK_NEXT:
        case VK_END: case VK_HOME:
        case VK_INSERT: case VK_DELETE:
        case VK_DIVIDE: case VK_NUMLOCK:
            scanCode |= KF_EXTENDED;
        default:
            result = GetKeyNameTextA(static_cast<LONG>(scanCode << 16), szName, 128);
    }
    if(result == 0)
    {
        //throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "WinAPI error ocurred.");
        return QString();
    }
    return QString(szName);
}

QString KeyBoardHooker::getSettingNameByKeyName(QString keyName, bool settingMap)
{
    QMap<QString, unsigned int> settings = settingMap ? tempSettings : KeyBoardHooker::settings;
    QMap<QString, unsigned int>::const_iterator i = settings.constBegin();
    while (i != settings.constEnd()) {
        if(i.key() != QString("speed x") && i.key() != QString("speed y") && i.key() != QString("speed wheel") && i.key() != QString("autorun") &&
                i.key() != QString("hot key") && i.key() != QString("Ctrl state") && i.key() != QString("Alt state") &&
                i.key() != QString("another key state") && i.key() != QString("language") && getKeyNameByVirtualKey(i.value()) == keyName)
            return i.key();
        ++i;
    }

    return QString();
}

void KeyBoardHooker::configureSettings(QVector<int> *errors)
{
    if(errors->isEmpty())
        return;
    if(errors->contains(0))
        settings.insert("speed x", 5);
    if(errors->contains(1))
        settings.insert("speed y", 5);
    if(errors->contains(2))
        settings.insert("up", 0x26/*0x68*/);
    if(errors->contains(3))
        settings.insert("down", 0x28/*0x62*/);
    if(errors->contains(4))
        settings.insert("right", 0x27/*0x66*/);
    if(errors->contains(5))
        settings.insert("left", 0x25/*0x64*/);
    if(errors->contains(6))
        settings.insert("top-right", 0x69);
    if(errors->contains(7))
        settings.insert("top-left", 0x67);
    if(errors->contains(8))
        settings.insert("down-right", 0x63);
    if(errors->contains(9))
        settings.insert("down-left", 0x61);
    if(errors->contains(10))
        settings.insert("click", 0x65);
    if(errors->contains(11))
        settings.insert("right click", 0x60);
    if(errors->contains(12))
        settings.insert("autorun", Qt::Checked);
    if(errors->contains(13))
        settings.insert("hot key", Qt::Checked);
    if(errors->contains(14))
        settings.insert("Ctrl state", HOTKEYF_CONTROL);
    if(errors->contains(15))
        settings.insert("Alt state", HOTKEYF_ALT);
    if(errors->contains(16))
        settings.insert("another key state", 0x76);
    if(errors->contains(17))
        settings.insert("speed wheel", 5);
    if(errors->contains(18))
        settings.insert("wheel up", 0x6b);
    if(errors->contains(19))
        settings.insert("wheel down", 0x6d);
    if(errors->contains(20))
        settings.insert("language", Ukrainian);
}

void KeyBoardHooker::setNewKeyValue(QString key, unsigned int value)
{
    settings[key] = value;
}

bool KeyBoardHooker::setTempSetting(QString key, unsigned int value)
{
    if(!tempSettings.contains(key)) // во временных настройках нет такого ключа
    {
        tempSettings.insert(key, value);
        return false; // кнопка не была добавлена в временный массив ранее
    }
    else  // во временных настройках есть такой ключ
    {
        tempSettings[key] = value;
        return false;
    }
    return true; // кнопка была добавлена в временный массив ранее
}

KeyBoardHooker &KeyBoardHooker::instance()
{
    static KeyBoardHooker instance;
    return instance;
}

void KeyBoardHooker::unhookExit()
{
    UnhookWindowsHookEx(KeyBoardHooker::keyboardHook);
    QCoreApplication::quit();
}

void KeyBoardHooker::setParent(MainWindow *parent)
{
    this->parent = parent;
}

bool KeyBoardHooker::isContainKey(unsigned int key, bool settingMap)
{
    foreach(UINT value, settingMap ? tempSettings : settings)
    {
        if(value == key)
            return true;
    }
    return false;
}

bool KeyBoardHooker::isSM0ContainKeyWithoutCrossing(unsigned int key)
{
    QMap<QString, unsigned int>::const_iterator i = settings.constBegin();
    while (i != settings.constEnd()) {
        if(!tempSettings.contains(i.key()) && i.value() == key)
            return true;
        ++i;
    }

    return false;
}

LRESULT CALLBACK KeyBoardHooker::keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    bool bAltKeyDown = 0;
    bool bControlKeyDown = 0;
    KBDLLHOOKSTRUCT *p = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

    bAltKeyDown = GetAsyncKeyState(VK_MENU) >> ((sizeof(SHORT) * 8) - 1);
    bControlKeyDown = GetAsyncKeyState(VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1);

    // Проверка, если F12 + CTRL были нажаты, если да -> завершение работы программы
    if (p->vkCode == VK_F12 && bControlKeyDown)
        KeyBoardHooker::unhookExit();
    // Проверка, если F11 + CTRL были нажаты и окно было скрыто, если да -> открыть окно программы
    if (p->vkCode == VK_F11 && bControlKeyDown && !w->isVisible())
        w->show();

    static bool leftMouseButDown = false;
    static bool rightMouseButDown = false;
    static bool toUpButDown = false;
    static bool toDownButDown = false;
    static bool toRightButDown = false;
    static bool toLeftButDown = false;

    if (wParam == WM_SYSKEYUP || wParam == WM_KEYUP)
    {
        POINT currentPos;
        GetCursorPos(&currentPos);

        if(p->vkCode == settings["right click"])
        {
            rightMouseButDown = false;
            mouse_event(MOUSEEVENTF_RIGHTUP, static_cast<DWORD>(currentPos.x), static_cast<DWORD>(currentPos.y), 0, 0);
        }
        else if(p->vkCode == settings["click"])
        {
            leftMouseButDown = false;
            mouse_event(MOUSEEVENTF_LEFTUP, static_cast<DWORD>(currentPos.x), static_cast<DWORD>(currentPos.y), 0, 0);
        }
        else if(p->vkCode == settings["up"])
            toUpButDown = false;
        else if(p->vkCode == settings["down"])
            toDownButDown = false;
        else if(p->vkCode == settings["right"])
            toRightButDown = false;
        else if(p->vkCode == settings["left"])
            toLeftButDown = false;
        else return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    else if (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN)
    {
        if(w->isActiveWindow() && w->getFocusedLineEdit() != nullptr && p->vkCode != VK_TAB)
        {
            if(w->isStartKeyLineEdit(w->getFocusedLineEdit()) && p->vkCode != VK_MENU && p->vkCode != VK_CONTROL)
            {
                if(!tempSettings.contains("Ctrl state"))
                {
                    setTempSetting("Ctrl state", 0);
                    setTempSetting("Alt state", 0);
                    setTempSetting("another key state", 0);
                }
                QString hotKey = "";
                if(bControlKeyDown)
                {
                    hotKey += "Ctrl + ";
                    tempSettings["Ctrl state"] = HOTKEYF_CONTROL;
                }
                if(bAltKeyDown)
                {
                    hotKey += "Alt + ";
                    tempSettings["Alt state"] = HOTKEYF_ALT;
                }
                tempSettings["another key state"] = p->vkCode;
                hotKey += getKeyNameByVirtualKey(p->vkCode);
                 w->getFocusedLineEdit()->setText(hotKey);
            }
            else
            {
                QString tmpKeyName = getKeyNameByVirtualKey(p->vkCode);
                QString tmpSettingName0 = getSettingNameByKeyName( w->getFocusedLineEdit()->text());
                QString tmpSettingName1 = getSettingNameByKeyName( w->getFocusedLineEdit()->text(), 1);
                if(tmpKeyName == w->getFocusedLineEdit()->text())
                {
                    //return 0;
                }
                else if( !tmpKeyName.isEmpty() && !isContainKey(p->vkCode, 1) && !isSM0ContainKeyWithoutCrossing(p->vkCode))
                {
                    setTempSetting(tmpSettingName1.isEmpty() ? tmpSettingName0 : tmpSettingName1, p->vkCode);
                    w->getFocusedLineEdit()->setText(tmpKeyName);
                    return 0;
                }
                else
                {
                    w->getFocusedLineEdit()->clearFocus();
                    QMessageBox *msg = new QMessageBox(QMessageBox::Warning, tr("Увага"), tr("<p style='font-size:12pt'>Обрана клавіша вже використовується.</p>"), QMessageBox::Ok);
                    connect(msg, &QMessageBox::finished, msg, &QMessageBox::deleteLater);
                    msg->open();
                    //delete msg;
                    //::warning(w, tr("Увага"), tr("<p style='font-size:12pt'>Обрана клавіша вже використовується.</p>"));
                }
            }
        }

        POINT currentPos;
        GetCursorPos(&currentPos);

        // Virtual key codes reference: http://msdn.microsoft.com/en-us/library/dd375731%28v=VS.85%29.aspx  
        if(toUpButDown && toRightButDown)
            SetCursorPos(static_cast<int>(static_cast<unsigned long>(currentPos.x) + settings["speed x"]), static_cast<int>(static_cast<unsigned long>(currentPos.y) - settings["speed y"]));
        else if(toUpButDown && toLeftButDown)
            SetCursorPos(static_cast<int>(static_cast<unsigned long>(currentPos.x) - settings["speed x"]), static_cast<int>(static_cast<unsigned long>(currentPos.y) - settings["speed y"]));
        else if(toDownButDown && toRightButDown)
            SetCursorPos(static_cast<int>(static_cast<unsigned long>(currentPos.x) + settings["speed x"]), static_cast<int>(static_cast<unsigned long>(currentPos.y) + settings["speed y"]));
        else if(toDownButDown && toLeftButDown)
            SetCursorPos(static_cast<int>(static_cast<unsigned long>(currentPos.x) - settings["speed x"]), static_cast<int>(static_cast<unsigned long>(currentPos.y) + settings["speed y"]));
        else if(p->vkCode == settings["right click"])
        {
            rightMouseButDown = true;
            mouse_event(MOUSEEVENTF_RIGHTDOWN, static_cast<DWORD>(currentPos.x), static_cast<DWORD>(currentPos.y), 0, 0);
        }
        else if(p->vkCode == settings["down-left"])
            SetCursorPos(static_cast<int>(static_cast<unsigned long>(currentPos.x) - settings["speed x"]), static_cast<int>(static_cast<unsigned long>(currentPos.y) + settings["speed y"]));
        else if(p->vkCode == settings["down"])
        {
            toDownButDown = true;
            SetCursorPos(currentPos.x, static_cast<int>(static_cast<unsigned long>(currentPos.y) + settings["speed y"]));
        }
        else if(p->vkCode == settings["down-right"])
            SetCursorPos(static_cast<int>(static_cast<unsigned long>(currentPos.x) + settings["speed x"]), static_cast<int>(static_cast<unsigned long>(currentPos.y) + settings["speed y"]));
        else if(p->vkCode == settings["left"])
        {
            toLeftButDown = true;
            SetCursorPos(static_cast<int>(static_cast<unsigned long>(currentPos.x) - settings["speed x"]), currentPos.y);
        }
        else if(p->vkCode == settings["click"] && !leftMouseButDown)
        {
            leftMouseButDown = true;
            mouse_event(MOUSEEVENTF_LEFTDOWN, static_cast<DWORD>(currentPos.x), static_cast<DWORD>(currentPos.y), 0, 0);
        }
        else if(p->vkCode == settings["right"])
        {
            toRightButDown = true;
            SetCursorPos(static_cast<int>(static_cast<unsigned long>(currentPos.x) + settings["speed x"]), currentPos.y);
        }
        else if(p->vkCode == settings["top-left"])
            SetCursorPos(static_cast<int>(static_cast<unsigned long>(currentPos.x) - settings["speed x"]), static_cast<int>(static_cast<unsigned long>(currentPos.y) - settings["speed y"]));
        else if(p->vkCode == settings["up"])
        {
            toUpButDown = true;
            SetCursorPos(currentPos.x, static_cast<int>(static_cast<unsigned long>(currentPos.y) - settings["speed y"]));
        }
        else if(p->vkCode == settings["top-right"])
            SetCursorPos(static_cast<int>(static_cast<unsigned long>(currentPos.x) + settings["speed x"]), static_cast<int>(static_cast<unsigned long>(currentPos.y) - settings["speed y"]));
        else if(p->vkCode == settings["wheel up"])
            mouse_event(MOUSEEVENTF_WHEEL, static_cast<DWORD>(currentPos.x), static_cast<DWORD>(currentPos.y), static_cast<DWORD>(settings["speed wheel"]), 0);
        else if(p->vkCode == settings["wheel down"])
            mouse_event(MOUSEEVENTF_WHEEL, static_cast<DWORD>(currentPos.x), static_cast<DWORD>(currentPos.y), -static_cast<DWORD>(settings["speed wheel"]), 0);
        else return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    else return CallNextHookEx(nullptr, nCode, wParam, lParam);

    //return 1;
}
