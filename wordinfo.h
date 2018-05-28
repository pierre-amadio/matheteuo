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
#ifndef WORDINFO_H
#define WORDINFO_H
#include <QString>
/*
wordInfo is a class representing a word in an OSIS verse.
the qml model used in the verseView is a list of such objects.
*/
class wordInfo
{

public:
    wordInfo();

    QString getDisplayWord() const;
    void setDisplayWord(const QString cn);
    //the word as shown in the verse. (εὐαγγελίου)
    QString displayWord;
    //is there juicy information such as morph code
    //or strong id ?
    bool hasInfo;
    //the actual root of the word (εὐαγγέλιον)
    QString rootWord;
    //raw morphCode
    QString morphCode;
    //morph Code fit for being displayed to the user.
    QString morphDesciption;
    //raw strong id
    QString StrongId;
    //strong dictionnary entry fit for being displayed to user.
    QString StrongDescription;

};

#endif // WORDINFO_H
