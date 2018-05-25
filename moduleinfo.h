/*
    This file is part of Matheteuo, a tool to help learning koine
    greek and hebrew.

    Copyright 2018 Pierre Amadio <pierre.amadio@laposte.net>

    Matheteuo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Matheteuo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Matheteuo.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MODULEINFO_H
#define MODULEINFO_H

#include <QObject>

class moduleInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY notifyName)
    Q_PROPERTY(QString lang READ getLang WRITE setLang NOTIFY notifyLang)
    Q_PROPERTY(QString type READ getType WRITE setType NOTIFY notifyType)

public:
    moduleInfo(QObject *parent=0);
    moduleInfo(const QString &name, QObject *parent=0);

    QString getName() const;
    void setName(const QString name);

    QString getLang() const;
    void setLang(const QString lang);

    QString getType() const;
    void setType(const QString type);

signals:
    void notifyName();
    void notifyLang();
    void notifyType();


private:
    QString moduleName;
    QString moduleLang;
    QString moduleType;
};

#endif // MODULEINFO_H
