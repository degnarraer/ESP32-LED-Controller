
#include "Views.h"

void View::Setup()
{
  if(true == debugLEDs) Serial << "Setup View\n";
  for(int x = 0; x < SCREEN_WIDTH; ++x)
  {
    for(int y = 0; y < SCREEN_HEIGHT; ++y)
    {
      m_MyPixelStruct.Pixel[x][y] = CRGB::Black;
      if(true == debugLEDs) Serial << "\tR: " << m_MyPixelStruct.Pixel[x][y].red << "\tG: " << m_MyPixelStruct.Pixel[x][y].green << "\tB: " << m_MyPixelStruct.Pixel[x][y].blue << "\n";
    }
  }
  SetupView();
}

void View::MergeSubViews()
{
  for(int v = 0; v < m_SubViews.size(); ++v)
  {
    View *aView = m_SubViews.get(v);
    PixelStruct &aPixelStruct = aView->GetPixelStruct();
    for(int y = 0; y < SCREEN_HEIGHT; ++y)
    {
      for(int x = 0; x < SCREEN_WIDTH; ++x)
      {
        if(true == debugLEDs) Serial << "Pixel Value " << "\tR:" << aPixelStruct.Pixel[x][y].red << "\tG:" << aPixelStruct.Pixel[x][y].green << "\tB:" << aPixelStruct.Pixel[x][y].blue << "\n";
        if(
          aPixelStruct.Pixel[x][y].red != 0 ||
          aPixelStruct.Pixel[x][y].green != 0 ||
          aPixelStruct.Pixel[x][y].blue != 0
          )
          {
            if(true == debugLEDs) Serial << "Set Pixel " << x << "|" << y << " to: " << "\tR:" << aPixelStruct.Pixel[x][y].red << "\tG:" << aPixelStruct.Pixel[x][y].green << "\tB:" << aPixelStruct.Pixel[x][y].blue << "\n";
            m_MyPixelStruct.Pixel[x][y] = aPixelStruct.Pixel[x][y];
          }
      }
    }
  }
}

void VerticalBarView::RunViewTask()
{
  int scaledHeight = (m_Y + round(m_HeightScalar*(float)m_H));
  if(scaledHeight > m_Y + m_H) scaledHeight = m_Y + m_H;
  if(true == debugLEDs) Serial << "Coords: " << m_X << "|" << m_Y << "|" << m_W << "|" << m_H << " Scaled Height: " << scaledHeight << "\n";
  for(int x = 0; x<SCREEN_WIDTH; ++x)
  {
    for(int y = 0; y<SCREEN_HEIGHT; ++y)
    {
        m_MyPixelStruct.Pixel[x][y] = CRGB::Black;
      if( 
          (x >= m_X) && 
          (x < (m_X + m_W)) &&
          (y >= m_Y) && 
          (y < scaledHeight)
        )
      {
        m_MyPixelStruct.Pixel[x][y] = m_Color;
      }
    }
  }
  if(true == debugLEDs) Serial << "\n";
  if(true == debugLEDs) Serial << "************\n";
  for(int y = 0; y < SCREEN_HEIGHT; ++y)
  {
    for(int x = 0; x < SCREEN_WIDTH; ++x)
    {
      CRGB bufColor = m_MyPixelStruct.Pixel[x][y];
      if(true == debugLEDs) Serial << bufColor.red << ":" << bufColor.green << ":" << bufColor.blue << " \t";
    }
    if(true == debugLEDs) Serial << "\n";
  }
}
void ScrollingView::RunViewTask()
{
  switch(m_ScrollDirection)
  {
    case ScrollDirection_Up:
      for(int x = m_X; x < m_X + m_W; ++x)
      {
        for(int y = m_Y + m_H - 2; y >= m_Y; --y)
        {
          m_MyPixelStruct.Pixel[x][y+1] = m_MyPixelStruct.Pixel[x][y];
        }
      }
      break;
    case ScrollDirection_Down:
      for(int x = m_X; x < m_X + m_W; ++x)
      {
        for(int y = m_Y+1; y < m_Y + m_H; ++y)
        {
          m_MyPixelStruct.Pixel[x][y] = m_MyPixelStruct.Pixel[x][y-1];
        }
      }
      break;
    default:
    break;
  }
}
void ColorSpriteView::RunViewTask()
{
  if(true == debugView) Serial << "Coords: " << m_X << "|" << m_Y << "|" << m_W << "|" << m_H << "\n";
  for(int x = 0; x<SCREEN_WIDTH; ++x)
  {
    for(int y = 0; y<SCREEN_HEIGHT; ++y)
    {
        m_MyPixelStruct.Pixel[x][y] = CRGB::Black;
      if( 
          (x >= m_X) && 
          (x < (m_X + m_W)) &&
          (y >= m_Y) && 
          (y < (m_Y + m_H))
        )
      {
        m_MyPixelStruct.Pixel[x][y] = m_MyColor;
      }
    }
  }
}
