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
#include "moduleinfo.h"
#include <QDebug>

moduleInfo::moduleInfo(QObject *parent)
    : QObject(parent)
{
}


QString  moduleInfo::getName() const {
    //return "coin";
    //qDebug()<<"moduleInfo::getName";
    return moduleName;
}

void moduleInfo::setName(const QString name) {
    //qDebug()<<"moduleInfo::setName";

    if(moduleName!=name){
        moduleName=name;
        emit notifyName();
      }
}


QString  moduleInfo::getLang() const {
    return this->moduleLang;
}


void moduleInfo::setLang(const QString lang) {
    this->moduleLang=lang;
    this->notifyLang();
}

QString  moduleInfo::getType() const {
    return this->moduleType;
}


void moduleInfo::setType(const QString type) {
    this->moduleType=type;
    this->notifyType();
}
