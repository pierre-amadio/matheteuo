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

/*
simpleOsiVerseParser is a class that parse a verse in Osis format.

The constructor requires an osisVerse string and a module name.
Sample osisVerse: (Luke 20:30) for MorphGNT

<w lemma="lemma.Strong:καί strong:G2532" morph="robinson:CONJ" wn="001">καὶ</w> <w lemma="lemma.Strong:ὁ strong:G3588" morph="robinson:T-NSM" wn="002">⸂ὁ</w> <w lemma="lemma.Strong:δεύτερος strong:G1208" morph="robinson:A-NSM" wn="003">δεύτερος⸃</w><milestone type="line"/>
: <w lemma="lemma.Strong:καί strong:G2532" morph="robinson:CONJ" wn="001">καὶ</w> <w lemma="lemma.Strong:ὁ strong:G3588" morph="robinson:T-NSM" wn="002">⸂ὁ</w> <w lemma="lemma.Strong:δεύτερος strong:G1208" morph="robinson:A-NSM" wn="003">δεύτερος⸃</w><milestone type="line"/>

The instance store a representation of the verse in a QList of verseChunk.
Each nodes ends up in a new verseChunk so the following variables can be accessed: isXmlTag/fullWord/rootValue/strong/morph
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











