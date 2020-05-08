# Building libLAS Documentation

## Prerequisites

- Python 3
- Sphinx
- Doxygen

## Install

Create Python virtual environment:

```console
$ python3 -m venv .pyenv
$ source ./.pyenv/bin/activate
```

Install Sphinx and Sphinx extensions:

```console
(.pyenv)$ pip install --upgrade pip
(.pyenv)$ pip install sphinx
```

## Build

```console
$ cd doc/api
$ doxygen doxygen.conf
```

```console
$ cd doc
$ make clean
$ make html
```

## Deploy

- Copy `doc/api/html/` as `liblas.org/doxygen/`
