#!/bin/bash

pipenv sync --dev

pipenv run conan profile new --detect default
pipenv run conan profile update settings.compiler.libcxx=libc++ default
pipenv run conan remote add cpp-stat-bench https://gitlab.com/api/v4/projects/32226502/packages/conan

git config --global --add safe.directory /workspaces/cpp-hash-tables
pipenv run pre-commit install

git config commit.template .gitmessage

git config gpg.program gpg2
git config commit.gpgsign true
git config tag.gpgsign true

echo "source /usr/share/bash-completion/completions/git" >>~/.bashrc
