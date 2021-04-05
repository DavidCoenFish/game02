#pragma once

class DeviceResources;
class DrawSystemFrame;
//interface IDrawSystemNotify
//{
//   virtual void OnDeviceLost() = 0;
//   virtual void OnDeviceRestored() = 0;
//
//protected:
//   ~IDrawSystemNotify() = default;
//};

class DrawSystem //: public IDrawSystemNotify
{
public:
   DrawSystem(
      const HWND hWnd,
      const D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL_11_0,
      const unsigned int options = 0
      );
   ~DrawSystem();
   std::unique_ptr< DrawSystemFrame > CreateNewFrame(); //ctor == Prepare, dtor == Present
   void WaitForGpu() noexcept;
   void OnResize();

private:
   void CreateDeviceResources();

private:
   HWND m_hWnd;
   D3D_FEATURE_LEVEL m_d3dFeatureLevel;
   unsigned int m_options;
   std::unique_ptr< DeviceResources > m_pDeviceResources;
   //std::unique_ptr< ScreenSizeResources > m_pScreenSizeResources;


};
