import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "MorphParse.js" as MorphParse
import "MorphCodes.js" as MorphCodes


Window {
    id: rootWindow
    objectName: "rootWindow"
    visible:true
    width: 800;
    height: 440
    //color: "#00EE00"
    //opacity: .9
    onHeightChanged: {

    }

    title:"μαθητεύω"

    property string curModuleName: "none yet"
    property string curModuleLang: "none yet"
    property string curBookName: "none yet"
    property int curChapter: 155
    property int maxChapter: 156
    property int curVerse: 157
    property int maxVerse: 158

    property variant chapterListModel: []
    property variant verseListModel: []

    property string mainTextModel:"pika coin coin"
    property string strongViewText:"le strong"
    property string morphViewText:"le moprh"

    property string oshbMorphCode

    function cleanBookList(){
        var s=bookListView.model.lenght
        var m=bookListView.model
        while(m[0]!==undefined){
            m.pull(0)
        }
    }

    function fillChapterList(nbr){
        chapterListModel=[]
        var tmpArray = new Array (0)
        for (var i = 1; i <= nbr; i++){
            tmpArray.push(i)
        }
        chapterListModel=tmpArray
        chapterView.currentIndex=0
        newChapterSelected(chapterListModel[chapterView.currentIndex])
    }

    function fillVerseList(nbr){
        verseListModel=[]
        var tmpArray = new Array (0)
        for (var i = 1; i <= nbr; i++){
            tmpArray.push(i)
        }
        verseListModel=tmpArray
        singleVerseView.currentIndex=0
        newVerseSelected(verseListModel[singleVerseView.currentIndex])
    }


    signal newModuleSelected(string msg)
    signal newWordInfoRequested(int wordIndex)

    onCurModuleNameChanged: {
        newModuleSelected(curModuleName)
    }

    onOshbMorphCodeChanged: {
        var morphC=oshbMorphCode.substring(5,oshbMorphCode.length)
        var parser = new MorphParse.MorphParse()
        var res =  parser.parse(morphC)
        res = res + "\n\n(" + morphC + ")\n\n"
        res = res + "http://openscriptures.github.io/morphhb/parsing/HebrewMorphologyCodes.html"
        if(oshbMorphCode.length) {
            morphViewText=res
        } else {
            morphViewText=""
        }
    }

    signal newBookSelected(string msg)
    onCurBookNameChanged: {
        newBookSelected(curBookName)
    }

    signal newChapterSelected(int chapter)
    onCurChapterChanged: {
        newChapterSelected(curChapter)
    }


    onMaxChapterChanged: {
        fillChapterList(maxChapter)
    }

    signal newVerseSelected(int verse)
    onCurVerseChanged: {
        newVerseSelected(curVerse)
    }

    onMaxVerseChanged: {
        fillVerseList(maxVerse)
    }


    Row {
        objectName: "selectVerseRow"
        id: selectVerseRow
        width:parent.width
        //opacity: .5
        height:50
        spacing: 0

        anchors {
            top:rootWindow.top
            bottom:verseView.top
            left:rootWindow.left
            right:rootWindow.right
        }


        MyListSelect {
            id: selectModuleView
            objectName: "selectModuleView"
            width: parent.width/4
            ListView{
                id:moduleListView
                objectName: "moduleListView"
                anchors.fill:parent
                model:curModuleModel
                snapMode:ListView.SnapToItem
                highlightRangeMode:ListView.StrictlyEnforceRange
                onCurrentItemChanged:{
                    rootWindow.curModuleName=curModuleModel[currentIndex].name
                    rootWindow.curModuleLang=curModuleModel[currentIndex].lang
                }
                delegate:
                    Text{
                    id:moduleNameText
                    font.pixelSize: 16
                    height:selectVerseRow.height/1
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    text: modelData.name
                }

            }
        }



        MyListSelect {
            id: selectBookView
            objectName: "selectBookView"
            width:parent.width/4
            ListView{
                id:bookListView
                objectName: "bookListView"
                anchors.fill:parent
                model:curBookModel
                snapMode:ListView.SnapToItem
                highlightRangeMode:ListView.StrictlyEnforceRange
                onCurrentItemChanged:{
                    rootWindow.curBookName=curBookModel[currentIndex]
                }
                delegate:
                    Text{
                    id:bookNameDelegate
                    objectName: "bookNameDelegate"
                    font.pixelSize: 16
                    height:selectVerseRow.height/1
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    text: modelData
                }

            }
        }

        MyListSelect {
            id:selectChapterView
            width:parent.width/4



            ListView{
                id:chapterView
                anchors.fill:parent
                model: chapterListModel
                snapMode:ListView.SnapToItem
                highlightRangeMode:ListView.StrictlyEnforceRange
                onCurrentItemChanged:{
                    rootWindow.curChapter=chapterListModel[currentIndex]
                }
                delegate:
                    Text{
                    id:chapterId
                    font.pixelSize: 16
                    height:selectVerseRow.height/1
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    text: modelData
                }

            }

        }
        MyListSelect {
            id:selectVerseView
            width:parent.width/4


            ListView{
                id:singleVerseView
                anchors.fill:parent
                model: verseListModel
                snapMode:ListView.SnapToItem
                highlightRangeMode:ListView.StrictlyEnforceRange
                onCurrentItemChanged:{
                    rootWindow.curVerse=verseListModel[currentIndex]
                }
                delegate:
                    Text{
                    id:verseId
                    font.pixelSize: 16
                    height:selectVerseRow.height/1
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    text: modelData
                }

            }



        }


    }



    Rectangle {
        id: verseView
        objectName: "verseView"
        width:rootWindow.width
        height:15*(rootWindow.height-selectVerseRow.height)/30

        anchors {
            top:selectVerseRow.bottom
            //bottom:grammarView.top
            left:rootWindow.left
            right:rootWindow.right
        }




        focus: true
        TextArea{
            id: verseWindow
            textFormat: Text.RichText
            //anchors.fill:parent
            anchors {
                top:verseView.top
                bottom:verseView.bottom
                left:verseView.left
                right:verseView.right
            }

            style: TextAreaStyle {
                selectedTextColor: "blue"
                selectionColor: "yellow"
            }


            readOnly: true
            font {
                //family: "Ezra SIL"
                family: "Linux Libertine O"
                pixelSize: 40
            }
            wrapMode: Text.WordWrap
            text:mainTextModel

            onLinkActivated:{
                newWordInfoRequested(parseInt(link))
            }


        }

    }

    Rectangle {
        id:grammarView
        width:parent.width
        height:15*(rootWindow.height-selectVerseRow.height)/30

        anchors {
            top:verseView.bottom
            bottom:rootWindow.bottom
            left:rootWindow.left
            right:rootWindow.right
        }


        TextArea {
            id: strongView
            width:parent.width/2
            height:parent.height
            textFormat: Text.RichText
            horizontalAlignment: Text.AlignLeft
            readOnly: true

            anchors{
                top:parent.top
                left:parent.left
                bottom: parent.bottom
            }

            font {
                //family: "Ezra SIL"
                family: "Linux Libertine O"
                pixelSize: 20
            }

            text:strongViewText
        }


        TextArea {
            id: morphView
            height:parent.height
            //textFormat: Text.RichText
            readOnly: true


            anchors{
                top:parent.top
                left:strongView.right
                right:parent.right
                bottom: parent.bottom
            }

            font {
                //family: "Ezra SIL"
                family: "Linux Libertine O"
                pixelSize: 20
            }

            text:morphViewText
        }

    }

}
