import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Window {
    id: rootWindow
    objectName: "rootWindow"
    visible:true
    width: 800;
    height: 440
    //color: "#00EE00"
    //opacity: .9
    onHeightChanged: {
        //console.log("curModuleName",curModuleName)
        //console.log("currBookName",curBookName)
        //console.log("curChapter",curChapter)
        //console.log("curVerse", curVerse)
        //cleanBookList()
        var start=verseWindow.selectionStart
        var end=verseWindow.selectionEnd
        var curTxt=verseWindow.getText(start,end)
        //console.log("begin at",start)
        //console.log(verseWindow.selectedText)
        //console.log("end at",end)
        //console.log("\n",curTxt)
        verseWindow.font.underline=false
    }

    //onHeightChanged: console.log("max chapter",maxChapter)
    title:qsTr("Sword Reader")

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


    function cleanBookList(){
        console.log("Cleaning book list")
        //curBookModel=new Array (0)
        //curBookModel="none yet"
        //bookListView.model=new Array(0)
        var s=bookListView.model.lenght
        var m=bookListView.model
        //console.log(m[70])
        //for(var i=0;i<70;i++) {
        //    if (m[i]!==undefined){
        //        console.log(m[i])
        //    }
        //}
        while(m[0]!==undefined){
            console.log(m[0])
            m.pull(0)
        }

        //console.log(bookListView.model.count)
        //newModuleSelected(curModuleName)

    }

    function fillChapterList(nbr){
        //console.log("let's fill stuff:",nbr)
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
        //console.log("let's fill VerseList:",nbr)
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
        console.log("New module selected",curModuleName)
        //console.log(curBookModel)
        //curBookModel=new Array (0)
        //bookListView.model=curBookModel

        //curBookModel=tmpArray
        //bookListView.model=tmpArray
        //console.log("tmp array",tmpArray)
        //console.log("curbookModel is now ",curBookModel)

        //chapterListModel=[]

        newModuleSelected(curModuleName)
    }

    signal newBookSelected(string msg)
    onCurBookNameChanged: {
        console.log("New book selected",curBookName)
        newBookSelected(curBookName)
    }

    signal newChapterSelected(int chapter)
    onCurChapterChanged: {
        //console.log("New chapter selected",curChapter)
        newChapterSelected(curChapter)
    }


    onMaxChapterChanged: {
        console.log("mach chapter changed",maxChapter)
        fillChapterList(maxChapter)
    }

    signal newVerseSelected(int verse)
    onCurVerseChanged: {
        //console.log("New verse selected",curVerse)
        newVerseSelected(curVerse)
    }

    onMaxVerseChanged: {
        console.log("max verse changed",maxVerse)
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
        //height:275
        //opacity: .4
        //anchors.bottom: parent.bottom
        //anchors.top:selectVerseRow.bottom


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
                //backgroundColor: "black"
                //textColor: "white"
                selectedTextColor: "blue"
                selectionColor: "yellow"
            }


            readOnly: true
            font {
                //family: "Ezra SIL"
                family: "Linux Libertine O"
                pixelSize: 40
                //underline: false
            }
            wrapMode: Text.WordWrap
            text:mainTextModel

            onLinkActivated:{
                //console.log("cliketi:"+link)
                 newWordInfoRequested(parseInt(link))
            }

            onLinkHovered: {
                //console.log("what to do with:"+link)
            }



        }

    }

    Rectangle {
        id:grammarView
        width:parent.width
        //opacity: .5
        //height:2*(rootWindow.height-selectVerseRow.height)/10
        height:15*(rootWindow.height-selectVerseRow.height)/30
        //color:"yellow"

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
            readOnly: true

            anchors{
                top:parent.top
                left:parent.left
                //right:morphView.left
                bottom: parent.bottom
            }

            font {
                //family: "Ezra SIL"
                family: "Linux Libertine O"
                pixelSize: 20
                //underline: false
            }

            text:strongViewText
        }


        TextArea {
            id: morphView
            //width:parent.width/2
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
                //underline: false
            }


            text:morphViewText
        }


    }

}


