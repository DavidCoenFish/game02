#pragma once

class DrawSystem;
class CustomCommandList;

class iResource
{
public:
   iResource(DrawSystem* const pDrawSystem);
   virtual ~iResource();

   virtual void OnDeviceLost() = 0;
   virtual void OnDeviceRestored(
      CustomCommandList* const pCommandList,
      ID3D12Device* const pDevice
      ) = 0;

   virtual void OnResizeBefore();
   virtual void OnResizeAfter(
      CustomCommandList* const pCommandList,
      ID3D12Device* const pDevice,
      const int screenWidth,
      const int screenHeight
      );

protected:
   DrawSystem* m_pDrawSystem;

};
