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

class LUTPoint{
public:
    LUTPoint(double fx, double fy, double fz, double tx, double ty, double tz);
    LUTPoint(std::vector<double> f,std::vector<double> t);
    double data[6];
};

    LUTFileHolder();
    static void load_file(QString fn);
    static QString get_info_string();

    static std::vector<LUTPoint> get_points();
private:
    static void read_file(QFile &f);

    static int lutSize;
    static QFile currentFile;

    static std::vector<double> spacing;
    static std::vector<std::vector<double>> lut;
};

#endif // LUTFILEHOLDER_H
