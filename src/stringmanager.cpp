#include "stringmanager.h"

//! What it does:
//! 1) clears _data
//! 2) loads SMHeader
//! 3) loads every DataHeader and data associated with it into _data
void StringManager::load(QFile &file)
{
    SMHeader smHeader;
    DataHeader dataHeader;

    if (!file.isOpen() || !file.isReadable())
        return;

    clear();

    file.read((char*)&smHeader, sizeof(smHeader));

    for (int i = 0; i < smHeader.numStrings; ++i)
    {
        file.read((char*) &dataHeader, sizeof(dataHeader));
        _data[dataHeader.id] = file.read(dataHeader.length);
    }
}

//! What it does:
//! 1) writes to file SMHeader
//! 2) dumps every string from _data into the file
void StringManager::save(QFile &file)
{
    if (!file.isOpen() || !file.isWritable())
        return;

    SMHeader smHeader;
    DataHeader dataHeader;
    QMap<int, QString>::const_iterator i = _data.begin();

    smHeader.numStrings = 0;
    for (; i != _data.end(); ++i)
        if (i.value() != "")
            ++smHeader.numStrings;
    file.write((char*) &smHeader, sizeof(smHeader));

    for (i = _data.begin(); i != _data.end(); ++i)
    {
        if (i.value() == "")
            continue;
        dataHeader.id = i.key();
        dataHeader.length = i.value().toStdString().length();
        file.write((char*) &dataHeader, sizeof(dataHeader));
        file.write(i.value().toStdString().c_str());
    }
}
