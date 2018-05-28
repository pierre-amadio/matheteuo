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

#ifndef VERSECHUNK_H
#define VERSECHUNK_H
#include<QString>
/*
A class (well, it could have been a simple structure)
storing the content of an OSIS node.
isXmlTag is a flag set to true if the node has some extra information
such as Strong id or morphological informations.
*/
class verseChunk
{
public:
    verseChunk();
    bool isXmlTag;
    QString fullWord;
    QString rootValue;
    QString strong;
    QString morph;
    void setIsXmlTag(bool i);

};

#endif // VERSECHUNK_H
