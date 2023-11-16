#ifndef LINUX_COMMON_BASECONVERTER_H
#define LINUX_COMMON_BASECONVERTER_H

// 将16进制的字符转化为10进制
// Input:   c   待转化的16进制字符
// Return:      转化后的10进制字符
inline char CharHex2Dec(char c)
{
    return (c/16)*10 + c % 16;
}

// 将10进制的字符转化为16进制
// Input:   c   待转化的10进制字符
// Return:      转化后的16进制字符
inline char CharDec2Hex(char c)
{
    return (c/10)*16 + c % 10;
}

#endif //LINUX_COMMON_BASECONVERTER_H
