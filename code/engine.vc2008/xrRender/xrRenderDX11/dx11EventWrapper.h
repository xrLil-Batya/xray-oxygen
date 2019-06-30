#pragma once

#define PIX_EVENT(Name)	dxPixEventWrapper	pixEvent##Name(L#Name)

class dxPixEventWrapper // новый клас для анотации
{
public:
    dxPixEventWrapper(LPCWSTR wszName);
    ~dxPixEventWrapper();
};