/*
    Light Tower by Rob Shockency
    Copyright (C) 2020 Rob Shockency degnarraer@yahoo.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version of the License, or
    (at your option) any later version. 3

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VisualizationFactory_H
#define VisualizatiosnFactory_H

#include "Visualization.h"

//********* VUMeter *********
class VUMeter: public Visualization
{
  public:
    VUMeter( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController) : Visualization( StatisticalEngineModelInterface, LEDController)
    {
      if (true == debugMemory) Serial << "New: VUMeter\n";
    }
    virtual ~VUMeter()
    {
      if (true == debugMemory) Serial << "Deleted: VUMeter\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController);
    void SetupVisualization();
    bool CanRunVisualization();
    void RunVisualization();
  private:
    SoundPowerModel m_SoundPower = SoundPowerModel("Sound Power Model", 2, m_StatisticalEngineModelInterface);
    ReducedBandsBandPowerModel m_BandPower0 = ReducedBandsBandPowerModel("Sound Power Model 0", 0, 0, 3, m_StatisticalEngineModelInterface);
    RandomColorFadingModel m_ColorModel = RandomColorFadingModel("Color Model", 5000);
    VerticalBarView m_VerticalBar = VerticalBarView("Vertical Bar", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MergeType_Add);
    GravitationalModel m_GravitationalModel = GravitationalModel("GravitationalModel0", 0.01, 0.0);
    BassSpriteView m_PeakSprite0 = BassSpriteView("PeakBassSprite", 0, 0, 4, 4, 0, 6, CRGB::Red, CRGB::Green, false, true, MergeType_Layer);
};

//********* SolidColorTower *********
class SolidColorTower: public Visualization
{
  public:
    SolidColorTower( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController) : Visualization( StatisticalEngineModelInterface, LEDController)
    {
      if (true == debugMemory) Serial << "New: SolidColorTower\n";
    }
    virtual ~SolidColorTower()
    {
      if (true == debugMemory) Serial << "Deleted: SolidColorTower\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
    {
      if (true == debugMemory) Serial << "SolidColorTower: Get Instance\n";
      SolidColorTower *vis = new SolidColorTower(StatisticalEngineModelInterface, LEDController);
      return vis;
    }
    void SetupVisualization()
    {
      AddView(m_ColorView0);
      AddModel(m_MaximumBandPowerModel);
      AddModel(m_ColorFadingModel0);
      m_ColorFadingModel0.ConnectBandDataModel(m_MaximumBandPowerModel);
      m_ColorView0.ConnectColorModel(m_ColorFadingModel0);
    }
    bool CanRunVisualization() {
      return true;
    }
    void RunVisualization() {}
  private:
    MaximumBandModel m_MaximumBandPowerModel = MaximumBandModel("Maximum Band Model 0", 10, m_StatisticalEngineModelInterface);
    ColorFadingModel m_ColorFadingModel0 = ColorFadingModel("ColorFadingModel", 1000, 500);
    ColorSpriteView m_ColorView0 = ColorSpriteView("ColorView", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, CRGB::Red, MergeType_Layer);
};

//********* Vertical Bass Sprite Tower *********
class VerticalBassSpriteTower: public Visualization
{
  public:
    VerticalBassSpriteTower( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController) : Visualization( StatisticalEngineModelInterface, LEDController)
    {
      if (true == debugMemory) Serial << "New: SolidColorTower\n";
    }
    virtual ~VerticalBassSpriteTower()
    {
      if (true == debugMemory) Serial << "Deleted: SolidColorTower\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
    {
      if (true == debugMemory) Serial << "VerticalBassSpriteTower: Get Instance\n";
      VerticalBassSpriteTower *vis = new VerticalBassSpriteTower(StatisticalEngineModelInterface, LEDController);
      return vis;
    }
    void SetupVisualization()
    {
      AddView(m_BassSprite0);
      AddView(m_BassSprite1);
      AddView(m_BassSprite2);
      AddView(m_BassSprite3);
      AddView(m_BassSprite4);
      AddView(m_BassSprite5);
      AddView(m_BassSprite6);
      AddView(m_BassSprite7);
      
      AddModel(m_BandPower0);
      AddModel(m_BandPower1);
      AddModel(m_BandPower2);
      AddModel(m_BandPower3);
      AddModel(m_BandPower4);
      AddModel(m_BandPower5);
      AddModel(m_BandPower6);
      AddModel(m_BandPower7);

      AddModel(m_ColorModel0);
      AddModel(m_ColorModel1);
      AddModel(m_ColorModel2);
      AddModel(m_ColorModel3);
      AddModel(m_ColorModel4);
      AddModel(m_ColorModel5);
      AddModel(m_ColorModel6);
      AddModel(m_ColorModel7);
      
      m_BassSprite0.ConnectPowerModel(m_BandPower0);
      m_BassSprite1.ConnectPowerModel(m_BandPower1);
      m_BassSprite2.ConnectPowerModel(m_BandPower2);
      m_BassSprite3.ConnectPowerModel(m_BandPower3);
      m_BassSprite4.ConnectPowerModel(m_BandPower4);
      m_BassSprite5.ConnectPowerModel(m_BandPower5);
      m_BassSprite6.ConnectPowerModel(m_BandPower6);
      m_BassSprite7.ConnectPowerModel(m_BandPower7);
      
      m_BassSprite0.ConnectColorModel(m_ColorModel0);
      m_BassSprite1.ConnectColorModel(m_ColorModel1);
      m_BassSprite2.ConnectColorModel(m_ColorModel2);
      m_BassSprite3.ConnectColorModel(m_ColorModel3);
      m_BassSprite4.ConnectColorModel(m_ColorModel4);
      m_BassSprite5.ConnectColorModel(m_ColorModel5);
      m_BassSprite6.ConnectColorModel(m_ColorModel6);
      m_BassSprite7.ConnectColorModel(m_ColorModel7);
    }
    bool CanRunVisualization() {
      return true;
    }
    void RunVisualization() {}
  private:
    ReducedBandsBandPowerModel m_BandPower0 = ReducedBandsBandPowerModel("Sound Power Model 0", 0, 0, 8, m_StatisticalEngineModelInterface);
    ReducedBandsBandPowerModel m_BandPower1 = ReducedBandsBandPowerModel("Sound Power Model 1", 1, 0, 8, m_StatisticalEngineModelInterface);
    ReducedBandsBandPowerModel m_BandPower2 = ReducedBandsBandPowerModel("Sound Power Model 2", 2, 0, 8, m_StatisticalEngineModelInterface);
    ReducedBandsBandPowerModel m_BandPower3 = ReducedBandsBandPowerModel("Sound Power Model 3", 3, 0, 8, m_StatisticalEngineModelInterface);
    ReducedBandsBandPowerModel m_BandPower4 = ReducedBandsBandPowerModel("Sound Power Model 4", 4, 0, 8, m_StatisticalEngineModelInterface);
    ReducedBandsBandPowerModel m_BandPower5 = ReducedBandsBandPowerModel("Sound Power Model 5", 5, 0, 8, m_StatisticalEngineModelInterface);
    ReducedBandsBandPowerModel m_BandPower6 = ReducedBandsBandPowerModel("Sound Power Model 6", 6, 0, 8, m_StatisticalEngineModelInterface);
    ReducedBandsBandPowerModel m_BandPower7 = ReducedBandsBandPowerModel("Sound Power Model 7", 7, 0, 8, m_StatisticalEngineModelInterface);
    BassSpriteView m_BassSprite0 = BassSpriteView("PeakBassSprite0", 0, 1*SCREEN_HEIGHT/9, 4, 4, 0, SCREEN_HEIGHT/10, CRGB::Red, (CRGB){20,20,20}, false, true, MergeType_Add);
    BassSpriteView m_BassSprite1 = BassSpriteView("PeakBassSprite1", 0, 2*SCREEN_HEIGHT/9, 4, 4, 0, SCREEN_HEIGHT/10, CRGB::Red, (CRGB){20,20,20}, false, true, MergeType_Add);
    BassSpriteView m_BassSprite2 = BassSpriteView("PeakBassSprite2", 0, 3*SCREEN_HEIGHT/9, 4, 4, 0, SCREEN_HEIGHT/10, CRGB::Red, (CRGB){20,20,20}, false, true, MergeType_Add);
    BassSpriteView m_BassSprite3 = BassSpriteView("PeakBassSprite3", 0, 4*SCREEN_HEIGHT/9, 4, 4, 0, SCREEN_HEIGHT/10, CRGB::Red, (CRGB){20,20,20}, false, true, MergeType_Add);
    BassSpriteView m_BassSprite4 = BassSpriteView("PeakBassSprite4", 0, 5*SCREEN_HEIGHT/9, 4, 4, 0, SCREEN_HEIGHT/10, CRGB::Red, (CRGB){20,20,20}, false, true, MergeType_Add);
    BassSpriteView m_BassSprite5 = BassSpriteView("PeakBassSprite5", 0, 6*SCREEN_HEIGHT/9, 4, 4, 0, SCREEN_HEIGHT/10, CRGB::Red, (CRGB){20,20,20}, false, true, MergeType_Add);
    BassSpriteView m_BassSprite6 = BassSpriteView("PeakBassSprite6", 0, 7*SCREEN_HEIGHT/9, 4, 4, 0, SCREEN_HEIGHT/10, CRGB::Red, (CRGB){20,20,20}, false, true, MergeType_Add);
    BassSpriteView m_BassSprite7 = BassSpriteView("PeakBassSprite7", 0, 8*SCREEN_HEIGHT/9, 4, 4, 0, SCREEN_HEIGHT/10, CRGB::Red, (CRGB){20,20,20}, false, true, MergeType_Add);
    RainbowColorModel m_ColorModel0 = RainbowColorModel("Color Model 1", 0, 8);
    RainbowColorModel m_ColorModel1 = RainbowColorModel("Color Model 1", 1, 8);
    RainbowColorModel m_ColorModel2 = RainbowColorModel("Color Model 1", 2, 8);
    RainbowColorModel m_ColorModel3 = RainbowColorModel("Color Model 1", 3, 8);
    RainbowColorModel m_ColorModel4 = RainbowColorModel("Color Model 1", 4, 8);
    RainbowColorModel m_ColorModel5 = RainbowColorModel("Color Model 1", 5, 8);
    RainbowColorModel m_ColorModel6 = RainbowColorModel("Color Model 1", 6, 8);
    RainbowColorModel m_ColorModel7 = RainbowColorModel("Color Model 1", 7, 8);
};

//********* 8 Band VUMeter *********
class VUMeter8Band: public Visualization
{
  public:
    VUMeter8Band( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
      : Visualization( StatisticalEngineModelInterface, LEDController)
    {
      if (true == debugMemory) Serial << "New: VUMeter8Band\n";
    }
    virtual ~VUMeter8Band()
    {
      if (true == debugMemory) Serial << "Deleted: VUMeter8Band\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController);
    void SetupVisualization();
    bool CanRunVisualization();
    void RunVisualization();
  private:
    float numVisualizations = 8.0;
    VerticalBarView m_VerticalBar0 = VerticalBarView("Vertical Bar 0", 0, 0 * SCREEN_HEIGHT / numVisualizations, SCREEN_WIDTH, SCREEN_HEIGHT / numVisualizations, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower0 = ReducedBandsBandPowerModel("Sound Power Model 0", 0, 5, numVisualizations, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel0 = RainbowColorModel("Color Model 0", 0, numVisualizations);
    GravitationalModel m_GravitationalModel0 = GravitationalModel("GravitationalModel0", 0.01, 0.0);
    ColorSpriteView m_PeakSprite0 = ColorSpriteView("PeakSprite0", 0, 0, SCREEN_WIDTH, 1, CRGB::Red, MergeType_Add);

    VerticalBarView m_VerticalBar1 = VerticalBarView("Vertical Bar 1", 0, 1 * SCREEN_HEIGHT / numVisualizations, SCREEN_WIDTH, SCREEN_HEIGHT / numVisualizations, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower1 = ReducedBandsBandPowerModel("Sound Power Model 1", 1, 5, numVisualizations, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel1 = RainbowColorModel("Color Model 1", 1, numVisualizations);
    GravitationalModel m_GravitationalModel1 = GravitationalModel("GravitationalModel1", 0.01, 0.0);
    ColorSpriteView m_PeakSprite1 = ColorSpriteView("PeakSprite1", 0, 0, SCREEN_WIDTH, 1, CRGB::Red, MergeType_Add);

    VerticalBarView m_VerticalBar2 = VerticalBarView("Vertical Bar 2", 0, 2 * SCREEN_HEIGHT / numVisualizations, SCREEN_WIDTH, SCREEN_HEIGHT / numVisualizations, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower2 = ReducedBandsBandPowerModel("Sound Power Model 2", 2, 5, numVisualizations, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel2 = RainbowColorModel("Color Model 2", 2, numVisualizations);
    GravitationalModel m_GravitationalModel2 = GravitationalModel("GravitationalModel2", 0.01, 0.0);
    ColorSpriteView m_PeakSprite2 = ColorSpriteView("PeakSprite2", 0, 0, SCREEN_WIDTH, 1, CRGB::Red, MergeType_Add);

    VerticalBarView m_VerticalBar3 = VerticalBarView("Vertical Bar 3", 0, 3 * SCREEN_HEIGHT / numVisualizations, SCREEN_WIDTH, SCREEN_HEIGHT / numVisualizations, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower3 = ReducedBandsBandPowerModel("Sound Power Model 3", 3, 5, numVisualizations, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel3 = RainbowColorModel("Color Model 3", 3, numVisualizations);
    GravitationalModel m_GravitationalModel3 = GravitationalModel("GravitationalModel3", 0.01, 0.0);
    ColorSpriteView m_PeakSprite3 = ColorSpriteView("PeakSprite3", 0, 0, SCREEN_WIDTH, 1, CRGB::Red, MergeType_Add);

    VerticalBarView m_VerticalBar4 = VerticalBarView("Vertical Bar 4", 0, 4 * SCREEN_HEIGHT / numVisualizations, SCREEN_WIDTH, SCREEN_HEIGHT / numVisualizations, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower4 = ReducedBandsBandPowerModel("Sound Power Model 4", 4, 5, numVisualizations, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel4 = RainbowColorModel("Color Model 4", 4, numVisualizations);
    GravitationalModel m_GravitationalModel4 = GravitationalModel("GravitationalModel2", 0.01, 0.0);
    ColorSpriteView m_PeakSprite4 = ColorSpriteView("PeakSprite4", 0, 0, SCREEN_WIDTH, 1, CRGB::Red, MergeType_Add);

    VerticalBarView m_VerticalBar5 = VerticalBarView("Vertical Bar 5", 0, 5 * SCREEN_HEIGHT / numVisualizations, SCREEN_WIDTH, SCREEN_HEIGHT / numVisualizations, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower5 = ReducedBandsBandPowerModel("Sound Power Model 5", 5, 5, numVisualizations, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel5 = RainbowColorModel("Color Model 5", 5, numVisualizations);
    GravitationalModel m_GravitationalModel5 = GravitationalModel("GravitationalModel5", 0.01, 0.0);
    ColorSpriteView m_PeakSprite5 = ColorSpriteView("PeakSprite5", 0, 0, SCREEN_WIDTH, 1, CRGB::Red, MergeType_Add);

    VerticalBarView m_VerticalBar6 = VerticalBarView("Vertical Bar 6", 0, 6 * SCREEN_HEIGHT / numVisualizations, SCREEN_WIDTH, SCREEN_HEIGHT / numVisualizations, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower6 = ReducedBandsBandPowerModel("Sound Power Model 6", 6, 5, numVisualizations, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel6 = RainbowColorModel("Color Model 6", 6, numVisualizations);
    GravitationalModel m_GravitationalModel6 = GravitationalModel("GravitationalModel6", 0.01, 0.0);
    ColorSpriteView m_PeakSprite6 = ColorSpriteView("PeakSprite6", 0, 0, SCREEN_WIDTH, 1, CRGB::Red, MergeType_Add);

    VerticalBarView m_VerticalBar7 = VerticalBarView("Vertical Bar 7", 0, 7 * SCREEN_HEIGHT / numVisualizations, SCREEN_WIDTH, SCREEN_HEIGHT / numVisualizations, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower7 = ReducedBandsBandPowerModel("Sound Power Model 7", 7, 5, numVisualizations, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel7 = RainbowColorModel("Color Model 7", 7, numVisualizations);
    GravitationalModel m_GravitationalModel7 = GravitationalModel("GravitationalModel7", 0.01, 0.0);
    ColorSpriteView m_PeakSprite7 = ColorSpriteView("PeakSprite7", 0, 0, SCREEN_WIDTH, 1, CRGB::Red, MergeType_Add);
};

//********* 3 Band VUMeter *********
class VUMeter3Band: public Visualization
{
  public:
    VUMeter3Band( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
      : Visualization( StatisticalEngineModelInterface, LEDController)
    {
      if (true == debugMemory) Serial << "New: VUMeter3Band\n";
    }
    virtual ~VUMeter3Band()
    {
      if (true == debugMemory) Serial << "Deleted: VUMeter3Band\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController);
    void SetupVisualization();
    bool CanRunVisualization();
    void RunVisualization();
  private:
    float numVisualizations = 3.0;
    VerticalBarView m_VerticalBar0 = VerticalBarView("Vertical Bar 0", 0, 0 * SCREEN_HEIGHT / numVisualizations, SCREEN_WIDTH, SCREEN_HEIGHT / numVisualizations, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower0 = ReducedBandsBandPowerModel("Sound Power Model 0", 0, 5, 3, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel0 = RainbowColorModel("Color Model 0", 0, numVisualizations);
    GravitationalModel m_GravitationalModel0 = GravitationalModel("GravitationalModel0", 0.01, 0.0);
    ColorSpriteView m_PeakSprite0 = ColorSpriteView("PeakSprite0", 0, 0, SCREEN_WIDTH, 1, CRGB::Red, MergeType_Add);

    VerticalBarView m_VerticalBar1 = VerticalBarView("Vertical Bar 1", 0, 1 * SCREEN_HEIGHT / numVisualizations, SCREEN_WIDTH, SCREEN_HEIGHT / numVisualizations, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower1 = ReducedBandsBandPowerModel("Sound Power Model 1", 1, 5, 3, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel1 = RainbowColorModel("Color Model 1", 1, numVisualizations);
    GravitationalModel m_GravitationalModel1 = GravitationalModel("GravitationalModel1", 0.01, 0.0);
    ColorSpriteView m_PeakSprite1 = ColorSpriteView("PeakSprite1", 0, 0, SCREEN_WIDTH, 1, CRGB::Red, MergeType_Add);

    VerticalBarView m_VerticalBar2 = VerticalBarView("Vertical Bar 2", 0, 2 * SCREEN_HEIGHT / numVisualizations, SCREEN_WIDTH, SCREEN_HEIGHT / numVisualizations, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower2 = ReducedBandsBandPowerModel("Sound Power Model 2", 2, 5, 3, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel2 = RainbowColorModel("Color Model 2", 2, numVisualizations);
    GravitationalModel m_GravitationalModel2 = GravitationalModel("GravitationalModel2", 0.01, 0.0);
    ColorSpriteView m_PeakSprite2 = ColorSpriteView("PeakSprite2", 0, 0, SCREEN_WIDTH, 1, CRGB::Red, MergeType_Add);
};

//********* Waterfall *********
class Waterfall: public Visualization
{
  public:
    Waterfall( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
      : Visualization( StatisticalEngineModelInterface, LEDController) {
      if (true == debugMemory) Serial << "New: Waterfall\n";
    }
    virtual ~Waterfall() {
      if (true == debugMemory) Serial << "Deleted: Waterfall\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController);
    void SetupVisualization();
    bool CanRunVisualization();
    void RunVisualization();
  private:
    ScrollingView m_ScrollingView = ScrollingView("Scrolling View", ScrollDirection_Down, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    ColorSpriteView m_Sprite0 = ColorSpriteView("Sprite", 0, SCREEN_HEIGHT - 1, 4, 1);
    SoundPowerModel m_PowerModel = SoundPowerModel("Sound Power Model", 0, m_StatisticalEngineModelInterface);
    RandomColorFadingModel m_ColorModel = RandomColorFadingModel("Color Fading Model", 10000);
    SettableColorPowerModel m_PowerColorModel = SettableColorPowerModel("Settable Power Model");
};

//********* Fire *********
class Fire: public Visualization
{
  public:
    Fire( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
      : Visualization( StatisticalEngineModelInterface, LEDController) {
      if (true == debugMemory) Serial << "New: Fire\n";
    }
    virtual ~Fire() {
      if (true == debugMemory) Serial << "Deleted: Fire\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController);
    void SetupVisualization();
    bool CanRunVisualization();
    void RunVisualization();
  private:
    ScrollingView m_ScrollingView = ScrollingView("Scrolling View", ScrollDirection_Up, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    ColorSpriteView m_Sprite0 = ColorSpriteView("Sprite", 0, 0, 4, 1);
    SoundPowerModel m_PowerModel = SoundPowerModel("Sound Power Model", 0, m_StatisticalEngineModelInterface);
    RandomColorFadingModel m_ColorModel = RandomColorFadingModel("Color Fading Model", 10000);
    SettableColorPowerModel m_PowerColorModel = SettableColorPowerModel("Settable Power Model");
};

//********* WaterFireFromCenter *********
class WaterFireFromCenter: public Visualization
{
  public:
    WaterFireFromCenter( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
      : Visualization( StatisticalEngineModelInterface, LEDController) {
      if (true == debugMemory) Serial << "New: WaterFireFromCenter\n";
    }
    virtual ~WaterFireFromCenter() {
      if (true == debugMemory) Serial << "Deleted: WaterFireFromCenter\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController);
    void SetupVisualization();
    bool CanRunVisualization();
    void RunVisualization();
  private:
    ScrollingView m_ScrollingView0 = ScrollingView("Fire Scrolling View", ScrollDirection_Up, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
    ColorSpriteView m_Sprite0 = ColorSpriteView("Fire Sprite 0", 0, SCREEN_HEIGHT / 2, 4, 1);
    SoundPowerModel m_PowerModel0 = SoundPowerModel("Sound Power Model", 0, m_StatisticalEngineModelInterface);
    RandomColorFadingModel m_ColorModel0 = RandomColorFadingModel("Color Fading Model", 10000);
    SettableColorPowerModel m_PowerColorModel0 = SettableColorPowerModel("Settable Power Model");

    ScrollingView m_ScrollingView1 = ScrollingView("Water Scrolling View", ScrollDirection_Down, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
    ColorSpriteView m_Sprite1 = ColorSpriteView("Water Sprite 0", 0, SCREEN_HEIGHT / 2 - 1, 4, 1);
    SoundPowerModel m_PowerModel1 = SoundPowerModel("Sound Power Model", 0, m_StatisticalEngineModelInterface);
    RandomColorFadingModel m_ColorModel1 = RandomColorFadingModel("Color Fading Model", 10000);
    SettableColorPowerModel m_PowerColorModel1 = SettableColorPowerModel("Settable Power Model");
};

//********* WaterFireFromEdge *********
class WaterFireFromEdge: public Visualization
{
  public:
    WaterFireFromEdge( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
      : Visualization( StatisticalEngineModelInterface, LEDController) {
      if (true == debugMemory) Serial << "New: WaterFireFromEdge\n";
    }
    virtual ~WaterFireFromEdge() {
      if (true == debugMemory) Serial << "Deleted: WaterFireFromEdge\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController);
    void SetupVisualization();
    bool CanRunVisualization();
    void RunVisualization();
  private:
    ScrollingView m_ScrollingView0 = ScrollingView("Fire Scrolling View", ScrollDirection_Up, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    FadingView m_FadingView0 = FadingView("FadingView 0", SCREEN_HEIGHT, Direction_Up, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MergeType_Add);
    ColorSpriteView m_Sprite0 = ColorSpriteView("Fire Sprite 0", 0, 0, 4, 1);
    SoundPowerModel m_PowerModel0 = SoundPowerModel("Sound Power Model", 0, m_StatisticalEngineModelInterface);
    RandomColorFadingModel m_ColorModel0 = RandomColorFadingModel("Color Fading Model", 10000);
    SettableColorPowerModel m_PowerColorModel0 = SettableColorPowerModel("Settable Power Model");

    ScrollingView m_ScrollingView1 = ScrollingView("Water Scrolling View", ScrollDirection_Down, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    FadingView m_FadingView1 = FadingView("FadingView 1", SCREEN_HEIGHT, Direction_Down, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MergeType_Add);
    ColorSpriteView m_Sprite1 = ColorSpriteView("Water Sprite 0", 0, SCREEN_HEIGHT - 1, 4, 1);
    SoundPowerModel m_PowerModel1 = SoundPowerModel("Sound Power Model", 0, m_StatisticalEngineModelInterface);
    RandomColorFadingModel m_ColorModel1 = RandomColorFadingModel("Color Fading Model", 10000);
    SettableColorPowerModel m_PowerColorModel1 = SettableColorPowerModel("Settable Power Model");
};

//********* Vertical Band Tower *********
class VerticalBandTower: public Visualization
{
  public:
    VerticalBandTower( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
      : Visualization( StatisticalEngineModelInterface, LEDController) {
      if (true == debugMemory) Serial << "New: VerticalBandTower\n";
    }
    virtual ~VerticalBandTower() {
      if (true == debugMemory) Serial << "Deleted: VerticalBandTower\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController);
    void SetupVisualization();
    bool CanRunVisualization();
    void RunVisualization();
  private:
};

//********* scrolling Bands *********
class ScrollingBands: public Visualization
{
  public:
    ScrollingBands( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
      : Visualization( StatisticalEngineModelInterface, LEDController) {
      if (true == debugMemory) Serial << "New: ScrollingBands\n";
    }
    virtual ~ScrollingBands() {
      if (true == debugMemory) Serial << "Deleted: ScrollingBands\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController);
    void SetupVisualization();
    bool CanRunVisualization();
    void RunVisualization();
  private:

    RotatingView m_RotateView0 = RotatingView("Rotating View 0", Direction_Right, 5000, RotationType_Static, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MergeType_Layer);
    ScrollingView m_ScrollingView0 = ScrollingView("Upward Scrolling View", ScrollDirection_Up, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    FadingView m_FadingView0 = FadingView("FadingView 0", SCREEN_HEIGHT, Direction_Up, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MergeType_Add);

    RotatingView m_RotateView1 = RotatingView("Rotating View 1", Direction_Left, 5000, RotationType_Static, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MergeType_Layer);
    ScrollingView m_ScrollingView1 = ScrollingView("Downward Scrolling View", ScrollDirection_Down, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    FadingView m_FadingView1 = FadingView("FadingView 1", SCREEN_HEIGHT, Direction_Down, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MergeType_Add);

    ColorSpriteView m_Sprite0 = ColorSpriteView("Sprite 0", 0, 0, 1, 1);
    ReducedBandsBandPowerModel m_BandPower0 = ReducedBandsBandPowerModel("Sound Power Model 0", 0, 0, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel0 = RainbowColorModel("Color Model 1", 0, 8);
    SettableColorPowerModel m_PowerColorModel0 = SettableColorPowerModel("Settable Power Model");

    ColorSpriteView m_Sprite1 = ColorSpriteView("Sprite 0", 1, 0, 1, 1);
    ReducedBandsBandPowerModel m_BandPower1 = ReducedBandsBandPowerModel("Sound Power Model 1", 1, 0, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel1 = RainbowColorModel("Color Model 1", 1, 8);
    SettableColorPowerModel m_PowerColorModel1 = SettableColorPowerModel("Settable Power Model");

    ColorSpriteView m_Sprite2 = ColorSpriteView("Sprite 0", 2, 0, 1, 1);
    ReducedBandsBandPowerModel m_BandPower2 = ReducedBandsBandPowerModel("Sound Power Model 2", 2, 0, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel2 = RainbowColorModel("Color Model 1", 2, 8);
    SettableColorPowerModel m_PowerColorModel2 = SettableColorPowerModel("Settable Power Model");

    ColorSpriteView m_Sprite3 = ColorSpriteView("Sprite 0", 3, 0, 1, 1);
    ReducedBandsBandPowerModel m_BandPower3 = ReducedBandsBandPowerModel("Sound Power Model 3", 3, 0, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel3 = RainbowColorModel("Color Model 1", 3, 8);
    SettableColorPowerModel m_PowerColorModel3 = SettableColorPowerModel("Settable Power Model");


    ColorSpriteView m_Sprite4 = ColorSpriteView("Sprite 0", 0, SCREEN_HEIGHT - 1, 1, 1);
    ReducedBandsBandPowerModel m_BandPower4 = ReducedBandsBandPowerModel("Sound Power Model 0", 4, 0, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel4 = RainbowColorModel("Color Model 1", 4, 8);
    SettableColorPowerModel m_PowerColorModel4 = SettableColorPowerModel("Settable Power Model");

    ColorSpriteView m_Sprite5 = ColorSpriteView("Sprite 0", 1, SCREEN_HEIGHT - 1, 1, 1);
    ReducedBandsBandPowerModel m_BandPower5 = ReducedBandsBandPowerModel("Sound Power Model 1", 5, 0, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel5 = RainbowColorModel("Color Model 1", 5, 8);
    SettableColorPowerModel m_PowerColorModel5 = SettableColorPowerModel("Settable Power Model");

    ColorSpriteView m_Sprite6 = ColorSpriteView("Sprite 0", 2, SCREEN_HEIGHT - 1, 1, 1);
    ReducedBandsBandPowerModel m_BandPower6 = ReducedBandsBandPowerModel("Sound Power Model 2", 6, 0, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel6 = RainbowColorModel("Color Model 1", 6, 8);
    SettableColorPowerModel m_PowerColorModel6 = SettableColorPowerModel("Settable Power Model");

    ColorSpriteView m_Sprite7 = ColorSpriteView("Sprite 0", 3, SCREEN_HEIGHT - 1, 1, 1);
    ReducedBandsBandPowerModel m_BandPower7 = ReducedBandsBandPowerModel("Sound Power Model 3", 7, 0, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel7 = RainbowColorModel("Color Model 1", 7, 8);
    SettableColorPowerModel m_PowerColorModel7 = SettableColorPowerModel("Settable Power Model");
};

//********* Scrolling Max Band *********
class ScrollingMaxBand: public Visualization
{
  public:
    ScrollingMaxBand( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
      : Visualization( StatisticalEngineModelInterface, LEDController) {
      if (true == debugMemory) Serial << "New: ScrollingMaxBand\n";
    }
    virtual ~ScrollingMaxBand() {
      if (true == debugMemory) Serial << "Deleted: ScrollingMaxBand\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController);
    void SetupVisualization();
    bool CanRunVisualization();
    void RunVisualization();
  private:
    ScrollingView m_ScrollingView = ScrollingView("Scrolling View", ScrollDirection_Up, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    ColorSpriteView m_Sprite0 = ColorSpriteView("Sprite", 0, 0, 4, 1);
    BandDataColorModel m_BandDataColorModel = BandDataColorModel( "Band Data Color Model" );
    MaximumBandModel m_MaxBandModel = MaximumBandModel( "Max Band Model", 0, m_StatisticalEngineModelInterface );
};

//********* Rotating View *********
class RotatingSprites: public Visualization
{
  public:
    RotatingSprites( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
      : Visualization( StatisticalEngineModelInterface, LEDController) {
      if (true == debugMemory) Serial << "New: RotatingSprites\n";
    }
    virtual ~RotatingSprites() {
      if (true == debugMemory) Serial << "Deleted: RotatingSprites\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
    {
      if (true == debugMemory) Serial << "RotatingSprites: Get Instance\n";
      RotatingSprites *vis = new RotatingSprites(StatisticalEngineModelInterface, LEDController);
      return vis;
    }
    void SetupVisualization()
    {

      AddView(m_RotateView0);
      AddModel(m_ColorModel0);
      m_Sprite0.ConnectColorModel(m_ColorModel0);
      m_RotateView0.AddSubView(m_Sprite0);

      /*
      AddView(m_RotateView1);
      m_RotateView1.AddSubView(m_RotateView0);

      m_RotateView0.AddSubView(m_Sprite0);
      m_RotateView0.AddSubView(m_Sprite1);
      m_RotateView0.AddSubView(m_Sprite2);
      m_RotateView0.AddSubView(m_Sprite3);

      AddModel(m_PowerModel0);

      AddModel(m_ColorModel0);
      AddModel(m_ColorModel1);
      AddModel(m_ColorModel2);
      AddModel(m_ColorModel3);

      AddModel(m_PowerColorModel0);
      AddModel(m_PowerColorModel1);
      AddModel(m_PowerColorModel2);
      AddModel(m_PowerColorModel3);

      m_PowerColorModel0.ConnectColorModel(m_ColorModel0);
      m_PowerColorModel1.ConnectColorModel(m_ColorModel1);
      m_PowerColorModel2.ConnectColorModel(m_ColorModel2);
      m_PowerColorModel3.ConnectColorModel(m_ColorModel3);

      m_PowerColorModel0.ConnectPowerModel(m_PowerModel0);
      m_PowerColorModel1.ConnectPowerModel(m_PowerModel0);
      m_PowerColorModel2.ConnectPowerModel(m_PowerModel0);
      m_PowerColorModel3.ConnectPowerModel(m_PowerModel0);

      m_Sprite0.ConnectColorModel(m_ColorModel0);
      m_Sprite1.ConnectColorModel(m_ColorModel1);
      m_Sprite2.ConnectColorModel(m_ColorModel2);
      m_Sprite3.ConnectColorModel(m_ColorModel3);
      */
    }
    bool CanRunVisualization() {
      return true;
    }
    void RunVisualization() {}
  private:
    SoundPowerModel m_PowerModel0 = SoundPowerModel("Sound Power Model", 0, m_StatisticalEngineModelInterface);
    RandomColorFadingModel m_ColorModel0 = RandomColorFadingModel("Color Fading Model 0", 1000);
    RandomColorFadingModel m_ColorModel1 = RandomColorFadingModel("Color Fading Model 1", 1000);
    RandomColorFadingModel m_ColorModel2 = RandomColorFadingModel("Color Fading Model 2", 1000);
    RandomColorFadingModel m_ColorModel3 = RandomColorFadingModel("Color Fading Model 3", 1000);
    SettableColorPowerModel m_PowerColorModel0 = SettableColorPowerModel("Settable Power Model 0");
    SettableColorPowerModel m_PowerColorModel1 = SettableColorPowerModel("Settable Power Model 1");
    SettableColorPowerModel m_PowerColorModel2 = SettableColorPowerModel("Settable Power Model 2");
    SettableColorPowerModel m_PowerColorModel3 = SettableColorPowerModel("Settable Power Model 3");
    ColorSpriteView m_Sprite0 = ColorSpriteView("Sprite 0", 0, SCREEN_HEIGHT / 2, 1, 1, MergeType_Layer);
    ColorSpriteView m_Sprite1 = ColorSpriteView("Sprite 1", 1, SCREEN_HEIGHT / 2, 1, 1, MergeType_Layer);
    ColorSpriteView m_Sprite2 = ColorSpriteView("Sprite 2", 2, SCREEN_HEIGHT / 2, 1, 1, MergeType_Layer);
    ColorSpriteView m_Sprite3 = ColorSpriteView("Sprite 3", 3, SCREEN_HEIGHT / 2, 1, 1, MergeType_Layer);
    RotatingView m_RotateView0 = RotatingView("Rotating View 0", Direction_Down, 100, RotationType_Scroll, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MergeType_Layer);
    RotatingView m_RotateView1 = RotatingView("Rotating View 1", Direction_Right, 100, RotationType_Static, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MergeType_Layer);
};

