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
#include "simpleosisverseparser.h"
#include <QString>
#include <QDebug>
#include "versechunk.h"
#include <QXmlStreamReader>
#include <utilxml.h>
#include <QRegularExpression>
#include <QChar>
using namespace::sword;


simpleOsisVerseParser::simpleOsisVerseParser(QString verse, QString curModule)
{

    QList<QString> wordList;
    int maxLenght=verse.length();
    QString newWord("");

    for(int n=0;n<maxLenght;n++){

        if(n>1 && verse.mid(n-2,2)=="<w") {
            if(newWord.mid(0,newWord.length()-2).length()>0){
                wordList.append(newWord.mid(0,newWord.length()-2));
                newWord="<w";
            }
        }

        newWord.append(verse[n]);

        if (n>3){
            if(verse.mid(n-3,4)=="</w>" ) {
                wordList.append(newWord);
                newWord="";
            }
        }
    }

    if(newWord.length()>0){
        wordList.append(newWord);
    }


    foreach( QString curWord, wordList ) {
        QString tmpWord=curWord;

        verseChunk tmpChunk;

        if(curWord.mid(0,2)=="<w") {
            tmpChunk.setIsXmlTag(true);
            QString tmpRoot="none found";
            QString tmpStrong="none found";
            QString tmpMorph="none found";
            QString tmpFullWord="none found";

            XMLTag xmlTag;

            QXmlStreamReader reader(curWord);

            while(!reader.atEnd() && !reader.hasError()) {
                if(reader.readNext() == QXmlStreamReader::StartElement && reader.name() == QChar('w')) {
                    tmpFullWord=reader.readElementText();
                }
                if(reader.hasError()) {
                    qDebug()<<"error:"<<curWord<<"\n";
                    qDebug()<< "\n\nreader error: " << reader.errorString() << "\n";
                }
            }


            xmlTag.setText(curWord.toUtf8());

            StringList attributes = xmlTag.getAttributeNames();

            for (StringList::iterator it = attributes.begin(); it != attributes.end(); it++) {
                QString attributeName=it->c_str();

                if(curModule=="MorphGNT") {

                    if(attributeName=="lemma") {
                        tmpStrong=xmlTag.getAttribute("lemma", 1, ' ');
                        tmpRoot=xmlTag.getAttribute("lemma", 0, ' ');
                    } else if (attributeName=="morph"){
                        tmpMorph=xmlTag.getAttribute("morph", 0, ' ');
                    } else {
                        qDebug()<<"unknown attributeName"<<attributeName;
                    }
                } else if(curModule=="OSHB") {
                   if(attributeName=="lemma") {
			int goodInd=0;
			while(1){
				/* 
				Since OSHB 2.1 the content of the lemma tag has changed:
				diatheke -b OSHB -o avlmn -f OSIS -k Genesis 1:1
			 
				Version 1.4 gives:
				<w lemma="strong:H07225" morph="oshm:HR/Ncfsa" n="1.0">בְּרֵאשִׁית</w>
					 
				2.1 now gives:
				<w lemma="strong:Hb strong:H7225" morph="oshm:HR/Ncfsa">בְּרֵאשִׁית</w>

				This strong:Hb with an invalid strong id result in the strong id for this node being 8674 (תּתּני / Tatnai)
				Let's make sure the strong attribute we pick is an actual number.

				*/ 
				QString testString="";	
				testString=xmlTag.getAttribute("lemma",goodInd,' ');

				QRegularExpression re("H\\d+",QRegularExpression::CaseInsensitiveOption);
				QRegularExpressionMatch match = re.match(testString);
				if(match.hasMatch()){
					//qDebug()<<"BINGO"<<testString;
					tmpStrong=testString;
				}
			if(not testString.length()){ break;}
				goodInd++;
			}
			//qDebug()<<"good ind"<<goodInd;

                        //tmpStrong=xmlTag.getAttribute("lemma",0,' ');
                    } else if (attributeName=="morph") {
                        tmpMorph=xmlTag.getAttribute("morph",0,' ');
                    } else if (attributeName=="n") {
                        //qDebug()<<"What to do with "<<xmlTag;
                    } else {
                        qDebug()<<"unknown attributeName"<<attributeName;
                    }

                }  else if(curModule=="LXX"){
                    if(attributeName=="lemma"){
                        tmpStrong=xmlTag.getAttribute("lemma",0,' ');
                    } else if(attributeName=="morph"){
                        tmpMorph=xmlTag.getAttribute("morph",0,' ');
                    } else {
                        qDebug()<<"unknown attribue:"<<attributeName;
                    }

                }else {
                    qDebug()<<"Unkown module:"<<curModule;
                }
            }

            tmpChunk.fullWord=tmpFullWord;
            tmpChunk.rootValue=tmpRoot;
            tmpChunk.strong=tmpStrong;
            tmpChunk.morph=tmpMorph;


        } else {
            tmpChunk.morph="NONE";
            tmpChunk.strong="NONE";
            tmpChunk.fullWord=curWord;
        }



        verseChunkList.append(tmpChunk);

    }

}

QList<verseChunk> simpleOsisVerseParser::getVerselist(){
    return this->verseChunkList;
}
