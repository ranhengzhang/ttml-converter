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

#include "winregistry.h"
#include <QDebug>

WinRegistry::WinRegistry(const RootKey root)
    : _m_root(toHKey(root))
      , _m_key(nullptr)
      , _m_is_open(false) {
}

WinRegistry::~WinRegistry() {
    close();
}

HKEY WinRegistry::toHKey(const RootKey root) {
    return reinterpret_cast<HKEY>(root);
}

bool WinRegistry::open(const QString &path, bool create_if_not_exist) {
    close();

    const std::wstring wpath = path.toStdWString();
    LONG result;

    if (create_if_not_exist) {
        DWORD disposition;
        result = RegCreateKeyExW(
            _m_root,
            wpath.c_str(),
            0,
            nullptr,
            REG_OPTION_NON_VOLATILE,
            KEY_READ | KEY_WRITE,
            nullptr,
            &_m_key,
            &disposition
        );
    } else {
        result = RegOpenKeyExW(
            _m_root,
            wpath.c_str(),
            0,
            KEY_READ | KEY_WRITE,
            &_m_key
        );
    }

    if (result == ERROR_SUCCESS) {
        _m_is_open = true;
        return true;
    }

    return false;
}

void WinRegistry::close() {
    if (_m_is_open &&_m_key 
    !=
    nullptr
    ) {
        RegCloseKey(_m_key);
        _m_key = nullptr;
        _m_is_open = false;
    }
}

bool WinRegistry::setValue(const QString &name, const QString &value) const {
    if (!_m_is_open) return false;

    const std::wstring wname = name.isEmpty() ? L"" : name.toStdWString();
    const std::wstring wvalue = value.toStdWString();

    LONG result = RegSetValueExW(
        _m_key,
        wname.empty() ? nullptr : wname.c_str(),
        0,
        REG_SZ,
        reinterpret_cast<const BYTE *>(wvalue.c_str()),
        static_cast<DWORD>((wvalue.length() + 1) * sizeof(wchar_t))
    );

    return result == ERROR_SUCCESS;
}

bool WinRegistry::setValue(const QString &name, const uint32_t value) const {
    if (!_m_is_open) return false;

    const std::wstring wname = name.isEmpty() ? L"" : name.toStdWString();

    LONG result = RegSetValueExW(
        _m_key,
        wname.empty() ? nullptr : wname.c_str(),
        0,
        REG_DWORD,
        reinterpret_cast<const BYTE *>(&value),
        sizeof(DWORD)
    );

    return result == ERROR_SUCCESS;
}

bool WinRegistry::setValue(const QString &name, const QByteArray &value) const {
    if (!_m_is_open) return false;

    const std::wstring wname = name.isEmpty() ? L"" : name.toStdWString();

    LONG result = RegSetValueExW(
        _m_key,
        wname.empty() ? nullptr : wname.c_str(),
        0,
        REG_BINARY,
        reinterpret_cast<const BYTE *>(value.constData()),
        static_cast<DWORD>(value.size())
    );

    return result == ERROR_SUCCESS;
}

bool WinRegistry::deleteValue(const QString &name) const {
    if (!_m_is_open) return false;

    const std::wstring wname = name.toStdWString();
    const LONG result = RegDeleteValueW(_m_key, wname.c_str());

    return result == ERROR_SUCCESS;
}

bool WinRegistry::deleteKey(const QString &path) const {
    const std::wstring wpath = path.toStdWString();
    const LONG result = RegDeleteKeyW(_m_root, wpath.c_str());

    return result == ERROR_SUCCESS;
}

bool WinRegistry::deleteTree(const QString &path) const {
    const std::wstring wpath = path.toStdWString();
    const LONG result = RegDeleteTreeW(_m_root, wpath.c_str());

    return result == ERROR_SUCCESS;
}

