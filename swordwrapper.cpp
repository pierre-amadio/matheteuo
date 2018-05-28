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


using namespace::sword;


swordWrapper::swordWrapper(QObject *parent) : QObject(parent)
{
    qDebug()<<"New wrapper should not be called without an engine";
}

swordWrapper::swordWrapper(QQmlApplicationEngine *myEngine, QObject *parent): QObject(parent)
{
    AppEngine=myEngine;
}

void swordWrapper::refreshMenus(){
    refreshModuleListModel(moduleListModel);
    QQmlContext *rootContext = AppEngine->rootContext();
    rootContext->setContextProperty("curModuleModel", QVariant::fromValue(moduleListModel));
}

void swordWrapper::moduleNameChangedSlot(const QString &msg) {
    QQmlContext *rootContext = AppEngine->rootContext();
    bookListModel.clear();
    foreach(QString c,getBookList(msg)){
        bookListModel.append(c);
    }
    rootContext->setContextProperty("curBookModel",QVariant::fromValue(bookListModel));
}

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

void  swordWrapper::wordInfoRequested(int wordIndex){
    QObject *rootObject = AppEngine->rootObjects().first();
    QString curModule=rootObject->property("curModuleName").toString();
    wordInfo * cw=wordInfoListModel[wordIndex];
    QString strongText=getStrongInfo(curModule,cw);
    QString morphText=getMorphInfo(curModule,cw);
    if(curModule=="OSHB"){
        rootObject->setProperty("oshbMorphCode",morphText);
        rootObject->setProperty("strongViewText",strongText);
    } else {
        rootObject->setProperty("strongViewText",strongText);
        rootObject->setProperty("morphViewText",morphText);
    }
}

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
            if(curMod->getName()!="MorphGNT" && curMod->getName()!="OSHB") {continue;}
            curMod->setLang(swordModule->getLanguage());
            curMod->setType(swordModule->getType());
            model.append(curMod);
        }
    }
}

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
            QString tpl="<a href=\"%1\" style=\" color:#000; text-decoration:none;\" >%2</a>";
            QString htmlBlob=QString (tpl).arg(indexSnt,s.fullWord);
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

QString swordWrapper::getStrongInfo(QString module, wordInfo * src){
    QString out="none";
    SWMgr library(new MarkupFilterMgr(FMT_HTML));
    SWModule * target;
    if(module=="MorphGNT") {
        //So the strongId should looks like "strong:G2532"
        //And the rootWord something like "lemma.Strong:βίβλος"
        out="<b>";
        out.append(src->rootWord.mid(13,src->rootWord.length()-13));
        out.append("</b><br>");
        if(src->StrongId.length()){
        QString q=src->StrongId.mid(8,src->StrongId.length()-8);
        target = library.getModule("StrongsGreek");
        if (!target) {qDebug()<<"Ooops StrongsGreek module not found"; }
        target->setKey(q.toStdString().c_str());
        QString tmpRaw=QString(target->renderText());
        tmpRaw.replace("\n","<br>");
        out.append(tmpRaw);
        }
    }

    if(module=="OSHB"){
        //So the strongId should looks like "strong:H07225"
        //and the rootWord like:
        QString q=src->StrongId.mid(8,src->StrongId.length()-8);
        target = library.getModule("StrongsHebrew");
        if (!target) {qDebug()<<"Ooops StrongsHebrew module not found"; }
        target->setKey(q.toStdString().c_str());
        QString tmpRaw=QString(target->renderText());
        tmpRaw.replace("\n","<br>");
        out=tmpRaw;
    }

    src->StrongDescription=out;
    return out;

}

QString swordWrapper::getMorphInfo(QString module, wordInfo * src){
    QString out="non";
    SWMgr library(new MarkupFilterMgr(FMT_PLAIN));
    SWModule * target;
    if(module=="MorphGNT") {
        QString q=src->morphCode.mid(9,src->morphCode.length()-9);
        target = library.getModule("Robinson");
        if (!target) {qDebug()<<"Ooops Robinson strong module not found"; }
        target->setKey(q.toStdString().c_str());
        out=target->renderText();
    }

    if(module=="OSHB") {
        out=QString(src->morphCode);
    }
    return out;
}
