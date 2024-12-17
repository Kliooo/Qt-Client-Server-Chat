#include <QTextStream>
#include <QHostAddress>

#include "server.hpp"

namespace VBEK {
	Server::Server(QObject* parent) : QTcpServer(parent) {}

	bool Server::startServer(quint16 port) {
		return listen(QHostAddress::Any, port);
	}

	void Server::incomingConnection(qintptr socketDescriptor) {
		QTcpSocket* clientSocket = new QTcpSocket(this);
		clientSocket->setSocketDescriptor(socketDescriptor);

		connect(clientSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
		connect(clientSocket, &QTcpSocket::disconnected, this, &Server::onClientDisconnected);

		clientsSockets.append(clientSocket);
		clientsNames.append("");
	}

	QString Server::generateRandomUsername() {
		const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
		QString username;
		for (int i = 0; i < 10; ++i) {
			username.append(chars.at(rand() % chars.length()));
		}
		qDebug() << "Client connected: " << username;
		return username;
	}

	void Server::broadcastMessage(const QString& sender, const QString& message) {
		QString formattedMessage = QString("MSG:%1:ALL:%2\n").arg(sender, message);
		for (QTcpSocket* client : clientsSockets) {
			sendMessageToClient(client, formattedMessage);
		}
	}

	void Server::broadcastUserList() {
		QStringList userList = clientsNames;
		QString userListMessage = QString("USERS:%1\n").arg(userList.join(','));
		for (QTcpSocket* client : clientsSockets) {
			sendMessageToClient(client, userListMessage);
		}
	}

	void Server::privateMessage(QTcpSocket* from, const QString& recipient, const QString& message) {
		QString formattedMessage = QString("MSG:%1:%2:%3\n").arg(clientsNames[clientsSockets.indexOf(from)], recipient, message);

		bool recipientFound = false;

		for (QTcpSocket* client : clientsSockets) {
			int index = clientsSockets.indexOf(client);
			if (clientsNames[index] == recipient) {
				if (client != from) {
					sendMessageToClient(client, formattedMessage);
				}
				recipientFound = true;
			}
		}

		if (recipientFound) {
			sendMessageToClient(from, formattedMessage);
		} else {
			sendMessageToClient(from, "MSG:SERVER:The user was not found\n");
		}
	}

	void Server::sendMessageToClient(QTcpSocket* client, const QString& message) {
		if (client && client->isOpen()) {
			client->write(message.toUtf8() + "\n");
		}
	}

	void Server::onReadyRead() {
		QTcpSocket* senderSocket = qobject_cast<QTcpSocket*>(sender());
		if (!senderSocket) return;

		while (senderSocket->canReadLine()) {
			QString data = QString::fromUtf8(senderSocket->readLine()).trimmed();

			if (data.startsWith("CONNECT:")) {
				QString username = data.mid(8);

				if (clientsNames.contains(username)) {
					QString newUsername = generateRandomUsername();
					int index = clientsSockets.indexOf(senderSocket);
					clientsNames[index] = newUsername;
					sendMessageToClient(senderSocket, QString("SERVER:Your username is already taken. Your new username is: %1").arg(newUsername));
				} else {
					int index = clientsSockets.indexOf(senderSocket);
					clientsNames[index] = username;
					qDebug() << "Client connected: " << username;
				}

				broadcastUserList();
			}
			else if (data.startsWith("UPDATE_NAME:")) {
				QString newUsername = data.mid(12);

				if (clientsNames.contains(newUsername)) {
					sendMessageToClient(senderSocket, "SERVER:This username is already taken. Please choose another.");
					return;
				}

				int index = clientsSockets.indexOf(senderSocket);
				QString oldUsername = clientsNames[index];
				clientsNames[index] = newUsername;
				qDebug() << "User changed name: " << oldUsername << " -> " << newUsername;

				broadcastMessage("SERVER", QString("User changed name: %1 -> %2").arg(oldUsername, newUsername));
				broadcastUserList();
			}
			else if (data.startsWith("MSG:ALL:")) {
				QString message = data.mid(8);
				QString senderName = clientsNames[clientsSockets.indexOf(senderSocket)];
				broadcastMessage(senderName, message);
			}
			else if (data.startsWith("MSG:")) {
				QStringList parts = data.mid(4).split(':');
				if (parts.size() >= 2) {
					QString recipient = parts[0];
					QString message = parts[1];
					QString senderName = clientsNames[clientsSockets.indexOf(senderSocket)];
					privateMessage(senderSocket, recipient, message);
				}
			}
		}
	}

	void Server::onClientDisconnected() {
		QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
		if (!clientSocket) return;

		int index = clientsSockets.indexOf(clientSocket);
		QString username = clientsNames[index];
		clientsNames.removeAt(index);
		clientsSockets.removeAt(index);
		clientSocket->deleteLater();

		qDebug() << "The client has disconnected: " << username;
		broadcastUserList();
	}
}
