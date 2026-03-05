cmake_minimum_required(VERSION 3.10)

aui_executable(aui.example.tree
    src/main.cpp
)
aui_link(aui.example.tree PRIVATE aui::views)
