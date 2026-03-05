cmake_minimum_required(VERSION 3.10)

aui_executable(aui.example.atree
    src/main.cpp
)
aui_link(aui.example.atree PRIVATE aui::views)
