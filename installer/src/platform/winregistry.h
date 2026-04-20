//  TTML Converter Installer - Installer for TTML Converter tool
//  Copyright (C) 2026  ranhengzhang
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef WINREGISTRY_H
#define WINREGISTRY_H

#include <QString>
#include <QVariant>
#include <windows.h>

class WinRegistry {
public:
    enum RootKey : intptr_t {
        ClassesRoot = reinterpret_cast<intptr_t>(HKEY_CLASSES_ROOT),
        CurrentUser = reinterpret_cast<intptr_t>(HKEY_CURRENT_USER),
        LocalMachine = reinterpret_cast<intptr_t>(HKEY_LOCAL_MACHINE),
        Users = reinterpret_cast<intptr_t>(HKEY_USERS),
        CurrentConfig = reinterpret_cast<intptr_t>(HKEY_CURRENT_CONFIG)
    };

    WinRegistry(RootKey root);

    ~WinRegistry();

    bool open(const QString &path, bool create_if_not_exist = false);

    void close();

    [[nodiscard]] bool setValue(const QString &name, const QString &value) const;

    [[nodiscard]] bool setValue(const QString &name, uint32_t value) const;

    [[nodiscard]] bool setValue(const QString &name, const QByteArray &value) const;

    [[nodiscard]] bool deleteValue(const QString &name) const;

    [[nodiscard]] bool deleteKey(const QString &path) const;

    [[nodiscard]] bool deleteTree(const QString &path) const;

    [[nodiscard]] QString stringValue(const QString &name, const QString &default_value = QString()) const;

    [[nodiscard]] uint32_t dwordValue(const QString &name, uint32_t default_value = 0) const;

    [[nodiscard]] QByteArray binaryValue(const QString &name) const;

    [[nodiscard]] bool exists(const QString &name) const;

    [[nodiscard]] QStringList subKeys() const;

    [[nodiscard]] QStringList valueNames() const;

    static bool writeString(RootKey root, const QString &path, const QString &name, const QString &value);

    static bool writeDword(RootKey root, const QString &path, const QString &name, uint32_t value);

    static QString readString(RootKey root, const QString &path, const QString &name,
                              const QString &default_value = QString());

    static bool deleteTree(RootKey root, const QString &path);

private:
    HKEY _m_root;
    HKEY _m_key;
    bool _m_is_open;

    static HKEY toHKey(RootKey root);
};

#endif // WINREGISTRY_H
