#include "testservice.h"

TestService::TestService(QObject *parent)
    : QJsonRpcService(parent)
{
}

TestService::~TestService()
{
}

QString TestService::serviceName() const
{
    return "agent";
}

void TestService::testMethod()
{
    qDebug() << __PRETTY_FUNCTION__ << "called" << endl;
}

void TestService::testMethodWithParams(const QString &first, bool second, double third)
{
    qDebug() << __PRETTY_FUNCTION__ << "called with parameters: " << endl
             << " first: " << first << endl
             << "second: " << second << endl
             << " third: " << third << endl;
}

void TestService::testMethodWithVariantParams(const QString &first, bool second, double third, const QVariant &fourth)
{
    qDebug() << __PRETTY_FUNCTION__ << "called with variant parameters: " << endl
             << " first: " << first << endl
             << "second: " << second << endl
             << " third: " << third << endl
             << "fourth: " << fourth << endl;
}

QString TestService::testMethodWithParamsAndReturnValue(const QString &name)
{
    qDebug() << __PRETTY_FUNCTION__ << "called" << endl;
    return QString("Hello %1").arg(name);
}

void TestService::testMethodWithDefaultParameter(const QString &first, const QString &second)
{
    qDebug() << __PRETTY_FUNCTION__ << endl
             << "first: " << first << endl
             << (second.isEmpty() ? "not defined, default parameter" : second) << endl;
}

