#include <QApplication>

#include "client.hpp"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	VBEK::Client client;
	client.show();

	return app.exec();
}
