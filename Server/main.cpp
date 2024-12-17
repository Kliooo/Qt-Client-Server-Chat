#include <QCoreApplication>

#include "server.hpp"


int main(int argc, char *argv[]) {
	QCoreApplication app(argc, argv);

	VBEK::Server server;

	qDebug() << "The server is running on port 1234";

	if (!server.startServer(1234)) {
		qDebug() << "Failed to start server!";
		return -1;
	}

	return app.exec();
}
