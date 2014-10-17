#include <QLocalServer>
#include <QLocalSocket>

#include "qjsonrpcsocket.h"
#include "qjsonrpcabstractserver_p.h"
#include "qjsonrpclocalserver.h"

class QJsonRpcLocalServerPrivate : public QJsonRpcAbstractServerPrivate
{
    Q_DECLARE_PUBLIC(QJsonRpcLocalServer)
public:
    QJsonRpcLocalServerPrivate(QJsonRpcLocalServer *q)
        : QJsonRpcAbstractServerPrivate(q),
          server(0)
    {
    }

    virtual void _q_processIncomingConnection();
    virtual void _q_clientDisconnected();

    QLocalServer *server;
    QHash<QLocalSocket*, QJsonRpcSocket*> socketLookup;
};

QJsonRpcLocalServer::QJsonRpcLocalServer(QObject *parent)
    : QJsonRpcAbstractServer(*new QJsonRpcLocalServerPrivate(this), parent)
{
}

QJsonRpcLocalServer::~QJsonRpcLocalServer()
{
    Q_D(QJsonRpcLocalServer);
    foreach (QLocalSocket *socket, d->socketLookup.keys())
        socket->deleteLater();
    d->socketLookup.clear();
}

bool QJsonRpcLocalServer::listen(const QString &service)
{
    Q_D(QJsonRpcLocalServer);
    if (!d->server) {
        d->server = new QLocalServer(this);
        connect(d->server, SIGNAL(newConnection()), this, SLOT(_q_processIncomingConnection()));
    }

    return d->server->listen(service);
}

void QJsonRpcLocalServer::close()
{
    Q_D(QJsonRpcLocalServer);
    d->server->close();
}

void QJsonRpcLocalServerPrivate::_q_processIncomingConnection()
{
    Q_Q(QJsonRpcLocalServer);
    QLocalSocket *localSocket = server->nextPendingConnection();
    if (!localSocket) {
        qJsonRpcDebug() << Q_FUNC_INFO << "nextPendingConnection is null";
        return;
    }

    QIODevice *device = qobject_cast<QIODevice*>(localSocket);
    QJsonRpcSocket *socket = new QJsonRpcSocket(device, q);
    QObject::connect(socket, SIGNAL(messageReceived(QJsonRpcMessage)),
                          q, SLOT(_q_processMessage(QJsonRpcMessage)));
    clients.append(socket);
    QObject::connect(localSocket, SIGNAL(disconnected()), q, SLOT(_q_clientDisconnected()));
    socketLookup.insert(localSocket, socket);
    Q_EMIT q->clientConnected();
}

void QJsonRpcLocalServerPrivate::_q_clientDisconnected()
{
    Q_Q(QJsonRpcLocalServer);
    QLocalSocket *localSocket = static_cast<QLocalSocket*>(q->sender());
    if (!localSocket) {
        qJsonRpcDebug() << Q_FUNC_INFO << "called with invalid socket";
        return;
    }

    if (socketLookup.contains(localSocket)) {
        QJsonRpcSocket *socket = socketLookup.take(localSocket);
        clients.removeAll(socket);
        socket->deleteLater();
    }

    localSocket->deleteLater();
    Q_EMIT q->clientDisconnected();
}

QString QJsonRpcLocalServer::errorString() const
{
    Q_D(const QJsonRpcLocalServer);
    return d->server->errorString();
}

#include "moc_qjsonrpclocalserver.cpp"
