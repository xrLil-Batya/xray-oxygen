// Author	: Abramcumner
// data		: 04.02.2017

#ifndef _XR_POOL_H
#define _XR_POOL_H

// В сталкере x64 беда с luajit`ом - из-за того, что луаджит непременно требуется память из младших адресов,
// на больших локациях луаджит не может выделить память, так как она уже занята под другие ресурсы игры.
//Как вариант делаю при страте игры кучу на 256МБ и выделаю память luajit`у из нее.


BOOL XR_INIT();
void XR_DESTROYPOOL();
void* XR_MMAP(size_t size);
void XR_DESTROY(void* p, size_t size);

#endif