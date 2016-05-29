#include "Filters.h"


double Moving_Average_Filter(double newValue, double* filteValue, double* movingWindow, uint16_t windowLength, uint16_t* currentCount)
{
    *filteValue += (newValue - movingWindow[*currentCount]) / (float)windowLength;
    movingWindow[(*currentCount)++] = newValue;
    *currentCount = *currentCount < windowLength ? *currentCount : 0;
    return *filteValue;
}

double Weight_Moving_Average_Filter(double newValue, double* filteValue, double valueTrust, double* movingWindow, uint16_t windowLength, uint16_t* currentCount)
{
    newValue = newValue * valueTrust + movingWindow[((*currentCount) ? (*currentCount) : windowLength)-1] * (1 - valueTrust);
    *filteValue += (newValue - movingWindow[*currentCount]) / (float)windowLength;
    movingWindow[(*currentCount)++] = newValue;
    *currentCount = *currentCount < windowLength ? *currentCount : 0;
    return *filteValue;
}

// �����Իع鷽�̣�Y = ax + b
// dada[rows*2]���飺X, Y��rows������������a, b�����ػع�ϵ��
// SquarePoor[4]�����ط������ָ��: �ع�ƽ���ͣ�ʣ��ƽ���ͣ��ع�ƽ���ʣ��ƽ����
// ����ֵ��1���ɹ���0����
int LinearRegression(double *data, int rows, double *a, double *b, double *SquarePoor)
{
    int m;
    double *p, Lxx = 0.0, Lxy = 0.0, xa = 0.0, ya = 0.0;
    if (data == 0 || a == 0 || b == 0 || rows < 1)
        return 0;
    for (p = data, m = 0; m < rows; m ++)
    {
        xa += *p ++;
        ya += *p ++;
    }
    xa /= rows;                                     // Xƽ��ֵ
    ya /= rows;                                     // Yƽ��ֵ
    for (p = data, m = 0; m < rows; m ++, p += 2)
    {
        Lxx += ((*p - xa) * (*p - xa));             // Lxx = Sum((X - Xa)ƽ��)
        Lxy += ((*p - xa) * (*(p + 1) - ya));       // Lxy = Sum((X - Xa)(Y - Ya))
    }
    *a = Lxy / Lxx;                                 // b = Lxy / Lxx
    *b = ya - *a * xa;                              // a = Ya - b*Xa
    if (SquarePoor == 0)
        return 1;
    // �������
    SquarePoor[0] = SquarePoor[1] = 0.0;
    for (p = data, m = 0; m < rows; m ++, p ++)
    {
        Lxy = *b + *a * *p ++;
        SquarePoor[0] += ((Lxy - ya) * (Lxy - ya)); // U(�ع�ƽ����)
        SquarePoor[1] += ((*p - Lxy) * (*p - Lxy)); // Q(ʣ��ƽ����)
    }
    SquarePoor[2] = SquarePoor[0];                  // �ع鷽��
    SquarePoor[3] = SquarePoor[1] / (rows - 2);     // ʣ�෽��
    return 1;
}

