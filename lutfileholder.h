#ifndef LUTFILEHOLDER_H
#define LUTFILEHOLDER_H

#include <QFile>
#include <QString>
#include <QDebug>
#include <QTextStream>
#include <QRegularExpression>
#include <vector>

class LUTFileHolder
{
public:
    LUTFileHolder();
    static void load_file(QString fn);
    static QString get_info_string();
private:
    static void read_file(QFile &f);

    static int lutSize;
    static QFile currentFile;

    static std::vector<double> spacing;
    static std::vector<std::vector<double>> lut;
};

#endif // LUTFILEHOLDER_H
