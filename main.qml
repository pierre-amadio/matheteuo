import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

/*
  Javascript magic to parse OSHB morphological code.
  This comes from https://github.com/openscriptures/morphhb-parsing
*/
import "MorphParse.js" as MorphParse
import "MorphCodes.js" as MorphCodes


Window {
    id: rootWindow
    objectName: "rootWindow"
    visible:true
    width: 800;
    height: 440
    color: globalBgColor
    //opacity: .9
    onHeightChanged: {
        console.log(curModuleName+" "+curBookName+" "+curChapter+" "+curVerse)
        var test=""
        //selectBookView.activeFocus ? test="book  have active focus!" : test="book not have active focus"
        //console.log(test)
        //selectChapterView.activeFocus ? test="chapter  have active focus!" : test="chapter not have active focus"
        //console.log(test)

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

    property string mainTextModel:"μαθητεύω did not find any Sword modules. Check your SWORD_PATH environment variable."
    property string strongViewText:""
    property string morphViewText:""

    /*
    An array storing the past strong id words shown
    in the strongViewText to be used in order to go back
    in a previously article.
    */
    property var strongHistory : []
    property string curStrongId : ""


    /*
     Some fonts looks better than others.

     I like the following:
                hebrew:  "Ezra SIL"
                greek: "Linux Libertine O"
    */
    property string favGreekFont: "Linux Libertine O"
    property string favHebrewFont: "Ezra SIL"
    property string curVerseFont
    property int curVersePixelSize: 55
    property int smallPixelSizeFactor: 4


    /*

      DEPRECATED: there seems to be an OSHM module now.

      There is no sword module yet to get OSHB morphological code informations.
      However, there is a simple javascript class that generate a human readable
      description based on a OSHB morphological code.
      So the c++ back engine store the actual OSHB morph code in this property
      wich is then used by javascript magic to fill the morphView when dealing
      with OSHB module.
    */
    property string oshbMorphCode

    property string globalBgColor: "white"
    property string globalFontColor: "black"

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
        curChapter=1
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
        curVerse=1
        newVerseSelected(verseListModel[singleVerseView.currentIndex])
    }


    signal newModuleSelected(string msg)
    signal newWordInfoRequested(int wordIndex)
    signal newStrongInfoRequested(string wordIndex)

    onCurModuleNameChanged: {
        newModuleSelected(curModuleName)
    }

    onCurModuleLangChanged: {
        if(curModuleLang=="grc") {
            curVerseFont=favGreekFont
        }
        if(curModuleLang=="he") {
            curVerseFont=favHebrewFont
        }
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
        //console.log("curChapter changed:"+curChapter)
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

    onMainTextModelChanged: {
        strongHistory=[]
        strongGoBackRectangle.visible=false
    }

    //The horizontal verse selection widgets.
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
                    color: globalFontColor
                    height:selectVerseRow.height/1
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    text: modelData.name
                }

            }
        }


        FocusScope {
            //x: select.x
            //y: selectBookView.y
            id: selectBookScope
            width: selectVerseRow.width/4
            height: selectVerseRow.height

            MyListSelect {
                id: selectBookView
                objectName: "selectBookView"
                width:parent.width
                ListView{
                    id:bookListView
                    objectName: "bookListView"
                    anchors.fill:parent
                    model:curBookModel


                    MouseArea { anchors.fill: parent;
                        onClicked: {
                            selectBookScope.focus = true ;
                            //console.log("book scope");
                        }
                    }


                    snapMode:ListView.SnapToItem
                    highlightRangeMode:ListView.StrictlyEnforceRange
                    onCurrentItemChanged:{
                        rootWindow.curBookName=curBookModel[currentIndex]
                    }


                    focus: true
                    Keys.onPressed: {
                        //console.log("book key press")

                        var now=new Date().getTime()
                        var timeDiff=now-selectBookView.keyPressLastTime
                        selectBookView.keyPressLastTime=new Date().getTime()
                        if(timeDiff>selectBookView.searchTimeWindow) {
                            selectBookView.searchString=event.text
                        } else {
                            selectBookView.searchString+=event.text
                        }
                        for(var i = 0; i < curBookModel.length; ++i) {
                            var curBookLower=curBookModel[i].toLowerCase();
                            var searchLower=selectBookView.searchString.toLowerCase()
                            if(curBookLower.indexOf(searchLower)===0) {
                                bookListView.currentIndex=i
                            }
                        }

                    }


                    delegate:
                        Text{
                        id:bookNameDelegate
                        objectName: "bookNameDelegate"
                        font.pixelSize: 16
                        color: globalFontColor
                        height:selectVerseRow.height/1
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        width: parent.width
                        text: modelData
                    }
                }
            }
        }

        FocusScope {
            width: selectVerseRow.width/4
            height: selectVerseRow.height
            id: selectChapterScope


            MyListSelect {
                id:selectChapterView
                width:parent.width

                ListView{
                    id:chapterView
                    anchors.fill:parent
                    model: chapterListModel
                    snapMode:ListView.SnapToItem
                    highlightRangeMode:ListView.StrictlyEnforceRange
                    onCurrentItemChanged:{
                        //console.log("current chapter index changed"+currentIndex)
                        if(currentIndex || currentIndex==0){
                            if(chapterListModel[currentIndex]!==undefined) {
                                rootWindow.curChapter=chapterListModel[currentIndex];
                            }
                        }
                    }



                    MouseArea { anchors.fill: parent;
                        onClicked: {
                            selectChapterScope.focus = true ;
                            //console.log("chapter scope");
                        }
                    }

                    focus: true
                    Keys.onPressed: {
                        //console.log("chapter key press")
                        var now=new Date().getTime()
                        var timeDiff=now-selectChapterView.keyPressLastTime
                        selectChapterView.keyPressLastTime=new Date().getTime()
                        if(timeDiff>selectChapterView.searchTimeWindow) {
                            selectChapterView.searchString=event.text
                        } else {
                            selectChapterView.searchString+=event.text
                        }
                        //console.log("searching chapter"+selectChapterView.searchString)
                        for(var i = 0; i < maxChapter; ++i) {
                            var tmpChapter=(1+i)
                            if( parseInt(selectChapterView.searchString,10)===tmpChapter ) {
                                chapterView.currentIndex=i
                            }
                        }
                    }

                    delegate:
                        Text{
                        id:chapterId
                        font.pixelSize: 16
                        color: globalFontColor
                        height:selectVerseRow.height/1
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        width: parent.width
                        text: modelData
                    }
                }
            }
        }


        FocusScope{
            width:selectVerseRow.width/4
            height:selectVerseRow.height
            id: selectVerseScope

            MyListSelect {
                id:selectVerseView
                width:parent.width

                ListView{
                    id:singleVerseView
                    anchors.fill:parent
                    model: verseListModel
                    snapMode:ListView.SnapToItem
                    highlightRangeMode:ListView.StrictlyEnforceRange
                    onCurrentItemChanged:{
                        if(currentIndex || currentIndex==0){
                            if (verseListModel[currentIndex] !== undefined) {
                                rootWindow.curVerse=verseListModel[currentIndex];
                            }
                        }
                    }

                    MouseArea { anchors.fill: parent;
                        onClicked: {
                            selectVerseScope.focus = true ;
                            //console.log("verse scope");
                        }
                    }

                    focus: true
                    Keys.onPressed: {
                        //console.log("verse key press")
                        var now=new Date().getTime()
                        var timeDiff=now-selectVerseView.keyPressLastTime
                        selectVerseView.keyPressLastTime=new Date().getTime()
                        if(timeDiff>selectVerseView.searchTimeWindow) {
                            selectVerseView.searchString=event.text
                        } else {
                            selectVerseView.searchString+=event.text
                        }
                        //console.log("searching verse"+selectVerseView.searchString)
                        for(var i = 0; i < maxVerse; ++i) {
                            var tmpVerse=1+i
                            if( parseInt(selectVerseView.searchString,10)===tmpVerse ) {
                                singleVerseView.currentIndex=i
                            }
                        }
                    }

                    delegate:
                        Text{
                        id:verseId
                        font.pixelSize: 16
                        color: globalFontColor
                        height:selectVerseRow.height/1
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        width: parent.width
                        text: modelData
                    }
                }
            }
        }
    }
    //Where the current verse is being displayed.
    Rectangle {
        id: verseView
        objectName: "verseView"
        width:rootWindow.width
        height:15*(rootWindow.height-selectVerseRow.height)/30
        color: globalBgColor

        anchors {
            top:selectVerseRow.bottom
            left:rootWindow.left
            right:rootWindow.right
        }

        focus: true

        Keys.onPressed: {
            if(event.key===Qt.Key_Plus) {curVersePixelSize=curVersePixelSize+smallPixelSizeFactor;}
            if(event.key===Qt.Key_Minus) {curVersePixelSize=curVersePixelSize-smallPixelSizeFactor;}
        }

        TextArea{
            id: verseWindow
            textFormat: Text.RichText

            anchors {
                top:verseView.top
                bottom:verseView.bottom
                left:verseView.left
                right:verseView.right
            }

            style: TextAreaStyle {
                selectedTextColor: "blue"
                selectionColor: "yellow"
                backgroundColor: globalBgColor
                textColor: globalFontColor
            }

            readOnly: true
            font {
                family: curVerseFont
                pixelSize: curVersePixelSize
            }
            wrapMode: Text.WordWrap
            text:mainTextModel

            onLinkActivated:{
                strongHistory=[]
                strongGoBackRectangle.visible=false
                newWordInfoRequested(parseInt(link))
            }
        }
    }

    //Below the verseView, strong and morphological infos.
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

            style: TextAreaStyle{
                backgroundColor: globalBgColor
                textColor: globalFontColor
            }


            onLinkActivated:{
                /*
                  Old style link:
                  <a href="H085">HEBREW for 085</a>

                  Now, with the Xiphos strongs module, it looks like
                  sword://StrongsRealGreek/04238
                  */

                var newlink
                if(link.search("sword://StrongsRealGreek/")==0) {
                    newlink=link.replace("sword://StrongsRealGreek/","G")
                } else if (link.search("sword://StrongsRealHebrew/")==0) {
                    newlink=link.replace("sword://StrongsRealHebrew/","H")
                } else
                {
                    console.log("Cannot parse link")
                    console.log(link)
                    newlink="H01"
                }

                strongHistory.push(newlink);
                curStrongId=newlink;
                newStrongInfoRequested(newlink);
                strongGoBackRectangle.visible=true;
            }


            anchors{
                top:parent.top
                left:parent.left
                bottom: parent.bottom
            }

            font {
                family: curVerseFont
                pixelSize: curVersePixelSize/smallPixelSizeFactor
            }

            text:strongViewText

            Rectangle {
                id: strongGoBackRectangle
                anchors.right: parent.right
                anchors.rightMargin: 5
                anchors.top: parent.top
                anchors.topMargin: 5
                width:parent.width/15
                height:parent.width/15
                opacity: .50
                color: "grey"
                visible:false
                Text{
                    anchors.fill:parent
                    text:"<b>←</b>"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 2*curVersePixelSize/smallPixelSizeFactor
                }

                border.color: Qt.lighter("grey")
                border.width: 2
                radius:5

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        var windex;
                        strongHistory.pop();
                        windex=strongHistory[strongHistory.length-1]
                        curStrongId=windex;
                        newStrongInfoRequested(windex);
                        if(strongHistory.length==1) {
                            strongGoBackRectangle.visible=false;
                        }
                    }
                }
            }
        }


        TextArea {
            id: morphView
            height:parent.height
            //textFormat: Text.RichText
            readOnly: true

            style: TextAreaStyle{
                backgroundColor: globalBgColor
                textColor: globalFontColor

            }


            anchors{
                top:parent.top
                left:strongView.right
                right:parent.right
                bottom: parent.bottom
            }

            font {
                family: curVerseFont
                pixelSize: curVersePixelSize/smallPixelSizeFactor
            }

            text:morphViewText
        }

    }

}
