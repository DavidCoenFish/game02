#pragma once

class DrawSystem;

class DrawSystemFrame
{
public:
   DrawSystemFrame(DrawSystem& drawSystem);
   ~DrawSystemFrame();

private:
   DrawSystem& m_drawSystem;

};
