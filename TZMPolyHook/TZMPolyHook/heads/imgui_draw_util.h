#pragma once
#include "../resources/imgui/imgui.h"
#pragma warning(disable:4244)

enum
{
    FL_NONE = 1 << 0,
    FL_SHADOW = 1 << 1,
    FL_OUTLINE = 1 << 2,
    FL_CENTER_X = 1 << 3,
    FL_CENTER_Y = 1 << 4
};
//��������ת��Ļ����
bool WorldToScreen(float position[3], float screen[2], float matrix[16], int windowWidth, int windowHeight);
//����Text
void DrawTextVal(int x, int y, const ImColor& color, const char* val);
//���ͼƬ
void AddImage(const ImVec2& position, const ImVec2& size, const ImTextureID pTexture, const ImColor& color);
//������Բ
void AddCircleFilled(const ImVec2& position, float radius, const ImColor& color, int segments = 100);
//���Բ
void AddCircle(const ImVec2& position, float radius, const ImColor& color, int segments = 100);
//���������
void AddRectFilled(const ImVec2& position, const ImVec2& size, const ImColor& color, float rounding = 0.f);
//��ӽ���������
void AddRectFilledGradient(const ImVec2& position, const ImVec2& size, const ImColor& leftTop, const ImColor& rightTop, const ImColor& leftBot, const ImColor& rightBot);
//��������������
void DrawFillArea(float x, float y, float w, float h, const ImColor& color, float rounding = 0.f);
//���ƽ����������
void DrawFillAreaGradient(float x, float y, float w, float h, const ImColor& leftTop, const ImColor& rightTop, const ImColor& leftBot, const ImColor& rightBot);
//���������
void AddTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImColor& color);
//������������
void AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImColor& color);
//�����
void AddLine(const ImVec2& from, const ImVec2& to, const ImColor& color, float thickness = 1.f);
//����ı�
void AddText(float x, float y, const ImColor& color, float fontSize, int flags, const char* format, ...);
//��Ӿ���
void AddRect(const ImVec2& position, const ImVec2& size, const ImColor& color, float rounding = 0.f);
//���Ʒ���
void DrawBox(float x, float y, float w, float h, const ImColor& color);
//���Ʒ��������
void DrawBoxOutline(float x, float y, float w, float h, const ImColor& color);
//����Բ�ο�
void DrawRoundBox(float x, float y, float w, float h, const ImColor& color, float rounding);
//����Բ�ο������
void DrawRoundBoxOutline(float x, float y, float w, float h, const ImColor& color, float rounding);
//���Ʒ����
void DrawCornerBox(float x, float y, float w, float h, const ImColor& color);
//���Ʒ���������
void DrawCornerBoxOutline(float x, float y, float w, float h, const ImColor& color);
//���������
void DrawEspBox(int box_type, float x, float y, float w, float h, float r, float g, float b, float a);
//���Ƶ�
void DrawDot(int x, int y, const ImColor& color);
//����3D��
void Draw3DBox(int x, int y,int w,int h,int offset, const ImColor& color);

