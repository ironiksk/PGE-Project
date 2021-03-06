/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pge_x.h"
#include "file_strlist.h"

namespace PGEExtendedFormat
{
    QRegExp section_title = QRegExp("^[A-Z0-9_]*$");

    QRegExp qstr = QRegExp("^\"(?:[^\"\\\\]|\\\\.)*\"$");
    QRegExp heximal = QRegExp("^[0-9a-fA-F]+$");

    QRegExp boolean = QRegExp("^(1|0)$");

    QRegExp usig_int = QRegExp("\\d+");     //Check "Is Numeric"
    QRegExp sig_int = QRegExp("^[\\-0]?\\d*$");     //Check "Is signed Numeric"

    QRegExp floatptr = QRegExp("^[\\-]?(\\d*)?[\\(.|,)]?\\d*[Ee]?[\\-\\+]?\\d*$");     //Check "Is signed Float Numeric"

    //Arrays
    QRegExp boolArray = QRegExp("^[1|0]+$");
    QRegExp intArray = QRegExp("^\\[(\\-?\\d+,?)*\\]$"); // ^\[(\-?\d+,?)*\]$

}

PGEFile::PGEFile(QObject *parent)
    : QObject(parent)
{
    _lastError = "";
    rawData = "";
}


PGEFile::PGEFile(PGEFile &pgeFile, QObject *parent)
    : QObject(parent)
{
    rawData = pgeFile.rawData;
    rawDataTree = pgeFile.rawDataTree;
    _lastError = pgeFile._lastError;
}

PGEFile::PGEFile(QString _rawData)
{
    rawData = _rawData;
    _lastError = "";
}

QString PGEFile::removeQuotes(QString str)
{
    QString target = str.remove(0,1);
    target = target.remove(target.size()-1,1);
    return target;
}

void PGEFile::setRawData(QString _rawData)
{
    rawData = _rawData;
}

bool PGEFile::buildTreeFromRaw()
{
    PGEXSct PGEXsection;

    FileStringList in;
    in.addData( rawData );

    //Read raw data sections
    bool sectionOpened=false;
    while(!in.atEnd())
    {
        PGEXsection.first = in.readLine();
        PGEXsection.second.clear();

        if(QString(PGEXsection.first).remove(' ').isEmpty()) continue; //Skip empty strings

        sectionOpened=true;
        QString data;
        while(!in.atEnd())
        {
            data = in.readLine();
            if(data==PGEXsection.first+"_END") {sectionOpened=false; break;} // Close Section
            PGEXsection.second.push_back(data);
        }
        rawDataTree.push_back(PGEXsection);
    }

    if(sectionOpened)
    {
        _lastError=QString("Section [%1] is not closed").arg(PGEXsection.first);
        return false;
    }

    //Building tree

    for(int z=0; z< rawDataTree.size(); z++)
    {
        bool valid=true;
        PGEX_Entry subTree = buildTree( rawDataTree[z].second, &valid );
        if(valid)
        {   //Store like subtree
            subTree.type = PGEX_Struct;
            subTree.name = rawDataTree[z].first;
            dataTree.push_back( subTree );
        }
        else
        {   //Store like plain text
            PGEX_Item dataItem;
            dataItem.type = PGEX_PlainText;
            subTree.data.clear();
            subTree.subTree.clear();
            PGEX_Val dataValue;
                dataValue.marker = "PlainText";
                foreach(QString x, rawDataTree[z].second) dataValue.value += x+"\n";
            dataItem.values.push_back(dataValue);
            subTree.name = rawDataTree[z].first;
            subTree.type = PGEX_PlainText;
            subTree.data.push_back(dataItem);
            dataTree.push_back( subTree );
            valid = true;
        }
    }

    return true;
}


