# QApi

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

Bibliotheque C++ Qt statique pour effectuer des requetes HTTP asynchrones (GET/POST) avec parsing JSON automatique. Basee sur `QNetworkAccessManager`.

- [English](README.md)

## Prerequis

- **Qt 5** ou **Qt 6** (modules Core + Network)
- Compilateur **C++17**
- **CMake** 3.16+

## Installation

Cloner le depot et l'ajouter a votre projet :

```bash
git clone https://github.com/neylorxt/QApi.git
```

### Avec CMake (sous-repertoire)

```cmake
add_subdirectory(QApi)
target_link_libraries(votre_cible PRIVATE QApi)
```

### Avec CMake (manuellement)

Copier `QApi.h`, `QApi.cpp` et `CMakeLists.txt` dans l'arborescence de votre projet.

## Utilisation

### GET simple

```cpp
#include "QApi.h"

QApi* api = new QApi(this);

connect(api, &QApi::QApiReadyObject, this, [](const QJsonObject& obj, int status) {
    qDebug() << "Status :" << status;
    qDebug() << "Donnees :" << obj;
});

connect(api, &QApi::QApiReadyErrorOccurred, this, [](const QString& err, int status) {
    qWarning() << "Erreur" << status << ":" << err;
});

api->Get(QUrl("https://jsonplaceholder.typicode.com/posts/1"));
```

### GET avec options

```cpp
QApi::Options opt;
opt.headers.insert("Authorization", "Bearer mon-token");
opt.query.addQueryItem("page", "1");
opt.query.addQueryItem("limit", "10");

api->Get(QUrl("https://api.example.com/items"), opt);
```

### POST avec body JSON

```cpp
QJsonObject body;
body["title"] = "Bonjour";
body["body"] = "Monde";
body["userId"] = 1;

api->Post(QUrl("https://jsonplaceholder.typicode.com/posts"), body);
```

### POST avec body form-urlencoded

```cpp
QApi::Options opt;
opt.bodyType = QApi::BodyType::FormUrlEncoded;

QJsonObject body;
body["username"] = "admin";
body["password"] = "secret";

api->Post(QUrl("https://api.example.com/login"), body, opt);
```

## Reference API

### Methodes

| Methode | Description |
|---------|-------------|
| `Get(const QUrl& api)` | Envoie une requete GET |
| `Get(const QUrl& api, const Options& options)` | GET avec options personnalisees |
| `Post(const QUrl& api, const QJsonValue& body)` | Envoie une requete POST avec un body JSON |
| `Post(const QUrl& api, const QJsonValue& body, const Options& options)` | POST avec options personnalisees |

### Signaux

| Signal | Description |
|--------|-------------|
| `QApiReady(QJsonDocument json, int httpStatus)` | Emet la reponse JSON complete en cas de succes |
| `QApiReadyArray(QJsonArray arr, int httpStatus)` | Emet si la reponse est un tableau JSON |
| `QApiReadyObject(QJsonObject obj, int httpStatus)` | Emet si la reponse est un objet JSON |
| `QApiReadyErrorOccurred(QString message, int httpStatus)` | Emet en cas d'erreur reseau ou de parsing |

> `QApiReady` est toujours emis en premier. `QApiReadyArray` ou `QApiReadyObject` suit selon le type de reponse.

### Options

```cpp
struct Options {
    QMap<QByteArray, QByteArray> headers;  // En-tetes HTTP personnalises
    BodyType bodyType = BodyType::Json;    // Encodage du body (Json ou FormUrlEncoded)
    QUrlQuery query;                       // Parametres de requete optionnels
    int timeoutMs = 30000;                 // Delai d'attente (defaut : 30s)
};
```

### BodyType

| Valeur | Content-Type | Format du body |
|--------|-------------|----------------|
| `BodyType::Json` | `application/json` | `QJsonObject` ou `QJsonArray` (serialise en JSON compact) |
| `BodyType::FormUrlEncoded` | `application/x-www-form-urlencoded` | `QJsonObject` (paires cle-valeur encodees en parametres URL) |

## Compilation avec Qt Creator

1. Ouvrir `CMakeLists.txt` dans Qt Creator
2. Selectionner le kit **Desktop Qt** (MinGW ou MSVC)
3. Compiler (Ctrl+B)

## Licence

[MIT](LICENSE) -- Copyright (c) 2026 neylorxt
