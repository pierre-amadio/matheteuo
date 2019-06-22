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
#include "swordwrapper.h"
#include "wordinfo.h"
#include "moduleinfo.h"
#include "simpleosisverseparser.h"
#include <QDebug>
#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>
#include <markupfiltmgr.h>
#include <QQmlContext>
#include <QDateTime>
#include <QListView>
#include <QRegularExpression>

using namespace::sword;

//no arg constructor.Do not use...
swordWrapper::swordWrapper(QObject *parent) : QObject(parent)
{
    qDebug()<<"New wrapper should not be called without an engine";
}

//constructor with the qml engine as argument.
swordWrapper::swordWrapper(QQmlApplicationEngine *myEngine, QObject *parent): QObject(parent)
{
    AppEngine=myEngine;
}

//let s fill menus so we have a selection
//when the application start.
void swordWrapper::refreshMenus(){
    refreshModuleListModel(moduleListModel);
    QQmlContext *rootContext = AppEngine->rootContext();
    rootContext->setContextProperty("curModuleModel", QVariant::fromValue(moduleListModel));
}

//something changed the curModuleName property in qml.
void swordWrapper::moduleNameChangedSlot(const QString &msg) {
    QQmlContext *rootContext = AppEngine->rootContext();
    bookListModel.clear();
    foreach(QString c,getBookList(msg)){
        bookListModel.append(c);
    }
    rootContext->setContextProperty("curBookModel",QVariant::fromValue(bookListModel));
}

//something changed the curBookName property in qml.
void swordWrapper::bookNameChangedSlot(const QString &curBook) {
    QObject *rootObject = AppEngine->rootObjects().first();

    int oldNbrChapter=rootObject->property("maxChapter").toInt();
    int newNbrChapter=getChapterMax();

    if(oldNbrChapter==newNbrChapter) {
        //This can happen when the next selected book has the same
        //amount of chapter than the previous one.
        //by example Romans and I Corinthians both have 16 chapters.
        rootObject->setProperty("maxChapter", -1);
    }
    rootObject->setProperty("maxChapter", newNbrChapter);
}

//something changed the curChapter in qml.
void swordWrapper::chapterChangedSlot(int chapterNbr) {
    QObject *rootObject = AppEngine->rootObjects().first();

    int oldVerseMax=rootObject->property("maxVerse").toInt();
    int newVerseMax=getVerseMax();
    if(oldVerseMax==newVerseMax) {
        //this can happen when switching from gen:10 to gen:11 by example
        //because both chapter have 32 verses.
        rootObject->setProperty("maxVerse", -1);
    }
    rootObject->setProperty("maxVerse", newVerseMax);
}

//something changed the curVerse in qml.
void swordWrapper::verseChangedSlot(int verseNbr){
    QObject *rootObject = AppEngine->rootObjects().first();
    QString module=rootObject->property("curModuleName").toString();
    QString book=rootObject->property("curBookName").toString();
    int chapter=rootObject->property("curChapter").toInt();
    int verse=rootObject->property("curVerse").toInt();
    QString rawVerse=getVerse(module,  book , chapter,  verse);
    refreshWordInfoListModel(rawVerse);
    rootObject->setProperty("strongViewText","");
    rootObject->setProperty("morphViewText","");
}


//return a list of book name available i n moduleName module.
QStringList swordWrapper::getBookList(const QString &moduleName){
    QList<QString> output;

    SWMgr library(new MarkupFilterMgr(FMT_PLAIN));
    SWModule *target;
    target = library.getModule(moduleName.toStdString().c_str());

    VerseKey *vk = (target) ? (VerseKey *)target->getKey() : new VerseKey();

    for ((*vk) = TOP; !vk->popError(); vk->setBook(vk->getBook()+1)) {
        if (!target || target->hasEntry(vk)) {
            QString  plop=QString::fromStdString(vk->getBookName());
            output.append(plop);
        }
    }
    return output;

}

