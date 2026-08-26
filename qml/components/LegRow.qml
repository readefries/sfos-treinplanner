import QtQuick 2.0
import Sailfish.Silica 1.0

Row {
    property var leg

    spacing: Theme.paddingSmall

    Label {
        width: Theme.itemSizeSmall
        text: leg.category + " " + leg.number
        font.pixelSize: Theme.fontSizeExtraSmall
        color: leg.cancelled ? Theme.errorColor : Theme.secondaryColor
        truncationMode: TruncationMode.Fade
    }
    Label {
        width: parent.width - Theme.itemSizeSmall - platformBadge.width - 2 * parent.spacing
        text: leg.originName + " → " + leg.destinationName
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.secondaryColor
        truncationMode: TruncationMode.Fade
    }
    PlatformBadge {
        id: platformBadge
        track: leg.actualTrack
        plannedTrack: leg.plannedTrack
    }
}
