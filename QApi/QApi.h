#ifndef QAPI_H
#define QAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantMap>
#include <QUrlQuery>
#include <QUrl>


class QApi : public QObject
{
    Q_OBJECT
public:

    enum class BodyType {
        Json,               // application/json
        FormUrlEncoded      // application/x-www-form-urlencoded
    };
    Q_ENUM(BodyType)

    struct Options {
        QMap<QByteArray, QByteArray> headers;  // headers custom
        BodyType bodyType = BodyType::Json;    // comment encoder le body
        QUrlQuery query;                       // query params optionnels
        int timeoutMs = 30000;                 // optionnel (si tu veux gérer plus tard)
    };

    explicit QApi(QObject* parent = nullptr);

    // Lance la requête, résultat via signal (asynchrone)
    Q_INVOKABLE void Get(const QUrl& api);
    Q_INVOKABLE void Post(const QUrl& api, const QJsonValue& body);

    Q_INVOKABLE void Get(const QUrl& api, const Options& options);
    Q_INVOKABLE void Post(const QUrl& api, const QJsonValue& body, const Options& options);


signals:
    void QApiReady(const QJsonDocument& json, int httpStatus);
    void QApiReadyArray(const QJsonArray& arr, int httpStatus);
    void QApiReadyObject(const QJsonObject& obj, int httpStatus);

    // Error
    void QApiReadyErrorOccurred(const QString& message, int httpStatus = -1);

private:
    void applyHeaders(QNetworkRequest& req, const Options& opt);
    QByteArray encodeBody(const QJsonValue& body, const Options& opt, QString* outContentType) const;
    void handleReply(QNetworkReply* reply);

private:
    QNetworkAccessManager m_nam;
    QUrl m_test = QUrl("https://www.freetogame.com/api/games");

};

#endif // QAPI_H
