// bmp_analys.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#define PARTS 30;

#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

using namespace std;

typedef struct
{
    unsigned int    bfType;
    unsigned long   bfSize;
    unsigned int    bfReserved1;
    unsigned int    bfReserved2;
    unsigned long   bfOffBits;
} BITMAPFILEHEADER;

typedef struct
{
    unsigned int    biSize;
    int             biWidth;
    int             biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned int    biCompression;
    unsigned int    biSizeImage;
    int             biXPelsPerMeter;
    int             biYPelsPerMeter;
    unsigned int    biClrUsed;
    unsigned int    biClrImportant;
} BITMAPINFOHEADER;

typedef struct
{
    int   rgbBlue;
    int   rgbGreen;
    int   rgbRed;
    int   rgbReserved;
} RGBQUAD;

static unsigned short read_u16(FILE* fp);
static unsigned int   read_u32(FILE* fp);
static int            read_s32(FILE* fp);

static unsigned short read_u16(FILE* fp)
{
    unsigned char b0, b1;

    b0 = getc(fp);
    b1 = getc(fp);

    return ((b1 << 8) | b0);
}


static unsigned int read_u32(FILE* fp)
{
    unsigned char b0, b1, b2, b3;

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}


static int read_s32(FILE* fp)
{
    unsigned char b0, b1, b2, b3;

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}

void Histogram(FILE* pFile, vector<int> &vect)
{
    // считываем заголовок файла
    BITMAPFILEHEADER header;

    header.bfType = read_u16(pFile);
    header.bfSize = read_u32(pFile);
    header.bfReserved1 = read_u16(pFile);
    header.bfReserved2 = read_u16(pFile);
    header.bfOffBits = read_u32(pFile);

    printf("%x %d %d\n", header.bfType, header.bfSize, header.bfOffBits);
    //system("pause");
    // считываем заголовок изображения
    BITMAPINFOHEADER bmiHeader;

    bmiHeader.biSize = read_u32(pFile);
    bmiHeader.biWidth = read_s32(pFile);
    bmiHeader.biHeight = read_s32(pFile);
    bmiHeader.biPlanes = read_u16(pFile);
    bmiHeader.biBitCount = read_u16(pFile);
    bmiHeader.biCompression = read_u32(pFile);
    bmiHeader.biSizeImage = read_u32(pFile);
    bmiHeader.biXPelsPerMeter = read_s32(pFile);
    bmiHeader.biYPelsPerMeter = read_s32(pFile);
    bmiHeader.biClrUsed = read_u32(pFile);
    bmiHeader.biClrImportant = read_u32(pFile);
    printf("width\theight\n%d\t%d\n", bmiHeader.biWidth, bmiHeader.biHeight);
    printf("bits per pixel\n%d\n", bmiHeader.biBitCount);
    printf("biCompression\n%d\n", bmiHeader.biCompression);
    //cout << bmiHeader.biSize << ' ' << bmiHeader.biWidth << ' ' << bmiHeader.biHeight << endl;

    //system("pause");

    RGBQUAD** rgb = new RGBQUAD * [bmiHeader.biWidth];
    for (int i = 0; i < bmiHeader.biWidth; i++) {
        rgb[i] = new RGBQUAD[bmiHeader.biHeight];
    }

    uint32_t** colors = new uint32_t * [bmiHeader.biWidth];
    for (int i = 0; i < bmiHeader.biWidth; i++) {
        colors[i] = new uint32_t[bmiHeader.biHeight];
    }

    for (int i = 0; i < bmiHeader.biWidth; i++) {
        for (int j = 0; j < bmiHeader.biHeight; j++) {
            rgb[i][j].rgbBlue = getc(pFile);
            rgb[i][j].rgbGreen = getc(pFile);
            rgb[i][j].rgbRed = getc(pFile);
            colors[i][j] = rgb[i][j].rgbBlue + (rgb[i][j].rgbGreen << 8) + (rgb[i][j].rgbRed << 16);
        }

        // пропускаем последний байт в строке
        getc(pFile);
    }

    // выводим результат

    /*for (int i = 0; i < bmiHeader.biWidth; i++) {
        for (int j = 0; j < bmiHeader.biHeight; j++) {
            if ((rgb[i][j].rgbRed != 102) && (rgb[i][j].rgbGreen != 102) && (rgb[i][j].rgbBlue != 102))
                printf("%d %d %d\n", rgb[i][j].rgbRed, rgb[i][j].rgbGreen, rgb[i][j].rgbBlue);

        }
        printf("\n");
    }*/


    int parts = PARTS;
    uint32_t max = UINT32_MAX & 0x00FFFFFF;
    uint32_t step = (max / parts) + 1;
    uint32_t border;

    for (int i = 0; i < bmiHeader.biWidth; i++) {
        for (int j = 0; j < bmiHeader.biHeight; j++) {
            border = step;
            for (int k = 0; k < parts; k++)
            {
                if (colors[i][j] < border)
                {
                    vect[k]++;
                    break;
                }
                border += step;
            }
        }
    }

    for (int i = 0; i < bmiHeader.biWidth; i++) {
        delete rgb[i];
        delete colors[i];
    }
    delete rgb;
    delete colors;
}

