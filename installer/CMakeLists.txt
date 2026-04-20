#  TTML Converter Installer - Installer for TTML Converter tool
#  Copyright (C) 2026  ranhengzhang
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

project(installer VERSION 0.1.2)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

# 获取当前年份
string(TIMESTAMP CURRENT_YEAR "%Y")

# 生成版本头文件
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
    @ONLY
)

# 生成 Windows 资源文件
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
    @ONLY
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SHARED_SOURCES
    src/app/application.cpp
    src/platform/registry.cpp
    src/platform/winregistry.cpp
)

set(SHARED_HEADERS
    src/app/application.h
    src/platform/registry.h
    src/platform/winregistry.h
)

add_executable(installer
    main.cpp
    ${SHARED_SOURCES}
    ${SHARED_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
)

set_target_properties(installer PROPERTIES
    OUTPUT_NAME "ttml-converter-installer"
)

target_link_libraries(installer PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
)

if(WIN32)
    target_link_libraries(installer PRIVATE
        advapi32
    )
    set_target_properties(installer PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
    # 禁用默认 manifest，使用自定义 manifest
    target_link_options(installer PRIVATE "/MANIFEST:NO"
    )
endif()
