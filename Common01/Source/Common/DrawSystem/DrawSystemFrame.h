#pragma once

class DrawSystem;

class DrawSystemFrame
{
public:
   DrawSystemFrame(DrawSystem& drawSystem);
   ~DrawSystemFrame();

   ID3D12GraphicsCommandList* GetCommandList();
   const int GetBackBufferIndex();

private:
   DrawSystem& m_drawSystem;
   ID3D12GraphicsCommandList* m_pCommandList;
   ID3D12CommandQueue* m_pCommandQueue;   

};