double Distance_between(const vector<int>& V1, const vector<int>& V2)
{
    if (V1.size() == V2.size())
    {
        int sum = 0, size = V1.size();

        for (int i = 0; i < size; i++)
            sum += (V1[i] - V2[i]) * (V1[i] - V2[i]);

        return sqrt(sum);
    }
    else
        return 0;
}

int main()
{
    int parts = PARTS;

    //apple_1
    cout << "\napple_1" << endl;
    FILE* pFile = fopen("C:/Users/KijoL/source/repos/CompImages/apple_1.bmp", "rb");
    vector<int> vect1(parts);
    Histogram(pFile, vect1);
    for (int i = 0; i < parts; i++)
    {
        cout << vect1[i] << endl;
    }
    fclose(pFile);

    //apple_2
    cout << "\napple_2" << endl;
    pFile = fopen("C:/Users/KijoL/source/repos/CompImages/apple_2.bmp", "rb");
    vector<int> vect2(parts);
    Histogram(pFile, vect2);
    for (int i = 0; i < parts; i++)
    {
        cout << vect2[i] << endl;
    }
    fclose(pFile);

    //apple_3
    cout << "\napple_3" << endl;
    pFile = fopen("C:/Users/KijoL/source/repos/CompImages/apple_3.bmp", "rb");
    vector<int> vect3(parts);
    Histogram(pFile, vect3);
    for (int i = 0; i < parts; i++)
    {
        cout << vect3[i] << endl;
    }
    fclose(pFile);

    //apple_4
    cout << "\napple_4" << endl;
    pFile = fopen("C:/Users/KijoL/source/repos/CompImages/apple_4.bmp", "rb");
    vector<int> vect4(parts);
    Histogram(pFile, vect4);
    for (int i = 0; i < parts; i++)
    {
        cout << vect4[i] << endl;
    }
    fclose(pFile);

    vector<double> result;
    result.push_back(Distance_between(vect1, vect2));
    cout << "apple 1 - apple 2 " << result[0] << endl;
    result.push_back(Distance_between(vect1, vect3));
    cout << "apple 1 - apple 3 " << result[1] << endl;
    result.push_back(Distance_between(vect1, vect4));
    cout << "apple 1 - apple 4 " << result[2] << endl;
    result.push_back(Distance_between(vect2, vect3));
    cout << "apple 2 - apple 3 " << result[3] << endl;
    result.push_back(Distance_between(vect2, vect4));
    cout << "apple 2 - apple 4 " << result[4] << endl;
    result.push_back(Distance_between(vect3, vect4));
    cout << "apple 3 - apple 4 " << result[5] << endl;

    double min = 1000000;
    for (int i = 0; i < result.size(); i++)
    {
        if (result[i] < min)
            min = result[i];
    }
    cout << "RESULT: " << min << endl;

    return 0;
}