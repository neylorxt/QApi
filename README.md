# QApi

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

A lightweight Qt C++ static library for making async HTTP requests (GET/POST) with automatic JSON parsing. Built on top of `QNetworkAccessManager`.

- [Francais](README.fr.md)

## Requirements

- **Qt 5** or **Qt 6** (Core + Network modules)
- **C++17** compiler
- **CMake** 3.16+

## Installation

Clone the repository and add it to your project:

```bash
git clone https://github.com/neylorxt/QApi.git
```

### With CMake (subdirectory)

```cmake
add_subdirectory(QApi)
target_link_libraries(your_target PRIVATE QApi)
```

### As a git submodule

```bash
git submodule add https://github.com/neylorxt/QApi.git libs/QApi
```

Then in your `CMakeLists.txt`:

```cmake
add_subdirectory(libs/QApi)
target_link_libraries(your_target PRIVATE QApi)
```

### With CMake (manually)

Copy `QApi.h`, `QApi.cpp`, and `CMakeLists.txt` into your project tree.

## Quick Start

Let's say you're building a **Qt Widgets app** that fetches a list of users from a REST API.

### Project structure

```
MyApp/
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
project(MyApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

add_executable(MyApp main.cpp mainwindow.cpp)

add_subdirectory(QApi)
target_link_libraries(MyApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets QApi)
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
    // Widget setup
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    m_list = new QListWidget(this);
    layout->addWidget(m_list);
    setCentralWidget(central);
    resize(400, 500);

    // Create API instance
    m_api = new QApi(this);

    // Connect signals to slots
    connect(m_api, &QApi::QApiReadyObject, this, &MainWindow::onUsersLoaded);
    connect(m_api, &QApi::QApiReadyErrorOccurred, this, &MainWindow::onError);

    // Fetch users from backend
    m_api->Get(QUrl("https://jsonplaceholder.typicode.com/users"));
}

void MainWindow::onUsersLoaded(const QJsonObject& obj, int status)
{
    qDebug() << "Status:" << status;

    // Parse the JSON response
    QJsonArray users = obj.value("users").toArray();
    if (users.isEmpty()) {
        // If response is directly an array of users
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
    qWarning() << "API Error" << status << ":" << message;
    m_list->addItem("Error: " + message);
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

## Usage

### GET request

```cpp
QApi* api = new QApi(this);

// When the response is a JSON object
connect(api, &QApi::QApiReadyObject, this, [](const QJsonObject& obj, int status) {
    qDebug() << "Status:" << status;
    qDebug() << "Data:" << obj;
});

// When the response is a JSON array
connect(api, &QApi::QApiReadyArray, this, [](const QJsonArray& arr, int status) {
    for (const QJsonValue& item : arr) {
        qDebug() << item.toObject();
    }
});

// On error
connect(api, &QApi::QApiReadyErrorOccurred, this, [](const QString& err, int status) {
    qWarning() << "Error" << status << ":" << err;
});

api->Get(QUrl("https://jsonplaceholder.typicode.com/posts"));
```

### GET with options (headers + query params)

```cpp
QApi::Options opt;
opt.headers.insert("Authorization", "Bearer my-token");
opt.headers.insert("X-Custom-Header", "my-value");
opt.query.addQueryItem("page", "1");
opt.query.addQueryItem("limit", "10");

api->Get(QUrl("https://api.example.com/users"), opt);
```

### POST with JSON body

```cpp
QJsonObject body;
body["title"] = "Hello";
body["body"] = "World";
body["userId"] = 1;

connect(api, &QApi::QApiReadyObject, this, [](const QJsonObject& obj, int status) {
    qDebug() << "Created:" << obj;
});

api->Post(QUrl("https://jsonplaceholder.typicode.com/posts"), body);
```

### POST with form-urlencoded body

```cpp
QApi::Options opt;
opt.bodyType = QApi::BodyType::FormUrlEncoded;

QJsonObject body;
body["username"] = "admin";
body["password"] = "secret";

api->Post(QUrl("https://api.example.com/login"), body, opt);
```

### Reusable API instance with custom headers

```cpp
// In your constructor, create once and reuse for all requests
m_api = new QApi(this);
m_api->Get(QUrl("https://api.example.com/config"));

// Then in any slot, use it for another request
QJsonObject payload;
payload["name"] = "John";

QApi::Options opt;
opt.headers.insert("Authorization", "Bearer " + m_token);
api->Post(QUrl("https://api.example.com/profile"), payload, opt);
```

## API Reference

### Methods

| Method | Description |
|--------|-------------|
| `Get(const QUrl& api)` | Sends a GET request |
| `Get(const QUrl& api, const Options& options)` | GET with custom options |
| `Post(const QUrl& api, const QJsonValue& body)` | Sends a POST request with a JSON body |
| `Post(const QUrl& api, const QJsonValue& body, const Options& options)` | POST with custom options |

### Signals

| Signal | Description |
|--------|-------------|
| `QApiReady(QJsonDocument json, int httpStatus)` | Emitted on success with the full JSON response |
| `QApiReadyArray(QJsonArray arr, int httpStatus)` | Emitted if the response is a JSON array |
| `QApiReadyObject(QJsonObject obj, int httpStatus)` | Emitted if the response is a JSON object |
| `QApiReadyErrorOccurred(QString message, int httpStatus)` | Emitted on network or parse error |

> `QApiReady` is always emitted first. `QApiReadyArray` or `QApiReadyObject` fires next depending on the response type.

### Options

```cpp
struct Options {
    QMap<QByteArray, QByteArray> headers;  // Custom HTTP headers
    BodyType bodyType = BodyType::Json;    // Body encoding (Json or FormUrlEncoded)
    QUrlQuery query;                       // Optional query parameters
    int timeoutMs = 30000;                 // Request timeout (default: 30s)
};
```

### BodyType

| Value | Content-Type | Body format |
|-------|-------------|-------------|
| `BodyType::Json` | `application/json` | `QJsonObject` or `QJsonArray` (serialized as compact JSON) |
| `BodyType::FormUrlEncoded` | `application/x-www-form-urlencoded` | `QJsonObject` (key-value pairs encoded as URL params) |

## Building with Qt Creator

1. Open `CMakeLists.txt` in Qt Creator
2. Select the **Desktop Qt** kit (MinGW or MSVC)
3. Build (Ctrl+B)

## License

[MIT](LICENSE) -- Copyright (c) 2026 neylorxt