//prepare the model of existing modules usable in the GUI
//currently, only OSHB,MorphGNT and LXX are supported.
void swordWrapper::refreshModuleListModel(QList<QObject*> &model){
    qDeleteAll(model.begin(), model.end());
    model.clear();
    SWMgr library;
    ModMap::iterator modIterator;

    for (modIterator = library.Modules.begin(); modIterator != library.Modules.end(); modIterator++) {
        SWModule *swordModule = (*modIterator).second;
        const char * bibleTextSnt = "Biblical Texts";
        const char * modType=swordModule->getType();
        int strCmp=strncmp ( bibleTextSnt, modType, strlen(bibleTextSnt));
        if(strlen(bibleTextSnt)==strlen(modType) && strCmp==0){
            moduleInfo * curMod;
            curMod=new moduleInfo();
            curMod->setName(swordModule->getName());
            //Let s deal only with module with embedded grammar data.
            if(curMod->getName()!="MorphGNT" && curMod->getName()!="OSHB"  && curMod->getName()!="LXX") {continue;}
            curMod->setLang(swordModule->getLanguage());
            curMod->setType(swordModule->getType());
            model.append(curMod);
        }
    }
}

/*
 Prepare the 2 models used to display the verse in the verseView.

  - wordInfoListModel:  a QList of pointer to wordInfo instances maching
  the verse being displayed. This variable is kept on the backend side.
  The qml side only knows about the mainTextModel.

  - mainTextModel:  the actual model for the VerseView TextArea.
    this is in html. Each words with information is a link with
    information about the index of this word in the wordInfoListModel list.
    When the user click on a word, this url is used to get the right word
    in the wordInfoListModel.


*/
void swordWrapper::refreshWordInfoListModel(QString vsnt){
    qDeleteAll(wordInfoListModel.begin(),wordInfoListModel.end());
    wordInfoListModel.clear();
    QObject *rootObject = AppEngine->rootObjects().first();
    QString curModule=rootObject->property("curModuleName").toString();
    simpleOsisVerseParser simpleParser(vsnt,curModule);
    QList<verseChunk> list=simpleParser.getVerselist();
    QString htmlText;
    int cnt=0;
    foreach( verseChunk s, list ) {

        wordInfo  * cwi;
        cwi=new wordInfo();

        if (s.isXmlTag) {
            QString indexSnt=QString::number(cnt);
            QString globalFontColor=rootObject->property("globalFontColor").toString();
            QString tpl="<a href=\"%1\" style=\" color:%2; text-decoration:none;\" >%3</a>";
            QString htmlBlob=QString (tpl).arg(indexSnt,globalFontColor,s.fullWord);
            htmlText.append(htmlBlob);
            cwi->setDisplayWord(s.fullWord);
            cwi->rootWord=s.rootValue;
            cwi->morphCode=s.morph;
            cwi->StrongId=s.strong;

            cwi->morphDesciption="TODO";
            cwi->StrongDescription="TODO";

            cwi->hasInfo=true;
        } else {
            htmlText.append(s.fullWord);
            cwi->displayWord=s.fullWord;
            cwi->hasInfo=false;
        }

        wordInfoListModel.append(cwi);
        cnt++;
    }
    rootObject->setProperty("mainTextModel",htmlText);

}

QList<QObject*> swordWrapper::getModuleListModel(){
    return moduleListModel;
}

QList<wordInfo*> swordWrapper::getWordInfoListModel(){
    return wordInfoListModel;
}

QStringList swordWrapper::getBookListModel(){
    return bookListModel;
}

//return how many chapter is there for the current book in the current module.
int swordWrapper::getChapterMax(){
    QObject *rootObject = AppEngine->rootObjects().first();
    QString curModule=rootObject->property("curModuleName").toString();
    QString curBook=rootObject->property("curBookName").toString();
    SWMgr manager;
    SWModule *bible = manager.getModule(curModule.toStdString().c_str());
    if (!bible) {
        qDebug() <<"Sword module "<< curModule << " not installed. This should not have happened...";
    }
    VerseKey *vk = (VerseKey *)bible->createKey();
    vk->setBookName(curBook.toStdString().c_str());
    return vk->getChapterMax();
}

//return how many verse are in the current chapter of the current book in the current module.
int swordWrapper::getVerseMax(){
    QObject *rootObject = AppEngine->rootObjects().first();
    QString curModule=rootObject->property("curModuleName").toString();
    QString curBook=rootObject->property("curBookName").toString();
    int curChapter=rootObject->property("curChapter").toInt();
    SWMgr manager;
    SWModule *bible = manager.getModule(curModule.toStdString().c_str());
    if (!bible) {
        qDebug() <<"Sword module "<< curModule << " not installed. This should not have happened...";
    }
    VerseKey *vk = (VerseKey *)bible->createKey();
    vk->setBookName(curBook.toStdString().c_str());
    vk->setChapter(curChapter);
    return vk->getVerseMax();

}


