import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material 2.15

import QtQuick.Dialogs

import WorkWithEncryption 1.0

ColumnLayout{
    Label{
        text:"encryption file"
        font.pointSize: 15
    }
    ColumnLayout{
        Label{
            text: "select public key"
            font.pointSize: 10
        }
        RowLayout{
            CustomButton{
                Layout.preferredWidth: parent.width / 5
                Layout.fillHeight: true
                text: "select path"
                onClicked: fileDialogPublicKeyEncrypt.open()
            }
            TextField{
                id:publicKeyPathEncrypt
                Layout.fillWidth: true
                Layout.fillHeight: true
                placeholderText: "select public key"
            }
            FileDialog {
                id: fileDialogPublicKeyEncrypt
                title: "select files"
                nameFilters: ["key (*.pem)","All files (*)"]

                onAccepted: {
                    publicKeyPathEncrypt.text = selectedFile.toString().replace("file:///", "")
                }
            }
        }
    }
    ColumnLayout{
        Label{
            text: "select file"
            font.pointSize: 10
        }
        RowLayout{
            CustomButton{
                Layout.preferredWidth: parent.width / 5
                Layout.fillHeight: true
                text: "select file"
                onClicked: fileDialogFileEncrypt.open()
            }
            TextField{
                id:fileEncrypt
                Layout.fillWidth: true
                Layout.fillHeight: true
                placeholderText: "select file"
            }
            FileDialog {
                id: fileDialogFileEncrypt
                title: "select files"
                nameFilters: ["All files (*)"]

                onAccepted: {
                    fileEncrypt.text = selectedFile.toString().replace("file:///", "")
                }
            }
        }
    }

    RowLayout{
        Layout.fillWidth: true
        Layout.preferredHeight: parent.height / 10
        Label {
            text: "Delete original file after encryption"
            font.pointSize: 10
        }
        Item{
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        CheckBox {
            id: deleteOriginalFileCheck
            Layout.preferredWidth: parent.width / 10
            Layout.fillHeight: true
            checked: false
        }

    }

    CustomButton {
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: "encrypt file"
        onClicked: {
            var saveFilePath = fileEncrypt.text

            var fileNameWithExtension = saveFilePath.split('/').pop()
            var fileNameWithoutExtension = fileNameWithExtension.split('.').slice(0, -1).join('.')
            fileDialogFileEncryptSave.selectedFile =  Qt.resolvedUrl(fileNameWithoutExtension)

            fileDialogFileEncryptSave.open();
        }
    }

    FileDialog {
        id: fileDialogFileEncryptSave
        title: "write filename"
        nameFilters: ["All files (*)"]
        fileMode: FileDialog.SaveFile

        onAccepted: {
            var saveFilePath = selectedFile.toString().replace("file:///", "")

            if (publicKeyPathEncrypt.text === "" || fileEncrypt.text === "") {
                statusLabel.text = "Please select all required files"
                return
            }

            var encryptedFile = saveFilePath;

            if (workWithEncription.encryptFile(fileEncrypt.text, encryptedFile, publicKeyPathEncrypt.text, deleteOriginalFileCheck.checked)) {
                statusLabel.text = "File encrypted successfully"
            } else {
                statusLabel.text = "Failed to encrypt file"
            }
        }
    }
}