PGEFile::PGEX_Entry PGEFile::buildTree(QStringList &src_data, bool *_valid)
{
    PGEX_Entry entryData;

    bool valid=true;
    for(int q=0; q<src_data.size(); q++)
    {
        if( IsSectionTitle(QString(src_data[q]).remove(' ') ) )
        {//Build and store subTree
            QString nameOfTree = QString(src_data[q]).remove(' ');
            QStringList rawSubTree;
            q++;
            for(; q<src_data.size() && src_data[q] != nameOfTree+"_END" ;q++)
            {
                rawSubTree.push_back( src_data[q] );
            }
            PGEX_Entry subTree = buildTree( rawSubTree, &valid );
            if(valid)
            {   //Store like subtree
                subTree.name = nameOfTree;
                entryData.subTree.push_back( subTree );
                entryData.type = PGEX_Struct;
            }
            else
            {   //Store like plain text
                subTree.name = nameOfTree;
                subTree.subTree.clear();
                subTree.type = PGEX_PlainText;
                subTree.data.clear();

                PGEX_Item dataItem; PGEX_Val dataValue;
                dataItem.type = PGEX_PlainText;

                dataValue.marker = nameOfTree;
                foreach(QString x, rawSubTree) dataValue.value += x+"\n";
                dataItem.values.push_back(dataValue);
                subTree.data.push_back(dataItem);
                entryData.subTree.push_back( subTree );
                entryData.type = PGEX_Struct;
                valid = true;
            }

        }
        else
        {
            QStringList fields = encodeEscape(src_data[q]).split(';');
            PGEX_Item dataItem;
            dataItem.type = PGEX_Struct;
            for(int i=0;i<fields.size();i++)
            {
                if(QString(fields[i]).remove(' ').isEmpty()) continue;

                //Store data into list
                QStringList value = fields[i].split(':');

                if(value.size()!=2)
                {
                    valid=false; break;
                }

                PGEX_Val dataValue;
                    dataValue.marker = value[0];
                    dataValue.value = value[1];
                dataItem.values.push_back(dataValue);
            }
            entryData.type = PGEX_Struct;
            entryData.data.push_back(dataItem);
        }
        if(!valid) break;
    }

    if(_valid) *_valid = valid;
    return entryData;
}



QString PGEFile::lastError()
{
    return _lastError;
}


bool PGEFile::IsSectionTitle(QString in)
{
    using namespace PGEExtendedFormat;
    return section_title.exactMatch(in);
}


//validatos
bool PGEFile::IsQStr(QString in) // QUOTED STRING
{
    using namespace PGEExtendedFormat;
    return qstr.exactMatch(in);
}

bool PGEFile::IsHex(QString in) // Heximal string
{
    using namespace PGEExtendedFormat;
    return heximal.exactMatch(in);
}

bool PGEFile::IsBool(QString in) // Boolean
{
    using namespace PGEExtendedFormat;
    return boolean.exactMatch(in);
}

bool PGEFile::IsIntU(QString in) // Unsigned Int
{
    using namespace PGEExtendedFormat;
    return usig_int.exactMatch(in);
}

bool PGEFile::IsIntS(QString in) // Signed Int
{
    using namespace PGEExtendedFormat;
    return sig_int.exactMatch(in);
}

bool PGEFile::IsFloat(QString in) // Float Point numeric
{
    using namespace PGEExtendedFormat;
    return floatptr.exactMatch(in);
}


bool PGEFile::IsBoolArray(QString in) // Boolean array
{
    using namespace PGEExtendedFormat;
    return boolArray.exactMatch(in);
}

bool PGEFile::IsIntArray(QString in) // Boolean array
{
    using namespace PGEExtendedFormat;
    return intArray.exactMatch(in);
}

bool PGEFile::IsStringArray(QString in) // String array
{
    using namespace PGEExtendedFormat;
    bool valid=true;
    int i=0, depth=0, comma=0;
    while(i<in.size())
    {
        switch(depth)
        {
        case 0://outside array
                if(in[i]=='[') {depth=1;comma++;}
                else valid=false;
             break;
        case 1://between entries
            switch(comma)
            {
                case 0:
                         if(in[i]==']') depth=0;//Close array
                    else if(in[i]==',') comma++;//Close array
                    else valid=false;
                    break;
                case 1:
                    if(in[i]=='"')     depth=2;//Open value
                    else valid=false;
            }
            break;
        case 2://Inside entry
                 if(in[i]=='"'){ depth=1;comma=0;}    //Close value
            else if(in[i]=='['){ valid=false;}
            else if(in[i]==']'){ valid=false;}
            else if(in[i]==','){ valid=false;}
            break;
        }
        if(!valid) break;//Stop parsing on invalid
        i++;
    }
    return valid;
}


