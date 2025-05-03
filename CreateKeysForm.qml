import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CryptoUtils
import QtQuick.Controls.Material 2.15
import Qt.labs.platform 1.1

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
                folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                nameFilters: ["key (*.pem)", "All files (*)"]
                fileMode: FileDialog.SaveFile
                onAccepted: {
                    publicKeyPathGenerate.text = String(fileDialogPublicKeyGenerate.file).replace("file:///", "")
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
            FileDialog {
                id: fileDialogPrivateKeyGenerate
                title: "select file"
                folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                nameFilters: ["key (*.pem)", "All files (*)"]
                fileMode: FileDialog.SaveFile
                onAccepted: {
                    privateKeyPathGenerate.text = String(fileDialogPrivateKeyGenerate.file).replace("file:///", "")
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
            if (cryptoUtils.createRsaKeys(publicKeyPathGenerate.text, privateKeyPathGenerate.text)) {
                statusLabel.text = "RSA keys generated successfully"
            } else {
                statusLabel.text = "Failed to generate RSA keys"
            }
        }
    }


}



