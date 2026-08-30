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

### En tant que sous-module git

```bash
git submodule add https://github.com/neylorxt/QApi.git libs/QApi
```

Puis dans votre `CMakeLists.txt` :

```cmake
add_subdirectory(libs/QApi)
target_link_libraries(votre_cible PRIVATE QApi)
```

### Avec CMake (manuellement)

Copier `QApi.h`, `QApi.cpp` et `CMakeLists.txt` dans l'arborescence de votre projet.

## Demarrage rapide

Disons que vous construisez une **application Qt Widgets** qui recupere une liste d'utilisateurs depuis une API REST.

### Structure du projet

```
MonApp/
  CMakeLists.txt
  main.cpp
  mainwindow.h
  mainwindow.cpp
  QApi/
    QApi.h
    QApi.cpp
    CMakeLists.txt
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(MonApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

add_executable(MonApp main.cpp mainwindow.cpp)

add_subdirectory(QApi)
target_link_libraries(MonApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets QApi)
```

### mainwindow.h

```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include "QApi.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onUsersLoaded(const QJsonObject& obj, int status);
    void onError(const QString& message, int status);

private:
    QApi* m_api;
    QListWidget* m_list;
};

#endif
```

### mainwindow.cpp

```cpp
#include "mainwindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Mise en place des widgets
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    m_list = new QListWidget(this);
    layout->addWidget(m_list);
    setCentralWidget(central);
    resize(400, 500);

    // Creation de l'instance API
    m_api = new QApi(this);

    // Connexion des signaux aux slots
    connect(m_api, &QApi::QApiReadyObject, this, &MainWindow::onUsersLoaded);
    connect(m_api, &QApi::QApiReadyErrorOccurred, this, &MainWindow::onError);

    // Requete GET pour recuperer les utilisateurs
    m_api->Get(QUrl("https://jsonplaceholder.typicode.com/users"));
}

void MainWindow::onUsersLoaded(const QJsonObject& obj, int status)
{
    qDebug() << "Status :" << status;

    // Parsing de la reponse JSON
    QJsonArray users = obj.value("users").toArray();
    if (users.isEmpty()) {
        // Si la reponse est directement un tableau
        users = QJsonDocument(obj).array();
    }

    m_list->clear();
    for (const QJsonValue& val : users) {
        QJsonObject user = val.toObject();
        m_list->addItem(user.value("name").toString());
    }
}

void MainWindow::onError(const QString& message, int status)
{
    qWarning() << "Erreur API" << status << ":" << message;
    m_list->addItem("Erreur : " + message);
}
```

### main.cpp

```cpp
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
```

## Utilisation

### Requete GET

```cpp
QApi* api = new QApi(this);

// Quand la reponse est un objet JSON
connect(api, &QApi::QApiReadyObject, this, [](const QJsonObject& obj, int status) {
    qDebug() << "Status :" << status;
    qDebug() << "Donnees :" << obj;
});

// Quand la reponse est un tableau JSON
connect(api, &QApi::QApiReadyArray, this, [](const QJsonArray& arr, int status) {
    for (const QJsonValue& item : arr) {
        qDebug() << item.toObject();
    }
});

// En cas d'erreur
connect(api, &QApi::QApiReadyErrorOccurred, this, [](const QString& err, int status) {
    qWarning() << "Erreur" << status << ":" << err;
});

api->Get(QUrl("https://jsonplaceholder.typicode.com/posts"));
```

### GET avec options (headers + parametres)

```cpp
QApi::Options opt;
opt.headers.insert("Authorization", "Bearer mon-token");
opt.headers.insert("X-Custom-Header", "ma-valeur");
opt.query.addQueryItem("page", "1");
opt.query.addQueryItem("limit", "10");

api->Get(QUrl("https://api.example.com/users"), opt);
```

### POST avec body JSON

```cpp
QJsonObject body;
body["title"] = "Bonjour";
body["body"] = "Monde";
body["userId"] = 1;

connect(api, &QApi::QApiReadyObject, this, [](const QJsonObject& obj, int status) {
    qDebug() << "Cree :" << obj;
});

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

### Instance API reutilisable avec headers personnalises

```cpp
// Dans le constructeur, creer une fois et reutiliser pour toutes les requetes
m_api = new QApi(this);
m_api->Get(QUrl("https://api.example.com/config"));

// Puis dans n'importe quel slot, l'utiliser pour une autre requete
QJsonObject payload;
payload["name"] = "Jean";

QApi::Options opt;
opt.headers.insert("Authorization", "Bearer " + m_token);
api->Post(QUrl("https://api.example.com/profile"), payload, opt);
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
