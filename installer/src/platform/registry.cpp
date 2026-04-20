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

#include "registry.h"
#include "winregistry.h"
#include <QCoreApplication>
#include <QString>
#include <QDir>

static const auto BASE_KEY = QStringLiteral(R"(*\shell\TTMLConverter)");

static QString getConverterPath() {
    const QString installer_path = QCoreApplication::applicationFilePath();
    const QFileInfo installer_info(installer_path);
    const QDir dir = installer_info.dir();

    QString converter_path = dir.filePath(QStringLiteral("ttml-converter.exe"));
    return converter_path;
}

bool Registry::createSubMenu(const QString &parent_key, const QString &menu_name, const QList<SubMenuItem> &items) {
    const QString exe_path = getConverterPath().replace("/", "\\");
    const QString exe_path_quoted = QString(R"("%1")").arg(exe_path);
    WinRegistry reg(WinRegistry::ClassesRoot);

    // 检查是否已存在，如果存在则先删除
    if (reg.open(parent_key, false)) {
        reg.close();
        deleteSubMenu(parent_key);
    }

    if (!reg.open(parent_key, true)) {
        return false;
    }

    reg.setValue(QString(), QString());
    reg.setValue(QStringLiteral("SubCommands"), QString());
    reg.setValue(QStringLiteral("AppliesTo"), QStringLiteral("System.ItemName:ttml"));
    reg.setValue(QStringLiteral("Icon"), exe_path + QStringLiteral(",0"));
    reg.setValue(QStringLiteral("MUIVerb"), menu_name);
    reg.close();

    const QString shell_key = parent_key + QStringLiteral("\\shell");
    if (!reg.open(shell_key, true)) {
        return false;
    }
    reg.close();

    for (const auto &[ext, title]: items) {
        QString item_key = shell_key + QStringLiteral("\\") + ext;

        if (!reg.open(item_key, true)) {
            continue;
        }

        reg.setValue(QString(), ext);
        reg.setValue(QStringLiteral("MUIVerb"), QStringLiteral("%1").arg(title));
        reg.close();

        QString command_key = item_key + QStringLiteral(R"(\command)");
        if (reg.open(command_key, true)) {
            QString command = QStringLiteral(R"(%1 -f %2 "%3")").arg(exe_path_quoted).arg(ext).arg(QString("%1"));
            reg.setValue(QString(), command);
            reg.close();
        }
    }

    return true;
}

bool Registry::deleteSubMenu(const QString &parent_key) {
    return WinRegistry::deleteTree(WinRegistry::ClassesRoot, parent_key);
}

bool Registry::installContextMenu() {
    QList<SubMenuItem> convert_items;

    convert_items.append({QStringLiteral("ttml"), QStringLiteral("TTML 文件去格式化/压缩")});
    convert_items.append({QStringLiteral("ass"), QStringLiteral("转换为 ASS 特效字幕")});
    convert_items.append({QStringLiteral("lrc"), QStringLiteral("转换为 LRC 歌词 (Walkman 标准)")});
    convert_items.append({QStringLiteral("spl"), QStringLiteral("转换为 SPL 歌词")});
    convert_items.append({QStringLiteral("lys"), QStringLiteral("转换为 LYS 歌词")});
    convert_items.append({QStringLiteral("qrc"), QStringLiteral("转换为 QRC 歌词")});
    convert_items.append({QStringLiteral("yrc"), QStringLiteral("转换为 YRC 歌词")});
    convert_items.append({QStringLiteral("krc"), QStringLiteral("转换为 KRC 歌词")});
    convert_items.append({QStringLiteral("txt"), QStringLiteral("转换为 TXT 文本")});

    return createSubMenu(BASE_KEY, QStringLiteral("TTML 格式转换"), convert_items);
}

bool Registry::uninstallContextMenu() {
    return deleteSubMenu(BASE_KEY);
}
