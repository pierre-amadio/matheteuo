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
swordWrapper is the actual backend engine of matheteuo.
It receive signals from the qml gui, makes request to the sword library
and set back the correct qml property and models based on those sword data.
*/

#ifndef SWORDWRAPPER_H
#define SWORDWRAPPER_H

#include <QObject>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include "wordinfo.h"

class swordWrapper : public QObject
{
    Q_OBJECT
public:
    explicit swordWrapper(QObject *parent = nullptr);
    swordWrapper(QQmlApplicationEngine  * engine, QObject *parent = nullptr);

    void refreshModuleListModel(QList<QObject*> &model);
    void refreshWordInfoListModel(QString vsnt);
    void refreshMenus();
    QList<QObject*> getModuleListModel();
    QList<wordInfo*> getWordInfoListModel();
    QStringList getBookListModel();
    QStringList getBookList(const QString & moduleName);
    QString getVerse(QString module, QString book ,int chapter, int verse);

    int getChapterMax();
    int getVerseMax();


private:
    QList<QObject*> moduleListModel;
    QStringList bookListModel;
    QList<wordInfo*> wordInfoListModel;
    QQmlApplicationEngine * AppEngine;
    QString getStrongInfo(QString module, wordInfo * src);
    QString getMorphInfo(QString module, wordInfo * src);

signals:
    void maxChapterChanged(int nbrChapter);

public slots:
    void moduleNameChangedSlot(const QString &msg);
    void bookNameChangedSlot(const QString &msg);
    void chapterChangedSlot(int chapterNbr);
    void verseChangedSlot(int verseNbr);
    void wordInfoRequested(int wordIndex);
};

#endif // SWORDWRAPPER_H
