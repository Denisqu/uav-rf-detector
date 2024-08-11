#include "utils/logger.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QWebSocket>

namespace
{

LOG_CAT(log_cat, "main")

std::unique_ptr<QWebSocket> createTestJRPCQWebsocketClient()
{
	auto socket = std::make_unique<QWebSocket>();

	QObject::connect(socket.get(), &QWebSocket::textMessageReceived, socket.get(), [](const QString& msg) {
		log_info(log_cat, "received string from websocket: {}", msg.toStdString());
	});
	QObject::connect(socket.get(), &QWebSocket::connected, socket.get(), [&socket]() {
		log_info(log_cat,  "connected to: {}", socket->peerAddress().toString().toStdString());

		auto msg2 = QString { "msg after connected..." };
		socket->sendTextMessage(msg2);
	});
	QObject::connect(socket.get(), &QWebSocket::disconnected, socket.get(), [&socket]() {
		log_info(log_cat,  "disconnected from: {}", socket->peerAddress().toString().toStdString());
	});
	QObject::connect(socket.get(), &QWebSocket::errorOccurred, socket.get(), [&socket](const QAbstractSocket::SocketError& error) {
		log_info(log_cat, "socket error: {}", socket->errorString().toStdString());
	});

	socket->open(QUrl{ "ws://127.0.0.1:80" });
	QString helloMsg = QString { "hello, server!" };
	socket->sendTextMessage(helloMsg);
	return socket;
}

}

int main(int argc, const char **argv)
{
	log_info(log_cat, "Starting application...");
    QGuiApplication app(argc, const_cast<char**>(argv));

	auto pWebSocket = createTestJRPCQWebsocketClient();


	// TODO: убрать использование шрифта из билд директории
    //qint32 fontId = QFontDatabase::addApplicationFont("./Roboto-Regular.ttf");
    //QStringList fontList = QFontDatabase::applicationFontFamilies(fontId);
    //QString family = fontList.first();
    //QGuiApplication::setFont(QFont(family));

    QQmlApplicationEngine appEngine(QUrl("qrc:/qt/qml/app/assets/qml/main.qml"));
    return app.exec();
}
