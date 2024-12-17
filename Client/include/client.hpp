#pragma once

#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QTcpSocket>
#include <QWidget>

namespace VBEK {
	class Client : public QWidget {
		Q_OBJECT

		public:
			Client(QWidget *parent = nullptr);

		private:
			QWidget* createLoginScreen();
			QWidget* createChatScreen();

			QWidget* loginScreen;
			QLineEdit* usernameInput;
			QLineEdit* serverIpInput;
			QLineEdit* nameInput;
			QPushButton* connectButton;

			QWidget* chatScreen;
			QListWidget* userList;
			QTextEdit* chatBox;
			QLineEdit* messageInput;
			QPushButton* sendButton;

			QTcpSocket* socket;
			QString currentRecipient;
			QString username;

		private:
			void switchToChatScreen();
			void addMessageToChat(const QString& sender, const QString& message);

		private slots:
			void onConnectToServer();
			void onChangeName();
			void onConnectionTimeout();
			void onReadyRead();
			void onSendMessage();
			void onUserSelected(QListWidgetItem* item);
	};
}