/*
 Return the verse content for this module/book/chapter/verse according to
 the sword library with accents/cantillation/morph info/strong info in OSIS  format.
*/
QString swordWrapper::getVerse(QString module, QString book ,int chapter, int verse){
    QString out="not done";
    SWMgr library(new MarkupFilterMgr(FMT_OSIS));
    library.setGlobalOption("Morpheme Segmentation","On");
    library.setGlobalOption("Lemmas","On");
    library.setGlobalOption("Morphological Tags","On");
    library.setGlobalOption("Strong's Numbers","On");
    library.setGlobalOption("OSISStrongs","On");
    library.setGlobalOption("Hebrew Cantillation", "On");
    library.setGlobalOption("Hebrew Vowel Points", "On");
    library.setGlobalOption("Greek Accents", "On");
    library.setGlobalOption("Textual Variants", "On");
    SWModule *bible = library.getModule(module.toStdString().c_str());
    if (!bible) {
        qDebug() <<"In getVerse: Sword module "<< module << " not installed. This should not have happened...";
    }
    VerseKey *vk = (VerseKey *)bible->createKey();
    vk->setBookName(book.toStdString().c_str());
    vk->setChapter(chapter);
    vk->setVerse(verse);

    bible->setKey(vk);
    out=bible->renderText();
    return out;
}


/*
Return the Strong's dictionnary entry for the correct 'lang' (grc for greek or he for hebrew)
maching index 'number'
This is in html format.
*/

QString swordWrapper::getStrongInfo(QString lang, QString number){
    QString out="none";
    SWModule * target;
    SWMgr library(new MarkupFilterMgr(FMT_HTML));

    if(lang=="grc") {
        target = library.getModule("StrongsRealGreek");
    } else {
        target = library.getModule("StrongsRealHebrew");
    }

    if (!target) {
        qDebug()<<QString("Ooops strong dictionnary for %1 not found").arg(lang);
    }
    target->setKey(number.toStdString().c_str());
    //out=QString(target->renderText());
    //out=htmlizeStrongInfo(QString(target->renderText()));
    out=QString(target->renderText());
    return out;
}


/*
 Set (and return) the strong's lexicon entry for a given src wordInfo.
 This is in html format.
*/
QString swordWrapper::getStrongInfo(QString module, wordInfo * src){
    QString out="";
    SWMgr library(new MarkupFilterMgr(FMT_HTML));
    //SWModule * target;

    if(module=="MorphGNT") {
        //So the strongId should looks like "strong:G2532"
        //And the rootWord something like "lemma.Strong:βίβλος"
        //out="<b>";
        //out.append(src->rootWord.mid(13,src->rootWord.length()-13));
        //out.append("</b><br>");
        if(src->StrongId.length()){
            QString nbr=src->StrongId.mid(8,src->StrongId.length()-8);
            out.append(getStrongInfo("grc",nbr));
        }
    }

    if(module=="OSHB"){
        //So the strongId should looks like "strong:H07225"
        //and the rootWord like:
        //out="<b>"+src->displayWord+"</b><br>";
        QString nbr=src->StrongId.mid(8,src->StrongId.length()-8);
        out.append(getStrongInfo("he",nbr));
        qDebug()<<getStrongInfo("he",nbr);

    }

    if(module=="LXX") {
        //So the strongId shouldlooks like "strong:G2316"
        if(src->StrongId.length()){
            //out="<b>";
            //out.append(src->displayWord);
            //out.append("</b><br>");
            QString nbr=src->StrongId.mid(8,src->StrongId.length()-8);
            if(nbr.toInt()==0){
                //For some strange reason, some strong entry are wrong.
                //By example Gen 1:2
                //<w  savlm="strong:GA-NSM">ακατασκευαστος</w>
                out="";
            } else {
                out.append(getStrongInfo("grc",nbr));
            }
        }
    }

    src->StrongDescription=out;
    return out;

}

