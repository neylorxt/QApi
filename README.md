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

### With CMake (manually)

Copy `QApi.h`, `QApi.cpp`, and `CMakeLists.txt` into your project tree.

## Usage

### Basic GET

```cpp
#include "QApi.h"

QApi* api = new QApi(this);

connect(api, &QApi::QApiReadyObject, this, [](const QJsonObject& obj, int status) {
    qDebug() << "Status:" << status;
    qDebug() << "Data:" << obj;
});

connect(api, &QApi::QApiReadyErrorOccurred, this, [](const QString& err, int status) {
    qWarning() << "Error" << status << ":" << err;
});

api->Get(QUrl("https://jsonplaceholder.typicode.com/posts/1"));
```

### GET with options

```cpp
QApi::Options opt;
opt.headers.insert("Authorization", "Bearer my-token");
opt.query.addQueryItem("page", "1");
opt.query.addQueryItem("limit", "10");

api->Get(QUrl("https://api.example.com/items"), opt);
```

### POST with JSON body

```cpp
QJsonObject body;
body["title"] = "Hello";
body["body"] = "World";
body["userId"] = 1;

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
