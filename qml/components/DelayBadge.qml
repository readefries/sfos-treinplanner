import QtQuick 2.0
import Sailfish.Silica 1.0

Rectangle {
    id: badge

    property int minutes: 0

    visible: minutes !== 0
    width: visible ? label.implicitWidth + Theme.paddingSmall : 0
    height: visible ? label.implicitHeight + Theme.paddingSmall / 2 : 0
    radius: height / 2
    color: minutes > 0 ? Theme.errorColor : Theme.highlightBackgroundColor

    Label {
        id: label
        anchors.centerIn: parent
        text: (minutes > 0 ? "+" : "") + minutes + qsTr("m", "delay in minutes, abbreviated")
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.primaryColor
    }
}
