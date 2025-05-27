import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material 2.15

import QtQuick.Dialogs

import WorkWithEncryption 1.0

ColumnLayout{
    Label{
        text:"create rsa keys"
        font.pointSize: 15
    }
    ColumnLayout{
        Label{
            text: "create public key"
            font.pointSize: 10
        }
        RowLayout{
            CustomButton{
                Layout.preferredWidth: parent.width / 5
                Layout.fillHeight: true
                text: "select path"
                onClicked: fileDialogPublicKeyGenerate.open()
            }
            TextField{
                id:publicKeyPathGenerate
                Layout.fillWidth: true
                Layout.fillHeight: true
                placeholderText: "public key"
            }
            FileDialog {
                id: fileDialogPublicKeyGenerate
                title: "select file"
                nameFilters: ["key (*.pem)", "All files (*)"]
                fileMode: FileDialog.SaveFile
                onAccepted: {
                    publicKeyPathGenerate.text = selectedFile.toString().replace("file:///", "")
                }
            }
        }
    }
    ColumnLayout{
        Label{
            text: "create private key"
            font.pointSize: 10
        }
        RowLayout{
            CustomButton{
                Layout.preferredWidth: parent.width / 5
                Layout.fillHeight: true
                text: "select path"
                onClicked: fileDialogPrivateKeyGenerate.open()
            }
            TextField{
                id:privateKeyPathGenerate
                Layout.fillWidth: true
                Layout.fillHeight: true
                placeholderText: "private key"
            }
            ComboBox {
                id: selectKeySizeComboBox
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width/7

                Material.accent: Material.Blue
                Material.foreground: Material.Blue

                model: [512, 1024, 2048, 4096]
            }
            FileDialog {
                id: fileDialogPrivateKeyGenerate
                title: "select file"
                nameFilters: ["key (*.pem)", "All files (*)"]
                fileMode: FileDialog.SaveFile
                onAccepted: {
                    privateKeyPathGenerate.text = selectedFile.toString().replace("file:///", "")
                }
            }
        }
    }
    CustomButton {
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: "create keys"
        onClicked: {
            if (publicKeyPathGenerate.text === "" || privateKeyPathGenerate.text === "") {
                statusLabel.text = "Please select both key paths"
                return
            }
            if (workWithEncription.createRsaKeys(publicKeyPathGenerate.text, privateKeyPathGenerate.text, selectKeySizeComboBox.currentText)) {
                //statusLabel.text = "RSA keys generated successfully"
            } else {
                //statusLabel.text = "Failed to generate RSA keys"
            }
        }
    }

    MessageDialog {
        id: overwriteDialog
        title: "Confirmation"
        text: "Are you sure you want to generate these keys?"
        buttons: MessageDialog.Ok | MessageDialog.Cancel

        onAccepted: workWithEncription.onDialogResult(true)
        onRejected: workWithEncription.onDialogResult(false)
    }

    Connections {
        target: workWithEncription
        onShowConfirmationDialog: {
            overwriteDialog.text = message
            overwriteDialog.open()
        }
        onKeysCreationFinished: {
            if(success) {
                statusLabel.text = "Keys created successfully"
            } else {
                statusLabel.text = "Operation cancelled"
            }
        }
    }

}