//********* Ball Shooter *********
class BallShooter: public Visualization
{
  public:
    BallShooter( StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
      : Visualization( StatisticalEngineModelInterface, LEDController) {
      if (true == debugMemory) Serial << "New: BallShooter\n";
    }
    virtual ~BallShooter() {
      if (true == debugMemory) Serial << "Deleted: BallShooter\n";
    }

    //Visualization
    static Visualization* GetInstance(StatisticalEngineModelInterface &StatisticalEngineModelInterface, LEDController &LEDController)
    {
      if (true == debugMemory) Serial << "BallShooter: Get Instance\n";
      BallShooter *vis = new BallShooter(StatisticalEngineModelInterface, LEDController);
      return vis;
    }
    void SetupVisualization()
    {
      AddView(m_SubView1);
      AddView(m_SubView0);

      m_SubView0.AddSubView(m_PeakSprite0);
      m_SubView0.AddSubView(m_PeakSprite1);
      m_SubView0.AddSubView(m_PeakSprite2);
      m_SubView0.AddSubView(m_PeakSprite3);
      m_SubView0.AddSubView(m_PeakSprite4);
      m_SubView0.AddSubView(m_PeakSprite5);
      m_SubView0.AddSubView(m_PeakSprite6);
      m_SubView0.AddSubView(m_PeakSprite7);

      m_SubView1.AddSubView(m_VerticalBar0);
      m_SubView1.AddSubView(m_VerticalBar1);
      m_SubView1.AddSubView(m_VerticalBar2);
      m_SubView1.AddSubView(m_VerticalBar3);
      m_SubView1.AddSubView(m_VerticalBar4);
      m_SubView1.AddSubView(m_VerticalBar5);
      m_SubView1.AddSubView(m_VerticalBar6);
      m_SubView1.AddSubView(m_VerticalBar7);

      AddModel(m_BandPower0);
      AddModel(m_ColorModel0);
      AddModel(m_VerticalBar0);
      AddModel(m_GravitationalModel0);
      m_VerticalBar0.ConnectBarHeightModel(m_BandPower0);
      m_VerticalBar0.ConnectBarColorModel(m_ColorModel0);
      m_PeakSprite0.ConnectYPositionModel(m_GravitationalModel0);
      m_GravitationalModel0.ConnectPositionModel(m_VerticalBar0);
      m_PeakSprite0.ConnectColorModel(m_ColorModel0);

      AddModel(m_BandPower1);
      AddModel(m_ColorModel1);
      AddModel(m_VerticalBar1);
      AddModel(m_GravitationalModel1);
      m_VerticalBar1.ConnectBarHeightModel(m_BandPower1);
      m_VerticalBar1.ConnectBarColorModel(m_ColorModel1);
      m_PeakSprite1.ConnectYPositionModel(m_GravitationalModel1);
      m_GravitationalModel1.ConnectPositionModel(m_VerticalBar1);
      m_PeakSprite1.ConnectColorModel(m_ColorModel1);

      AddModel(m_BandPower2);
      AddModel(m_ColorModel2);
      AddModel(m_VerticalBar2);
      AddModel(m_GravitationalModel2);
      m_VerticalBar2.ConnectBarHeightModel(m_BandPower2);
      m_VerticalBar2.ConnectBarColorModel(m_ColorModel2);
      m_PeakSprite2.ConnectYPositionModel(m_GravitationalModel2);
      m_GravitationalModel2.ConnectPositionModel(m_VerticalBar2);
      m_PeakSprite2.ConnectColorModel(m_ColorModel2);

      AddModel(m_BandPower3);
      AddModel(m_ColorModel3);
      AddModel(m_VerticalBar3);
      AddModel(m_GravitationalModel3);
      m_VerticalBar3.ConnectBarHeightModel(m_BandPower3);
      m_VerticalBar3.ConnectBarColorModel(m_ColorModel3);
      m_PeakSprite3.ConnectYPositionModel(m_GravitationalModel3);
      m_GravitationalModel3.ConnectPositionModel(m_VerticalBar3);
      m_PeakSprite3.ConnectColorModel(m_ColorModel3);

      AddModel(m_BandPower4);
      AddModel(m_ColorModel4);
      AddModel(m_VerticalBar4);
      AddModel(m_GravitationalModel4);
      m_VerticalBar4.ConnectBarHeightModel(m_BandPower4);
      m_VerticalBar4.ConnectBarColorModel(m_ColorModel4);
      m_PeakSprite4.ConnectYPositionModel(m_GravitationalModel4);
      m_GravitationalModel4.ConnectPositionModel(m_VerticalBar4);
      m_PeakSprite4.ConnectColorModel(m_ColorModel4);

      AddModel(m_BandPower5);
      AddModel(m_ColorModel5);
      AddModel(m_VerticalBar5);
      AddModel(m_GravitationalModel5);
      m_VerticalBar5.ConnectBarHeightModel(m_BandPower5);
      m_VerticalBar5.ConnectBarColorModel(m_ColorModel5);
      m_PeakSprite5.ConnectYPositionModel(m_GravitationalModel5);
      m_GravitationalModel5.ConnectPositionModel(m_VerticalBar5);
      m_PeakSprite5.ConnectColorModel(m_ColorModel5);

      AddModel(m_BandPower6);
      AddModel(m_ColorModel6);
      AddModel(m_VerticalBar6);
      AddModel(m_GravitationalModel6);
      m_VerticalBar6.ConnectBarHeightModel(m_BandPower6);
      m_VerticalBar6.ConnectBarColorModel(m_ColorModel6);
      m_PeakSprite6.ConnectYPositionModel(m_GravitationalModel6);
      m_GravitationalModel6.ConnectPositionModel(m_VerticalBar6);
      m_PeakSprite6.ConnectColorModel(m_ColorModel6);

      AddModel(m_BandPower7);
      AddModel(m_ColorModel7);
      AddModel(m_VerticalBar7);
      AddModel(m_GravitationalModel7);
      m_VerticalBar7.ConnectBarHeightModel(m_BandPower7);
      m_VerticalBar7.ConnectBarColorModel(m_ColorModel7);
      m_PeakSprite7.ConnectYPositionModel(m_GravitationalModel7);
      m_PeakSprite7.ConnectColorModel(m_ColorModel7);
      m_GravitationalModel7.ConnectPositionModel(m_VerticalBar7);
    }
    bool CanRunVisualization() {
      return true;
    }
    void RunVisualization() {}
  private:
    SubView m_SubView0 = SubView("SubView0", true, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MergeType_Layer);
    SubView m_SubView1 = SubView("SubView1", true, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MergeType_Layer);
    
    VerticalBarView m_VerticalBar0 = VerticalBarView("Vertical Bar 0", 0, 0, 1, SCREEN_HEIGHT / 2, MergeType_Add);
    ColorSpriteView m_PeakSprite0 = ColorSpriteView("PeakSprite0", 0, 0, SCREEN_WIDTH, 2, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower0 = ReducedBandsBandPowerModel("Sound Power Model 0", 0, 5, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel0 = RainbowColorModel("Color Model 0", 0, 8);
    GravitationalModel m_GravitationalModel0 = GravitationalModel("GravitationalModel0", 1.0, 10.0);

    VerticalBarView m_VerticalBar1 = VerticalBarView("Vertical Bar 1", 0, 0, 1, SCREEN_HEIGHT / 2, MergeType_Add);
    ColorSpriteView m_PeakSprite1 = ColorSpriteView("PeakSprite1", 0, 0, SCREEN_WIDTH, 2, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower1 = ReducedBandsBandPowerModel("Sound Power Model 1", 1, 5, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel1 = RainbowColorModel("Color Model 1", 1, 8);
    GravitationalModel m_GravitationalModel1 = GravitationalModel("GravitationalModel1", 1.0, 10.0);

    VerticalBarView m_VerticalBar2 = VerticalBarView("Vertical Bar 2", 1, 0, 1, SCREEN_HEIGHT / 2, MergeType_Add);
    ColorSpriteView m_PeakSprite2 = ColorSpriteView("PeakSprite2", 0, 0, SCREEN_WIDTH, 2, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower2 = ReducedBandsBandPowerModel("Sound Power Model 2", 2, 5, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel2 = RainbowColorModel("Color Model 2", 2, 8);
    GravitationalModel m_GravitationalModel2 = GravitationalModel("GravitationalModel2", 1.0, 10.0);

    VerticalBarView m_VerticalBar3 = VerticalBarView("Vertical Bar 3", 1, 0, 1, SCREEN_HEIGHT / 2, MergeType_Add);
    ColorSpriteView m_PeakSprite3 = ColorSpriteView("PeakSprite3", 0, 0, SCREEN_WIDTH, 2, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower3 = ReducedBandsBandPowerModel("Sound Power Model 3", 3, 5, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel3 = RainbowColorModel("Color Model 3", 3, 8);
    GravitationalModel m_GravitationalModel3 = GravitationalModel("GravitationalModel3", 1.0, 10.0);

    VerticalBarView m_VerticalBar4 = VerticalBarView("Vertical Bar 4", 2, 0, 1, SCREEN_HEIGHT / 2, MergeType_Add);
    ColorSpriteView m_PeakSprite4 = ColorSpriteView("PeakSprite4", 0, 0, SCREEN_WIDTH, 2, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower4 = ReducedBandsBandPowerModel("Sound Power Model 4", 4, 5, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel4 = RainbowColorModel("Color Model 4", 4, 8);
    GravitationalModel m_GravitationalModel4 = GravitationalModel("GravitationalModel2", 1.0, 10.0);

    VerticalBarView m_VerticalBar5 = VerticalBarView("Vertical Bar 5", 2, 0, 1, SCREEN_HEIGHT / 2, MergeType_Add);
    ColorSpriteView m_PeakSprite5 = ColorSpriteView("PeakSprite5", 0, 0, SCREEN_WIDTH, 2, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower5 = ReducedBandsBandPowerModel("Sound Power Model 5", 5, 5, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel5 = RainbowColorModel("Color Model 5", 5, 8);
    GravitationalModel m_GravitationalModel5 = GravitationalModel("GravitationalModel5", 1.0, 10.0);

    VerticalBarView m_VerticalBar6 = VerticalBarView("Vertical Bar 6", 3, 0, 1, SCREEN_HEIGHT / 2, MergeType_Add);
    ColorSpriteView m_PeakSprite6 = ColorSpriteView("PeakSprite6", 0, 0, SCREEN_WIDTH, 2, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower6 = ReducedBandsBandPowerModel("Sound Power Model 6", 6, 5, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel6 = RainbowColorModel("Color Model 6", 6, 8);
    GravitationalModel m_GravitationalModel6 = GravitationalModel("GravitationalModel6", 1.0, 10.0);

    VerticalBarView m_VerticalBar7 = VerticalBarView("Vertical Bar 7", 3, 0, 1, SCREEN_HEIGHT / 2, MergeType_Add);
    ColorSpriteView m_PeakSprite7 = ColorSpriteView("PeakSprite7", 0, 0, SCREEN_WIDTH, 2, MergeType_Add);
    ReducedBandsBandPowerModel m_BandPower7 = ReducedBandsBandPowerModel("Sound Power Model 7", 7, 5, 8, m_StatisticalEngineModelInterface);
    RainbowColorModel m_ColorModel7 = RainbowColorModel("Color Model 7", 7, 8);
    GravitationalModel m_GravitationalModel7 = GravitationalModel("GravitationalModel7", 1.0, 10.0);
};
#endif
