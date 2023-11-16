#ifndef LINUX_COMMON_BASECONVERTER_H
#define LINUX_COMMON_BASECONVERTER_H

// ��16���Ƶ��ַ�ת��Ϊ10����
// Input:   c   ��ת����16�����ַ�
// Return:      ת�����10�����ַ�
inline char CharHex2Dec(char c)
{
    return (c/16)*10 + c % 16;
}

// ��10���Ƶ��ַ�ת��Ϊ16����
// Input:   c   ��ת����10�����ַ�
// Return:      ת�����16�����ַ�
inline char CharDec2Hex(char c)
{
    return (c/10)*16 + c % 10;
}

#endif //LINUX_COMMON_BASECONVERTER_H
