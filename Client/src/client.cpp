#include <QHostAddress>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "client.hpp"


namespace VBEK {
	Client::Client(QWidget *parent) : QWidget(parent), currentRecipient("To everyone") {
		loginScreen = createLoginScreen();
		chatScreen = createChatScreen();
		chatScreen->hide();

		loginScreen->setMinimumSize(300, 200);
		chatScreen->setMinimumSize (450, 300);
		setWindowTitle("Registration");

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->addWidget(loginScreen);
		mainLayout->addWidget(chatScreen);

		socket = new QTcpSocket(this);

		connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
		connect(socket, &QTcpSocket::connected, this, &Client::switchToChatScreen);
	}


	QWidget* Client::createLoginScreen() {
		QWidget* screen = new QWidget(this);


		QLabel* usernameLabel = new QLabel("Username:");
		usernameInput = new QLineEdit();
		usernameInput->setMaxLength(15);

		QLabel* serverIpLabel = new QLabel("Server IP:");
		serverIpInput = new QLineEdit();
		serverIpInput->setText("127.0.0.1");

		connectButton = new QPushButton("Connect");
		connect(connectButton, &QPushButton::clicked, this, &Client::onConnectToServer);

		QVBoxLayout* layout = new QVBoxLayout(screen);
		layout->addWidget(usernameLabel);
		layout->addWidget(usernameInput);
		layout->addWidget(serverIpLabel);
		layout->addWidget(serverIpInput);
		layout->addWidget(connectButton);

		connect(usernameInput, &QLineEdit::returnPressed, connectButton, &QPushButton::click);
		connect(serverIpInput, &QLineEdit::returnPressed, connectButton, &QPushButton::click);

		return screen;
	}

	QWidget* Client::createChatScreen() {
		QWidget* screen = new QWidget(this);

		QLabel* nameLabel = new QLabel("Name:");
		nameInput = new QLineEdit();
		nameInput->setMaxLength(15);
		QPushButton* changeNameButton = new QPushButton("Change");
		connect(changeNameButton, &QPushButton::clicked, this, &Client::onChangeName);
		connect(nameInput, &QLineEdit::returnPressed, changeNameButton, &QPushButton::click);

		QHBoxLayout* nameLayout = new QHBoxLayout();
		nameLayout->addWidget(nameLabel);
		nameLayout->addWidget(nameInput);
		nameLayout->addWidget(changeNameButton);

		userList = new QListWidget();
		userList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
		userList->setMaximumWidth(100);

		chatBox = new QTextEdit();
		chatBox->setReadOnly(true);
		messageInput = new QLineEdit();
		sendButton = new QPushButton("Send");

		connect(sendButton, &QPushButton::clicked, this, &Client::onSendMessage);
		connect(userList, &QListWidget::itemClicked, this, &Client::onUserSelected);

		QHBoxLayout* mainLayout = new QHBoxLayout(screen);
		QVBoxLayout* chatLayout = new QVBoxLayout();

		chatLayout->addLayout(nameLayout);
		chatLayout->addWidget(chatBox);
		chatLayout->addWidget(messageInput);
		chatLayout->addWidget(sendButton);

		mainLayout->addWidget(userList);
		mainLayout->addLayout(chatLayout, 3);

		connect(messageInput, &QLineEdit::returnPressed, sendButton, &QPushButton::click);

		return screen;
	}


	void Client::switchToChatScreen() {
		loginScreen->hide();
		chatScreen->show();

		socket->write(QString("CONNECT:%1\n").arg(username).toUtf8());
	}

	void Client::addMessageToChat(const QString& sender, const QString& message) {
		chatBox->append(QString("%1: %2").arg(sender, message));
	}


	void Client::onConnectToServer() {
		QString ip = serverIpInput->text();
		username = usernameInput->text();

		if (ip.isEmpty() || username.isEmpty()) {
			QMessageBox::warning(this, "Error", "Enter the server IP and user name");
			return;
		}

		socket->connectToHost(QHostAddress(ip), 1234);

		if (!socket->waitForConnected(3000)) {
			QMessageBox::critical(this, "Error", "Failed to connect to server: " + socket->errorString());
			return;
		}
	}


	void Client::onChangeName() {
		QString newName = nameInput->text();
		if (newName.isEmpty()) {
			QMessageBox::warning(this, "Error", "Name cannot be empty.");
			return;
		}
		if (newName == username) {
			QMessageBox::warning(this, "Username Error", "Are you already using this name.");
			return;
		}
		if (newName != username) {
			username = newName;
			socket->write(QString("UPDATE_NAME:%1\n").arg(username).toUtf8());
			return;
		}
	}

	void Client::onConnectionTimeout() {
		QMessageBox::critical(this, "Connection Error", "Failed to connect to the server. Please check the IP address.");
	}

	void Client::onReadyRead() {
		while (socket->canReadLine()) {
			setWindowTitle("Chat");
			QString data = socket->readLine().trimmed();

			if (data.startsWith("USERS:")) {
				userList->clear();
				QStringList users = data.mid(6).split(',');
				userList->addItem("To everyone");
				userList->addItems(users);
			}
			else if (data.startsWith("MSG:")) {
				QStringList parts = data.mid(4).split(':');
				if (parts.size() >= 3) {
					QString sender = parts[0];
					QString recipient = parts[1];
					QString message = parts.mid(2).join(':');

					QString displayMessage;
					if (recipient == "ALL") {
						displayMessage = QString("%1 -> everyone: %2").arg(sender, message);
					} else {
						displayMessage = QString("%1 -> %2: %3").arg(sender, recipient, message);
					}

					chatBox->append(displayMessage);
				}
			}
			else if (data.startsWith("SERVER:Your username")) {
				QString newUsername = data.mid(61);
				username = newUsername;
				usernameInput->setText(newUsername);
				QMessageBox::information(this, "Username Taken", QString("Your username is already taken. Your new username is: %1").arg(newUsername));
			}
			else if (data.startsWith("SERVER:This username is already taken")) {
				QMessageBox::warning(this, "Username Error", "This username is already taken. Please choose another.");
				nameInput->clear();
			}
		}
	}

	void Client::onSendMessage() {
		QString message = messageInput->text();
		if (message.isEmpty()) return;

		QString formattedMessage;

		if (currentRecipient == "To everyone") {
			formattedMessage = QString("MSG:ALL:%1").arg(message);
		} else {
			formattedMessage = QString("MSG:%1:%2").arg(currentRecipient).arg(message);
		}

		socket->write(formattedMessage.toUtf8() + "\n");
		messageInput->clear();
	}

	void Client::onUserSelected(QListWidgetItem* item) {
		currentRecipient = item->text();
	}
}
