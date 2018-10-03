import QtQuick 2.0

Rectangle {
    id:rootMyListtView
    width:10
    height: 50
    color: "#DDDDAA"
    border.color: Qt.lighter("grey")
    border.width: 2
    radius:5
    //when was the last time the user press the keyboard to search stuff ?
    property var keyPressLastTime:0
    //search time window in ms.
    property int searchTimeWindow:1000
    //what is the current search string ?
    property var searchString:""
    //opacity: .9
}
