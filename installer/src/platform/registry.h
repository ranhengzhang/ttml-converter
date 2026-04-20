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

#ifndef REGISTRY_H
#define REGISTRY_H

#include <QList>

struct SubMenuItem {
    QString key;
    QString title;
};

class Registry {
public:
    static bool installContextMenu();

    static bool uninstallContextMenu();

private:
    static bool createSubMenu(const QString &parent_key, const QString &menu_name,
                              const QList<SubMenuItem> &items);

    static bool deleteSubMenu(const QString &parent_key);
};

#endif // REGISTRY_H
