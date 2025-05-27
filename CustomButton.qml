import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.Material 2.15

import WorkWithEncryption 1.0

Button{
    Material.background: pressed ? Qt.darker(Material.primary, 1.2) : Material.primary
    Material.foreground: Material.color(Material.Grey, Material.Shade900)
    Material.elevation: pressed ? 1 : 3

    background: Rectangle {
        radius: 4
        color: parent.Material.background
        Behavior on color { ColorAnimation { duration: 100 } }
    }
}
