#pragma once

class DrawSystem;

class DrawSystemFrame
{
public:
   DrawSystemFrame(DrawSystem& drawSystem);
   ~DrawSystemFrame();

   //get command list
private:
   DrawSystem& m_drawSystem;

};
