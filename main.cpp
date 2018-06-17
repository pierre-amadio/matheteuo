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
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include "swordwrapper.h"
#include <QQmlContext>
#include <QString>

/*
c++ and qml:
https://doc.qt.io/qt-5/qtqml-tutorials-extending-qml-example.html

implementing models in c++
http://doc.qt.io/qt-5/qtquick-modelviewsdata-cppmodels.html
http://doc.qt.io/qt-5/qtquick-models-objectlistmodel-example.html
http://doc.qt.io/qt-5/qtqml-cppintegration-exposecppattributes.html#exposing-properties

signal and stuff:
http://doc.qt.io/archives/qt-4.8/qtbinding.html
http://doc.qt.io/qt-5/qtqml-syntax-signals.html
http://www.qtcentre.org/threads/36782-SOLVED-qml-signal-with-c-slot

plugins:
TODO read https://qmlbook.github.io/ch16/index.html

multiple windows management
http://wiki.qt.io/QML_Application_Structuring_Approaches

parsing oshb morph with javascript:
https://github.com/openscriptures/morphhb-parsing

*/




int main(int argc, char *argv[])
{

    // The following requires at least Qt 5.6:
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QQmlContext *rootContext = engine.rootContext();
    rootContext->setContextProperty("curModuleModel",QVariant::fromValue(QStringList()));
    rootContext->setContextProperty("curBookModel",QVariant::fromValue(QStringList()));

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    QObject *rootObject = engine.rootObjects().first();

    swordWrapper * mySwordWrapper=new swordWrapper(&engine);

    //The user selected a new module (OSHB/MorphGNT)
    QObject::connect(rootObject, SIGNAL(newModuleSelected(QString)),
                     mySwordWrapper, SLOT(moduleNameChangedSlot(QString)));

    //The user selected a new Book (genesis/exodus/...)
    QObject::connect(rootObject,SIGNAL(newBookSelected(QString)),
                     mySwordWrapper, SLOT(bookNameChangedSlot(const QString))
                     );

    //The user selected a new Chapter.
    QObject::connect(rootObject,SIGNAL(newChapterSelected(int)),
                     mySwordWrapper,SLOT(chapterChangedSlot(int))
                     );

    //The user selected a new Verse.
    QObject::connect(rootObject,SIGNAL(newVerseSelected(int)),
                     mySwordWrapper,SLOT(verseChangedSlot(int))
                     );

    //The user requested more information about a word in the current verse.
    QObject::connect(rootObject,SIGNAL(newWordInfoRequested(int)),
                     mySwordWrapper,SLOT(wordInfoRequested(int))
                     );

    //The user requested more information about another strong entry in the Strong view.
    QObject::connect(rootObject,SIGNAL(newStrongInfoRequested(QString)),
                     mySwordWrapper,SLOT(strongInfoRequested(QString))
                     );


    //Now that signal are connected to slots, let s fill the menus of the GUI.
    mySwordWrapper->refreshMenus();


    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