QStringList PGEFile::X2STRArr(QString src)
{
    QStringList strArr;
    src.remove("[").remove("]");
    strArr = src.split(',');
    for(int i=0; i<strArr.size(); i++)
    {
        strArr[i] = X2STR(strArr[i]);
    }

    return strArr;
}

QList<bool > PGEFile::X2BollArr(QString src)
{
    QList<bool > arr;

    for(int i=0;i<src.size();i++)
        arr.push_back(src[i]=='1');

    return arr;
}

QList<QStringList > PGEFile::splitDataLine(QString src_data, bool *valid)
{
    QList<QStringList > entryData;

    bool wrong=false;

    QStringList fields = encodeEscape(src_data).split(';');
    for(int i=0;i<fields.size();i++)
    {
        if(QString(fields[i]).remove(' ').isEmpty()) continue;

        QStringList value = fields[i].split(':');

        if(value.size()!=2) {wrong=true; break;}
        entryData.push_back(value);
    }

    if(valid) * valid = (!wrong);
    return entryData;
}


QString PGEFile::IntS(long input)
{
    return QString::number(input);
}

QString PGEFile::BoolS(bool input)
{
    return QString((input)?"1":"0");
}

QString PGEFile::FloatS(double input)
{
    return QString::number(input, 'g', 10);
}

QString PGEFile::qStrS(QString input)
{
    return "\""+escapeStr(input)+"\"";
}

QString PGEFile::hStrS(QString input)
{
    return input.toLatin1();
}

QString PGEFile::strArrayS(QStringList input)
{
    QString output;

    if(input.isEmpty()) return QString("");

    output.append("[");

    for(int i=0; i< input.size(); i++)
    {
        output.append(qStrS(input[i])+(i<input.size()-1?",":""));
    }

    output.append("]");
    return output;
}

QString PGEFile::intArrayS(QList<int> input)
{
    QString output;
    if(input.isEmpty()) return QString("");
    output.append("[");
    for(int i=0; i< input.size(); i++)
    {
        output.append(QString::number(input[i])+(i<input.size()-1?",":""));
    }
    output.append("]");
    return output;
}

QString PGEFile::BoolArrayS(QList<bool> input)
{
    QString output;
    foreach(bool b, input)
    {
        output.append(b?"1":"0");
    }
    return output;
}

QString PGEFile::encodeEscape(QString input)
{
    QString output = input;
    output.replace("\\n", "==n==");
    output.replace("\\\"", "==q==");
    output.replace("\\\\", "==sl==");
    output.replace("\\;", "==Sc==");
    output.replace("\\:", "==Cl==");
    output.replace("\\[", "==Os==");
    output.replace("\\]", "==Cs==");
    output.replace("\\,", "==Cm==");
    output.replace("\\%", "==Pc==");
    return output;
}

QString PGEFile::decodeEscape(QString input)
{
    QString output = input;
    output.replace("==n==", "\\n");
    output.replace("==q==", "\\\"");
    output.replace("==sl==","\\\\");
    output.replace("==Sc==", "\\;");
    output.replace("==Cl==", "\\:");
    output.replace("==Os==", "\\[");
    output.replace("==Cs==", "\\]");
    output.replace("==Cm==", "\\,");
    output.replace("==Pc==", "\\%");
    return output;
}

QString PGEFile::X2STR(QString input)
{
    return restoreStr(
                decodeEscape(
                    removeQuotes(input)
                )
            );
}

QString PGEFile::restoreStr(QString input)
{
    QString output = input;
    output.replace("\\n", "\n");
    output.replace("\\\"", "\"");
    output.replace("\\;", ";");
    output.replace("\\:", ":");
    output.replace("\\[", "[");
    output.replace("\\]", "]");
    output.replace("\\,", ",");
    output.replace("\\%", "%");
    output.replace("\\\\", "\\");
    return output;
}

QString PGEFile::escapeStr(QString input)
{
    QString output = input;
    output.replace("\\", "\\\\");
    output.replace("\n", "\\n");
    output.replace("\"", "\\\"");
    output.replace(";", "\\;");
    output.replace(":", "\\:");
    output.replace("[", "\\[");
    output.replace("]", "\\]");
    output.replace(",", "\\,");
    output.replace("%", "\\%");
    return output;
}

QString PGEFile::value(QString marker, QString data)
{
    QString out;
    out +=marker+":"+data+";";
    return out;
}
