#ifndef STRINGMANAGER_H
#define STRINGMANAGER_H

#include <QMap>
#include <QFile>

//! This class will be responsible for reading and writing strings into/from file.
class StringManager
{
public:
    //! String manager header.
    struct SMHeader
    {
        int numStrings; //!< How many strings are in data block.
    };

    //! Data header.
    struct DataHeader
    {
        int id;     //!< String id.
        int length; //!< Data length.
    };

    StringManager() {}
    ~StringManager() {}

    void clear() { _data.clear(); }
    void insert(int key, const QString &string) { _data[key] = string; }
    const QString &string(int key) { return _data[key]; }

    void load(QFile &file);
    void save(QFile &file);

private:
    QMap<int, QString> _data;
};

#endif // STRINGMANAGER_H
