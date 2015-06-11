#pragma once

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void LoadContent();
void Update(float totalTime, float deltaTime);
void Render(float totalTime, float deltaTime);