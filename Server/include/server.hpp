#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QList>

namespace VBEK {

	class Server : public QTcpServer
	{
		Q_OBJECT

		private:
			QList<QTcpSocket*> clientsSockets;
			QList<QString> clientsNames;

		public:
			explicit Server(QObject* parent = nullptr);
			bool startServer(quint16 port);

		protected:
			void incomingConnection(qintptr socketDescriptor) override;


		private slots:
			void onReadyRead();
			void onClientDisconnected();

		private:
			QString generateRandomUsername();
			void broadcastMessage(const QString& sender, const QString& message);
			void broadcastUserList();
			void privateMessage(QTcpSocket* from, const QString& recipient, const QString& message);
			void sendMessageToClient(QTcpSocket* client, const QString& message);
	};
}
