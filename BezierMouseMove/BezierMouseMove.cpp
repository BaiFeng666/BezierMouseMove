#include <windows.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>

/****************************************************
* Copyright (C)	: Liv
* @file			: BezierMouseMove.cpp
* @author		: Liv
* @email		: 1319923129@qq.com / livkaze@gmail.com
* @version		: 1.0
* @date			: 2024/6/22 22:00
****************************************************/

// �������
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);
std::uniform_real_distribution<> dis2(-1.0, 1.0);

// �ƶ��ȼ�
const DWORD HotKey = VK_SHIFT;

// ���������߼���
POINT bezierCurve(const std::vector<POINT>& ControlPoints, double t) 
{
    int n = ControlPoints.size() - 1;
    POINT Result = { 0, 0 };
    for (int i = 0; i <= n; ++i) 
    {
        double binomialCoeff = 1;
        for (int j = 0; j < i; ++j) 
        {
            binomialCoeff *= (n - j) / (j + 1.0);
        }
        double factor = binomialCoeff * pow(t, i) * pow(1 - t, n - i);
        Result.x += static_cast<LONG>(factor * ControlPoints[i].x);
        Result.y += static_cast<LONG>(factor * ControlPoints[i].y);
    }
    return Result;
}

// ����ƶ�
void MouseMove(const POINT& CurrentPos, const POINT& TargetPos)
{
    // ����ƶ�
    LONG dx = TargetPos.x - CurrentPos.x;
    LONG dy = TargetPos.y - CurrentPos.y;
    mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
}

// ģ����������ƶ�
void SimulateMouseMove(const POINT& StartPos, const POINT& TargetPos)
{
    // �����ƶ�Flag
    static bool OnMoving = false;
    // ���Ƶ�
    static std::vector<POINT> ControlPoints;
    // �ܲ���
    const int Steps = 100;
    // ��ǰ����
    static int CurrentStep = 0;
    // �ϴ��ƶ�ʱ���
    static auto LastTime = std::chrono::steady_clock::now();
    // �ƶ����ʱ�䣨ms�� & �ٶȣ��ƶ�ʱ�����
    const int DefaultInterval = 7;
    static auto Interval = std::chrono::milliseconds(DefaultInterval);
    // ���Ƶ������Χ
    const float ControlRange = 100;

    if (GetAsyncKeyState(HotKey) & 0x8000)
    {
        // ����ƶ�ʱ����
        Interval = std::chrono::milliseconds((int)(DefaultInterval + 2 * dis2(gen)));

        // ��׼�������������Ŀ��������ָ����Χ�ڲ������ƶ�����ѡ�ã�
        auto DistanceToCenter = sqrtf(powf(StartPos.x - TargetPos.x, 2) + powf(StartPos.y - TargetPos.y, 2));
        if (DistanceToCenter < 15.f)
        {
            OnMoving = false;
            return;
        }

        if (!OnMoving)
        {
            OnMoving = true;

            // ����������Ƶ�
            ControlPoints = { StartPos,
                POINT{LONG(StartPos.x + dis(gen) * ControlRange), LONG(StartPos.y + dis(gen) * ControlRange)},
                POINT{LONG(TargetPos.x - dis(gen) * ControlRange), LONG(TargetPos.y - dis(gen) * ControlRange)},
                TargetPos };

            CurrentStep = 0;

            LastTime = std::chrono::steady_clock::now();
        }
        else
        {
            auto Now = std::chrono::steady_clock::now();
            if (Now - LastTime >= Interval)
            {
                // �����ܲ����͵�ǰ��������t����
                double t = CurrentStep / static_cast<double>(Steps);
                // �����¸��ƶ�������м�����㣬�����������ƶ�
                auto NextPos = bezierCurve(ControlPoints, t);

                MouseMove(StartPos, NextPos);

                CurrentStep++;
                LastTime = Now;
            }

            if (CurrentStep > Steps)
            {
                // ����������Ƶ�
                ControlPoints = { ControlPoints[3],
                    POINT{LONG(ControlPoints[3].x + dis(gen) * ControlRange),LONG(ControlPoints[3].y + dis(gen) * ControlRange)},
                    POINT{LONG(TargetPos.x - dis(gen) * ControlRange),  LONG(TargetPos.y - dis(gen) * ControlRange)},
                    TargetPos };

                CurrentStep = 0;
            }
        }
    }
    else
    {
        if (OnMoving)
        {
            // ȡ���ƶ�Flag
            OnMoving = false;
        }
    }
}

int main() 
{
    /*
        NOTICE:
        1. ����Ŀ�ص��������̷߳������������ģ���ƶ�����Ӱ���߳���������������
        2. ʹ�ñ��������߽��з���ģ��켣
        3. �������ɵ����������ݽ�������
    */

    // ��Ӧ�õ������ϣ�������ֱ�Ӹ�ֵΪ��Ϸ������������
    POINT CurrentPos;

    // ģ�����߳�
    while (true)
    {
        // �������д���...

        // ��Ӧ�õ������ϣ���ע�ʹ���
        GetCursorPos(&CurrentPos);

        SimulateMouseMove(CurrentPos, { 500,500 });

        // �������д���...
    }

    return 0;
}
