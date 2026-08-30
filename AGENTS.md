# AGENTS.md

## What this is

**QApi** — a Qt C++ static library wrapping `QNetworkAccessManager` for simple async HTTP calls (GET/POST). Intended to be linked into other Qt projects, not run standalone.

## Build

Primary build path: **Qt Creator** (Desktop Qt 6.10.1 MinGW 64-bit kit). The project has no standalone CLI build — the shell lacks a C++ compiler and Ninja in PATH.

CMake config: auto-detects Qt 5 or 6 via `find_package(QT NAMES Qt6 Qt5)`. Requires `Core` and `Network` modules.

## Architecture

- **`QApi.h` / `QApi.cpp`** — the entire library. One class `QApi : public QObject`.
- Signals: `QApiReady`, `QApiReadyArray`, `QApiReadyObject`, `QApiReadyErrorOccurred`.
- Supports JSON and form-urlencoded body encoding via `Options::bodyType`.
- `Q_INVOKABLE` methods mean this is designed to be used from both C++ and QML.

## Conventions

- Code comments are in French.
- Qt Creator style: spaces-for-tabs, 4-space indent, UTF-8, 80-col margin.
- Header uses traditional `#ifndef` guards (not `#pragma once`).
- `m_` prefix for private member variables.
