#include "QApi.h"
#include <QJsonParseError>

QApi::QApi(QObject* parent)
    : QObject(parent)
{
}

void QApi::applyHeaders(QNetworkRequest& req, const Options& opt)
{
    // Headers par défaut
    req.setHeader(QNetworkRequest::UserAgentHeader, "QApi/1.0");

    // Headers custom
    for (auto it = opt.headers.cbegin(); it != opt.headers.cend(); ++it) {
        req.setRawHeader(it.key(), it.value());
    }
}

QByteArray QApi::encodeBody(const QJsonValue& body, const Options& opt, QString* outContentType) const
{
    if (opt.bodyType == BodyType::Json) {
        if (outContentType) *outContentType = "application/json";

        // le body peut être object ou array
        QJsonDocument doc;
        if (body.isArray()) doc = QJsonDocument(body.toArray());
        else if (body.isObject()) doc = QJsonDocument(body.toObject());
        else {
            // fallback: wrap en objet
            doc = QJsonDocument(QJsonObject{{"value", body.toVariant().toString()}});
        }
        return doc.toJson(QJsonDocument::Compact);
    }

    // on attend un object simple (clé -> valeur)
    if (outContentType) *outContentType = "application/x-www-form-urlencoded";

    if (!body.isObject()) {
        return {}; // ou erreur si tu veux
    }

    QUrlQuery q;
    const QJsonObject obj = body.toObject();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        // conversion simple: string/number/bool
        q.addQueryItem(it.key(), it.value().toVariant().toString());
    }
    return q.query(QUrl::FullyEncoded).toUtf8();
}

void QApi::Get(const QUrl& api)
{
    Get(api, Options{});
}

void QApi::Post(const QUrl& api, const QJsonValue& body)
{
    Post(api, body, Options{});
}


void QApi::Get(const QUrl& api, const Options& opt)
{
    if (!api.isValid()) {
        emit QApiReadyErrorOccurred("Invalid API URL");
        return;
    }

    QUrl url = api;
    if (!opt.query.isEmpty()) {
        QUrlQuery q(url);
        // merge
        const auto items = opt.query.queryItems(QUrl::FullyDecoded);
        for (const auto& it : items) q.addQueryItem(it.first, it.second);
        url.setQuery(q);
    }

    QNetworkRequest req(url);
    // Content-Type n'a pas trop de sens en GET, mais ok si tu le veux
    applyHeaders(req, opt);

    QNetworkReply* reply = m_nam.get(req);
    handleReply(reply);
}

void QApi::Post(const QUrl& api, const QJsonValue& body, const Options& opt)
{
    if (!api.isValid()) {
        emit QApiReadyErrorOccurred("Invalid API URL");
        return;
    }

    QUrl url = api;
    if (!opt.query.isEmpty()) {
        QUrlQuery q(url);
        const auto items = opt.query.queryItems(QUrl::FullyDecoded);
        for (const auto& it : items) q.addQueryItem(it.first, it.second);
        url.setQuery(q);
    }

    QNetworkRequest req(url);
    applyHeaders(req, opt);

    QString contentType;
    const QByteArray payload = encodeBody(body, opt, &contentType);

    // si l’utilisateur n’a pas fourni Content-Type, on le met selon bodyType
    if (!opt.headers.contains("Content-Type")) {
        req.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    }

    QNetworkReply* reply = m_nam.post(req, payload);
    handleReply(reply);
}

void QApi::handleReply(QNetworkReply* reply)
{
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (reply->error() != QNetworkReply::NoError) {
            emit QApiReadyErrorOccurred(reply->errorString(), status);
            reply->deleteLater();
            return;
        }

        const QByteArray body = reply->readAll();

        QJsonParseError pe;
        QJsonDocument doc = QJsonDocument::fromJson(body, &pe);
        if (pe.error != QJsonParseError::NoError) {
            emit QApiReadyErrorOccurred(QString("JSON parse error: %1").arg(pe.errorString()), status);
            reply->deleteLater();
            return;
        }

        emit QApiReady(doc, status);

        if (doc.isArray()) emit QApiReadyArray(doc.array(), status);
        if (doc.isObject()) emit QApiReadyObject(doc.object(), status);

        reply->deleteLater();
    });
}
