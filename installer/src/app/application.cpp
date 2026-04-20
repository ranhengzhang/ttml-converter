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

#include "application.h"

#include "../platform/registry.h"

#include <iostream>

Application::Application() = default;

int Application::install() {
    std::cout << "\n[安装右键菜单]\n";
    if (Registry::installContextMenu()) {
        std::cout << "安装成功！\n";
        return 0;
    } else {
        std::cerr << "安装失败！请以管理员身份运行。\n";
        return 1;
    }
}

int Application::uninstall() {
    std::cout << "\n[卸载右键菜单]\n";
    if (Registry::uninstallContextMenu()) {
        std::cout << "卸载成功！\n";
        return 0;
    } else {
        std::cerr << "卸载失败！请以管理员身份运行。\n";
        return 1;
    }
}

int Application::runMenu() {
    while (true) {
        printMenu();

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                install();
                return 0;
            case 2:
                uninstall();
                return 0;
            case 3:
                return 0;
            default:
                std::cout << "\n无效选择，请重新输入\n";
        }
    }
}

void Application::printMenu() {
    std::cout << "\n";
    std::cout << "============================\n";
    std::cout << "     TTML 转换器\n";
    std::cout << "============================\n";
    std::cout << "  1. 安装右键菜单\n";
    std::cout << "  2. 卸载右键菜单\n";
    std::cout << "  3. 退出\n";
    std::cout << "----------------------------\n";
    std::cout << "请选择 [1-3]: ";
}
