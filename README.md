# TTML Converter

## 概述

TTML Converter 是一个用于将 TTML 字幕/歌词格式转换为多种其他格式的桌面工具。它基于 C++ Qt 开发，支持命令行操作，可以将 TTML 格式的歌词转换为 LRC、QRC、YRC、KRC、ASS、SPL、LYS 等主流歌词格式。

本项目包含两个主要组件：
- **Converter**：TTML 字幕转换器核心程序
- **Installer**：TTML Converter 的安装程序


## 构建项目

### 环境要求

- Windows 10 或更高版本
- C++23 或更高版本
- Qt 6.x
- CMake 3.14+
- MSVC (Visual Studio 2022 或更高版本)

### 构建命令

```bash
# 配置项目
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# 编译项目
cmake --build build
```

## 使用说明

直接通过命令行运行或通过 installer 安装后，右键点击任意 TTML 文件，即可在菜单中找到 TTML Converter 选项。

### 命令行运行

```bash
ttml-converter [选项] <输入文件>
```

#### 选项

| 选项 | 说明 |
|------|------|
| `-h, --help` | 显示帮助信息 |
| `-v, --version` | 显示版本信息 |
| `-f, --format <format>` | 指定输出格式（必需） |
| `-o, --output <template>` | 指定输出文件名模板（可选，默认为 `%filename%`） |

#### 支持的输出格式

| 格式 | 说明 |
|------|------|
| `ass` | ASS 字幕格式 |
| `lrc` | LRC 歌词格式 |
| `spl` | SPL 歌词格式 |
| `lys` | LYS 歌词格式 |
| `qrc` | QRC 歌词格式 |
| `yrc` | YRC 歌词格式 |
| `krc` | KRC 歌词格式 |
| `txt` | 纯文本格式 |

#### 使用示例

```bash
# 将 TTML 转换为 LRC 格式
ttml-converter -f lrc input.ttml

# 将 TTML 转换为 ASS 格式，并指定输出文件名
ttml-converter -f ass -o %title% input.ttml

# 显示帮助
ttml-converter --help
```

## 许可

本项目采用 GNU 通用公共许可证第 3 版（GPL-3.0-or-later）授权。

```
TTML Converter - A tool for converting TTML subtitles to various formats
Copyright (C) 2026  ranhengzhang

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
```

完整的许可证文本请参见 [LICENSE](LICENSE) 文件。

## 致谢

![](https://skills.syvixor.com/api/icons?perline=15&i=qtwidgets,clion,trae,deepseek,windows)