/*
 Set (and return) the morphological description of a given src wordInfo.
*/
QString swordWrapper::getMorphInfo(QString module, wordInfo * src){
    QString out="non";
    SWMgr library(new MarkupFilterMgr(FMT_PLAIN));
    SWModule * target;
    if(module=="MorphGNT") {
        QString q=src->morphCode.mid(9,src->morphCode.length()-9);
        target = library.getModule("Robinson");
        if (!target) {qDebug()<<"Ooops Robinson morphological module not found"; }
        target->setKey(q.toStdString().c_str());
        out=target->renderText();
    }

    if(module=="OSHB") {
        if(src->morphCode.left(5)=="oshm:") {
            QString q=QString(src->morphCode).mid(5,src->morphCode.length()-5);
            target = library.getModule("OSHM");
            if (!target) {qDebug()<<"Ooops OSHM morphological module not found"; }
            target->setKey(q.toStdString().c_str());
            out=target->renderText();
            //When there was no OSHM module the parsing was done in qml
            //and the c++  backend was setting some qml property with the raw code.
            //out=QString(src->morphCode);
        } else {
            out="";
        }
    }

    if(module=="LXX") {
        QString q=src->morphCode.mid(8,src->morphCode.length()-8);
        target = library.getModule("Robinson");
        if (!target) {qDebug()<<"Ooops Robinson morphological module not found"; }
        target->setKey(q.toStdString().c_str());
        out=target->renderText();
    }

    src->morphDesciption=out;
    return out;
}

//the user clicked on a word to request strong lexicon & morph informations.
void  swordWrapper::wordInfoRequested(int wordIndex){
    QObject *rootObject = AppEngine->rootObjects().first();
    QString curModule=rootObject->property("curModuleName").toString();

    wordInfo * cw=wordInfoListModel[wordIndex];

    QString strongCode=cw->StrongId.right(cw->StrongId.length()-7);
    QVariantList newStrongHistory;
    newStrongHistory.append(strongCode);
    rootObject->setProperty("strongHistory",newStrongHistory);
    rootObject->setProperty("curStrongId",strongCode);
    QString strongText=getStrongInfo(curModule,cw);
    QString morphText=getMorphInfo(curModule,cw);
    //Once upon a time,there was no OSHM morph module
    //the c++ backend would then reset the oshbMorphCode property
    //directly in qml and qml would parse it with javascript.
    //if(curModule=="OSHB"){
    //    rootObject->setProperty("oshbMorphCode",morphText);
    //    rootObject->setProperty("strongViewText",strongText);
    //} else {
    rootObject->setProperty("strongViewText",strongText);
    rootObject->setProperty("morphViewText",morphText);
    //}
}


//the user clicked on a strong number link.
void  swordWrapper::strongInfoRequested(QString wordIndex){
    QString nbr=wordIndex.right(wordIndex.length()-1);
    QString newDesc;

    if(wordIndex.left(1)=="H") {
        newDesc=getStrongInfo("he",nbr);
    } else {
        newDesc=getStrongInfo("grc",nbr);
    }
    QObject *rootObject = AppEngine->rootObjects().first();
    //rootObject->setProperty("strongViewText",htmlizeStrongInfo(newDesc));
    rootObject->setProperty("strongViewText",newDesc);

}

//Change the raw text strong dictionnary entry in html.
QString swordWrapper::htmlizeStrongInfo(QString raw){
    QString out=raw;


    /*
     * Now lets modify the following sort of line with http link:
     * see HEBREW for 085
     * see GREEK for 756
     * link should looks like <a href="H085">HEBREW for 085</a>
     *
     * Carefull with entry such as αδη in LXX psalm 6:5
     * where a strong id "GREEK for 1" is actually a substring
     * of another id in the same article "GREEK for 1492"
    */

    QRegularExpression hre("HEBREW for (\\d+)\\D*$");
    hre.setPatternOptions(QRegularExpression::MultilineOption);
    out.replace(hre,"<a href=\"H\\1\">HEBREW for \\1</a>");

    QRegularExpression gre("GREEK for (\\d+)\\D*$");
    gre.setPatternOptions(QRegularExpression::MultilineOption);
    out.replace(gre,"<a href=\"G\\1\">GREEK for \\1</a>");

    out.replace("\n","<br>");

    return out;
}
