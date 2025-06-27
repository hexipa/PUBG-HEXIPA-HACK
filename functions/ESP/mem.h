#import <string>
#import <dlfcn.h>
#import <stdio.h>
#import <mach/mach.h>
#import <mach-o/dyld.h>
#import <mach/vm_map.h>
#import <mach/vm_region.h>
#import <cmath> 
#include <unistd.h>
#include <sys/mman.h>

typedef long kaddr;

bool isMincore(uintptr_t address, size_t size) {
    size_t pageSize = getpagesize();
    uintptr_t alignedAddress = address & ~(pageSize - 1);

    char vec;
    if (mincore((void *)alignedAddress, size, &vec) == 0) {
        return vec & 1; 
    }
    return false;
}

// Проверка диапазона адресов
bool IsValidAddress(kaddr addr) {
    return addr >= 0x100000000 && addr < 0x2000000000; // Пример диапазона, уточните для своей системы
}


bool _read(kaddr addr, void *buffer, int len) {
    if (!IsValidAddress(addr)) return false;

    if (isMincore(addr, len) != 1) {
        return false;
    }

    vm_size_t size = 0;
    kern_return_t error = vm_read_overwrite(mach_task_self(), addr, len, (vm_address_t)buffer, &size);
    if (error != KERN_SUCCESS || size != len) {
        return false;
    }
    return true;
}

// Запись в память
bool _write(kaddr addr, void *buffer, int len) {
    if (!IsValidAddress(addr)) return false;

    kern_return_t error = vm_write(mach_task_self(), addr, (vm_offset_t)buffer, (mach_msg_type_number_t)len);
    if (error != KERN_SUCCESS) {
        return false;
    }
    return true;
}


template <typename T>
T Read(kaddr address) {
    T data{};
    if (isMincore(address, sizeof(T)) != 1) {
        return data;
    }
    if (!_read(address, &data, sizeof(T))) {
        throw std::runtime_error("Penis");
    }
    return data;
}


template <typename T>
void Write(kaddr address, T data) {
    if (!_write(address, &data, sizeof(T))) {
        throw std::runtime_error("Penis");
    }
}

template <typename T>
std::vector<T> ReadArr(kaddr address, unsigned int size) {
    std::vector<T> data(size);
    if (!_read(address, data.data(), sizeof(T) * size)) {
        throw std::runtime_error("Failed to read memory");
    }
    return data;
}

std::string ReadStr2(kaddr address, unsigned int size) {
    std::string name(size, '\0');
    if (!_read(address, name.data(), size)) {
        throw std::runtime_error("Failed to read memory");
    }
    return name;
}

kaddr GetPtr(kaddr address) {
    return Read<kaddr>(address);
}