#include "lutfileholder.h"

int LUTFileHolder::lutSize = -1;
std::vector<double> LUTFileHolder::spacing;
std::vector<std::vector<double>> LUTFileHolder::lut;


LUTFileHolder::LUTFileHolder()
{

}
void LUTFileHolder::load_file(QString fn){
    QFile currentFile(fn);
    
    if(!currentFile.open(QIODevice::ReadOnly)){
        qDebug() << "Could not open " << fn;
        return;
    }

    read_file(currentFile);

    qDebug() << fn;
    return;
}

void read_next_nocomment(QTextStream &ts, QString &line){
    while(!ts.atEnd()){
        line = ts.readLine();
        if(!line.startsWith("#") && !line.isEmpty()){
            return;
        }
    }
}

void LUTFileHolder::read_file(QFile &f){
    QTextStream in(&f);
    QString line;

    lutSize = -1;

    read_next_nocomment(in, line);

    int state=0;

    /* States:
     * 0:    Header
     * 1:    Body
     */

    while (!in.atEnd()){
        if(state==0){
            //Header section
            if(line.startsWith("LUT_3D_SIZE")){

                int size = line.mid(11).toInt();

                //Ensure size match
                if(lutSize == -1){
                    lutSize = size;
                }else if(lutSize != size){
                    qDebug() << line;
                    qDebug() << "LUT SIZE MISMATCH " << lutSize << "/" << size;
                }
            }else if(QRegularExpression("(([0-9]\\.)?[0-9]+(\\s)+){3,}([0-9]\\.)?[0-9]+").match(line).hasMatch()){
                //LUT input spacing, more than three numbers on a line

                QStringList spc = line.split(QRegularExpression("\\s+"));

                //Ensure size match
                if(lutSize == -1){
                    lutSize = spc.length();
                }else if(lutSize != spc.length()){
                    qDebug() << line;
                    qDebug() << "LUT SIZE MISMATCH " << lutSize << "/" << spc.length();
                }

                for(int i = 0; i < spc.length(); ++i){
                    double spacing_val = spc[i].toDouble();
                    if(spacing_val >= 1){
                        spacing_val = spacing_val / 1024.0;
                    }
                    spacing.push_back(spacing_val);
                }
            }else if(QRegularExpression("(([0-9]\\.)?[0-9]+(\\s)+){2}([0-9]\\.)?[0-9]+").match(line).hasMatch()){
                state = 1;

                //Default uniform spacing
                if(spacing.empty()){
                    for(int i = 0; i < lutSize; ++i){
                        spacing.push_back((double)i/(lutSize-1));
                    }
                }

                qDebug() << "State change: 1";
            }
            else{
                qDebug() << "unparsed line: \n" << line;
            }
        }else if(state == 1){
            QStringList s = line.split(QRegularExpression("\\s+"));
            std::vector<double> ret;
            ret.push_back(s[0].toDouble());
            ret.push_back(s[1].toDouble());
            ret.push_back(s[2].toDouble());
            lut.push_back(ret);
        }
        read_next_nocomment(in, line);
    }

}


QString LUTFileHolder::get_info_string(){
    QString out = "";
    out += "Size: ";
    out += QString::number(lutSize);
    out += "\nSpacing: ";
    for(int i = 0; i < spacing.size(); ++i){
        out += QString::number(spacing[i]) + " ";
    }

    return out;
}