QString WinRegistry::stringValue(const QString &name, const QString &default_value) const {
    if (!_m_is_open) return default_value;

    const std::wstring wname = name.isEmpty() ? L"" : name.toStdWString();
    wchar_t buffer[1024];
    DWORD size = sizeof(buffer);
    DWORD type;

    const LONG result = RegQueryValueExW(
        _m_key,
        wname.empty() ? nullptr : wname.c_str(),
        nullptr,
        &type,
        reinterpret_cast<BYTE *>(buffer),
        &size
    );

    if (result == ERROR_SUCCESS && type == REG_SZ) {
        return QString::fromWCharArray(buffer);
    }

    return default_value;
}

uint32_t WinRegistry::dwordValue(const QString &name, const uint32_t default_value) const {
    if (!_m_is_open) return default_value;

    const std::wstring wname = name.isEmpty() ? L"" : name.toStdWString();
    DWORD value;
    DWORD size = sizeof(DWORD);
    DWORD type;

    const LONG result = RegQueryValueExW(
        _m_key,
        wname.empty() ? nullptr : wname.c_str(),
        nullptr,
        &type,
        reinterpret_cast<BYTE *>(&value),
        &size
    );

    if (result == ERROR_SUCCESS && type == REG_DWORD) {
        return value;
    }

    return default_value;
}

QByteArray WinRegistry::binaryValue(const QString &name) const {
    if (!_m_is_open) return {};

    const std::wstring wname = name.isEmpty() ? L"" : name.toStdWString();
    DWORD size = 0;
    DWORD type;

    LONG result = RegQueryValueExW(
        _m_key,
        wname.empty() ? nullptr : wname.c_str(),
        nullptr,
        &type,
        nullptr,
        &size
    );

    if (result != ERROR_SUCCESS || type != REG_BINARY) {
        return {};
    }

    QByteArray data(size, 0);
    result = RegQueryValueExW(
        _m_key,
        wname.empty() ? nullptr : wname.c_str(),
        nullptr,
        &type,
        reinterpret_cast<BYTE *>(data.data()),
        &size
    );

    if (result == ERROR_SUCCESS) {
        return data;
    }

    return {};
}

bool WinRegistry::exists(const QString &name) const {
    if (!_m_is_open) return false;

    const std::wstring wname = name.isEmpty() ? L"" : name.toStdWString();
    DWORD type;
    DWORD size = 0;

    LONG result = RegQueryValueExW(
        _m_key,
        wname.empty() ? nullptr : wname.c_str(),
        nullptr,
        &type,
        nullptr,
        &size
    );

    return result == ERROR_SUCCESS;
}

QStringList WinRegistry::subKeys() const {
    QStringList keys;
    if (!_m_is_open) return keys;

    DWORD index = 0;
    wchar_t buffer[256];
    DWORD size = 256;

    while (RegEnumKeyExW(_m_key, index, buffer, &size, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
        keys.append(QString::fromWCharArray(buffer));
        size = 256;
        index++;
    }

    return keys;
}

QStringList WinRegistry::valueNames() const {
    QStringList names;
    if (!_m_is_open) return names;

    DWORD index = 0;
    wchar_t buffer[256];
    DWORD size = 256;

    while (RegEnumValueW(_m_key, index, buffer, &size, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
        names.append(QString::fromWCharArray(buffer));
        size = 256;
        index++;
    }

    return names;
}

bool WinRegistry::writeString(const RootKey root, const QString &path, const QString &name, const QString &value) {
    WinRegistry reg(root);
    if (!reg.open(path, true)) return false;
    return reg.setValue(name, value);
}

bool WinRegistry::writeDword(const RootKey root, const QString &path, const QString &name, const uint32_t value) {
    WinRegistry reg(root);
    if (!reg.open(path, true)) return false;
    return reg.setValue(name, value);
}

QString WinRegistry::readString(const RootKey root, const QString &path, const QString &name,
                                const QString &default_value) {
    WinRegistry reg(root);
    if (!reg.open(path, false)) return default_value;
    return reg.stringValue(name, default_value);
}

bool WinRegistry::deleteTree(const RootKey root, const QString &path) {
    const WinRegistry reg(root);
    return reg.deleteTree(path);
}
