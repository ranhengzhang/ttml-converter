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
#include <QApplication>
#include <QTextStream>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSettings>
#include <QInputDialog>

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

std::unique_ptr<QSettings> config;

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

QString toSafeFileName(QString name) {
    static QRegularExpression illegal_chars(R"([\\/:*?"<>|])");

    QString safe_name = name.replace(illegal_chars, "_");

    for (int i = 0; i < safe_name.length(); ++i) {
        if (safe_name.at(i).unicode() < 32) {
            safe_name[i] = '_';
        }
    }

    return safe_name.trimmed();
}

template <typename T>
QMap<QString, T> convertKeysToUpperCaseNoSpace(const QMap<QString, T> &old_map) {
    QMap<QString, T> new_map;

    for (auto it = old_map.begin(); it != old_map.end(); ++it) {
        QString old_key = it.key();

        QString new_key = old_key.remove("_").remove("-").remove(" ").toUpper();

        new_map.insert(new_key, it.value());
    }

    return new_map;
}

QString generateOutputFileName(const QString& original_filename, QString output_template, LyricObject &lyric_object) {
    auto original_map = lyric_object.getMeta();

    original_map["filename"] = {original_filename};
    original_map["songWriters"].append(lyric_object.getSongWriter());

    auto temp_map = convertKeysToUpperCaseNoSpace(original_map);

    temp_map["TITLE"] = temp_map["MUSICNAME"];
    temp_map["ARTIST"] = temp_map["ARTISTS"];

    QMap<QString, QString> template_map;

    for (const auto &[key, value]: temp_map.asKeyValueRange()) {
        template_map[QString("ALL") + key] = temp_map[key].join("／");
        template_map[key] = temp_map[key].first();
    }

    const QRegularExpression re(R"(%([A-z]+)%)");
    auto ite = re.globalMatch(output_template);
    QList<QRegularExpressionMatch> matches;

    while (ite.hasNext()) {
        matches.append(ite.next());
    }

    for (int i = matches.size() - 1; i >= 0; --i) {
        const auto &match = matches.at(i);

        output_template.replace(match.capturedStart(1), match.capturedLength(1), match.captured(1).remove("_").remove("-").remove(" ").toUpper());
    }

    for (const auto &[key, value]: template_map.asKeyValueRange()) {
        output_template.replace(QString("%") + key + "%", toSafeFileName(value));
    }

    return output_template;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QApplication::setApplicationName("ttml-converter");
    QApplication::setApplicationVersion(PROJECT_VERSION);

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

    const QCommandLineOption config_option(
        QStringList() << "c" << "config",
        "配置默认文件名模板"
    );

    parser.addOption(format_option);
    parser.addOption(output_option);
    parser.addOption(config_option);
    parser.addPositionalArgument("file", "输入的 TTML 文件路径");
    parser.process(a);

    const QString config_path = QApplication::applicationDirPath() + "/settings.ini";
    config = std::make_unique<QSettings>(config_path, QSettings::IniFormat);

    if (parser.isSet(config_option)) {
        auto hook = std::make_shared<QWidget>(new QWidget());
        bool ok;
        auto old_val = config->contains("template") ? config->value("template").toString() : "";
        auto new_val = QInputDialog::getText(hook.get(), "配置默认文件名模板", "请输入默认文件名模板", QLineEdit::Normal, old_val, &ok);

        if (ok) {
            config->setValue("template", new_val);
        }

        return 0;
    }

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
        output_template = config->contains("template") ? config->value("template").toString() : "%filename%";
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

    output_file_name = generateOutputFileName(base_name, output_template, lyric_object) + "." + extension;

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
