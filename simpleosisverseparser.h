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

#ifndef SIMPLEOSISVERSEPARSER_H
#define SIMPLEOSISVERSEPARSER_H
#include<QString>
#include<QList>
#include "versechunk.h"

class simpleOsisVerseParser
{
public:
    simpleOsisVerseParser(QString osisVerse,QString moduleName);
    QList<verseChunk> getVerselist();

private:
    QList<verseChunk> verseChunkList;
};

#endif // SIMPLEOSISVERSEPARSER_H











