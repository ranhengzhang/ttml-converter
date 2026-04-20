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

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QCoreApplication>

#include "src/app/application.h"
#include "version.h"

int main(int argc, char *argv[]) {
    const QCoreApplication qt_app(argc, argv);
    QCoreApplication::setApplicationName("ttml-converter-installer");
    QCoreApplication::setApplicationVersion(PROJECT_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription("TTML 字幕转换工具安装程序");

    // 自动处理 --help 和 --version
    parser.addHelpOption();
    parser.addVersionOption();

    const QCommandLineOption install_option(
        QStringList() << "i" << "install",
        "安装右键菜单"
    );

    const QCommandLineOption uninstall_option(
        QStringList() << "u" << "uninstall",
        "卸载右键菜单"
    );

    parser.addOption(install_option);
    parser.addOption(uninstall_option);

    parser.process(qt_app);

    int result = 0;

    if (parser.isSet(install_option)) {
        result = Application::install();
    } else if (parser.isSet(uninstall_option)) {
        result = Application::uninstall();
    } else {
        result = Application::runMenu();
    }

    system("PAUSE");

    return result;
}
