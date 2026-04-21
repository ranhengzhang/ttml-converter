#  TTML Converter - A tool for converting TTML subtitles to various formats
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

# 先引入 lyric 子项目，获取其版本号
add_subdirectory(src/lyric)

project(converter VERSION 0.2.1.${LYRIC_VERSION_COMBINED})

message(STATUS "converter version: ${PROJECT_VERSION}")

set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Xml Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Xml Gui)

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

add_executable(converter
    main.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
)

set_target_properties(converter PROPERTIES
    OUTPUT_NAME "ttml-converter"
)

target_link_libraries(converter PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Xml
    Qt${QT_VERSION_MAJOR}::Gui
    LyricParser::lyric
)
