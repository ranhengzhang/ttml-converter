//  TTML Converter - A tool for converting TTML subtitles to various formats
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
#include <QCoreApplication>
#include <QTextStream>
#include <QFileInfo>

#include "version.h"

#include "src/lyric/lyric_object/LyricObject.hpp"

#pragma comment( linker, R"(/subsystem:"windows" /entry:"mainCRTStartup")" ) // 设置入口地址

const QStringList supported_formats = {
    "ass",
    "lrc",
    "spl",
    "lys",
    "qrc",
    "yrc",
    "krc",
    "txt",
    "ttml",
    "amll"
};

const std::map<QString, QString> special_ext = {
    {"spl", "lrc"},
    {"amll", "ttml"}
};

QString convert(LyricObject &lyric_object, const QString &type) {
    // ReSharper disable once CppTooWideScope
    const auto index = supported_formats.indexOf(type.toLower());

    switch (index) {
        case 0: {
            return lyric_object.toASS();
        }
        case 1: {
            return lyric_object.toLRC("");
        }
        case 2: {
            return lyric_object.toSPL();
        }
        case 3: {
            auto [orig, trans] = lyric_object.toLYS("");
            return orig;
        }
        case 4: {
            auto [orig, trans, roma] = lyric_object.toQRC("", "");
            return orig;
        }
        case 5: {
            auto [orig, trans] = lyric_object.toYRC("");
            return orig;
        }
        case 6: {
            return lyric_object.toKRC("", "");
        }
        case 7: {
            return lyric_object.toTXT();
        }
        case 8: {
            return lyric_object.toTTML();
        }
        case 9: {
            return lyric_object.toAMLL();
        }
        default: {
            return {};
        }
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QCoreApplication::setApplicationName("ttml-converter");
    QCoreApplication::setApplicationVersion(PROJECT_VERSION);

    QCommandLineParser parser;

    parser.setApplicationDescription("TTML 字幕转换工具");
    parser.addHelpOption();
    parser.addVersionOption();

    const QCommandLineOption format_option(
        QStringList() << "f" << "format",
        "输出格式",
        "format"
    );

    const QCommandLineOption output_option(
        QStringList() << "o" << "output",
        "输出文件名模板",
        "output"
    );

    parser.addOption(format_option);
    parser.addOption(output_option);
    parser.addPositionalArgument("file", "输入的 TTML 文件路径");
    parser.process(a);

    const QStringList args = parser.positionalArguments();

    if (args.isEmpty()) {
        return 1;
    }

    const QString &input_file = args.first();

    if (!QFile::exists(input_file)) {
        return 1;
    }

    if (!parser.isSet(format_option)) {
        return 1;
    }

    const QString format = parser.value(format_option);

    if (!supported_formats.contains(format.toLower())) {
        return 1;
    }

    QString output_template = parser.value(output_option);

    if (output_template.isEmpty()) {
        output_template = "%filename%";
    }

    QFile file(input_file);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return 1;
    }

    QTextStream in(&file);

    in.setEncoding(QStringConverter::Utf8);

    auto text = in.readAll();

    file.close();

    auto [ttml_content, ttml_status] = lyric::utils::compressTtml(text);

    if (ttml_status != lyric::utils::Status::Success) {
        return 1;
    }

    auto [lyric_object, generate_status] = LyricObject::fromTTML(ttml_content);

    if (generate_status != LyricObject::Status::Success) {
        return 1;
    }

    QFileInfo input_info(input_file);
    QString output_dir = input_info.path();
    QString base_name = input_info.baseName();
    QString output_file_name;
    QString extension = format.toLower();

    if (auto index = special_ext.find(extension); index != special_ext.end()) {
        extension = index->second;
    }

    if (output_template == "%filename%") {
        output_file_name = base_name + "." + extension;
    } else {
        output_file_name = output_template + "." + extension;
    }

    QString output_path = output_dir + "/" + output_file_name;

    if (QFile::exists(output_path)) {
        QString name_without_ext = output_template == "%filename%" ? base_name : output_template;
        int counter = 1;

        do {
            output_file_name = QString("%1 (%2).%3").arg(name_without_ext).arg(counter).arg(extension);
            output_path = output_dir + "/" + output_file_name;
            counter++;
        } while (QFile::exists(output_path));
    }

    QString output_content;

    try {
        output_content = convert(lyric_object, format.toLower());
    } catch ([[maybe_unused]] const std::exception &e) {
        return 1;
    }

    QFile out_file(output_path);

    if (!out_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return 1;
    }

    QTextStream out(&out_file);

    out.setEncoding(QStringConverter::Utf8);
    out << output_content;
    out_file.close();

    return 0;
}
