#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkDelimitedTextReader.h>
#include <vtkTable.h>
#include <vtkContextView.h>
#include <vtkChartXY.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkContextScene.h>
#include <vtkIntArray.h>
#include <vtkPlot.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkContextActor.h>
#include <vtkAxis.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#define VTK_CREATE(type, name) vtkSmartPointer<type> name = vtkSmartPointer<type>::New()


int main(int argc, char* argv[]) {
    if(argc != 2){
        std::cout << "Usage: " << argv[0]
                  << "Filename" << std::endl;
        return EXIT_FAILURE;
    }

    // read table
    std::string inputFilename = argv[1];
    VTK_CREATE(vtkDelimitedTextReader, reader);
    reader->SetFileName(inputFilename.c_str());
    reader->DetectNumericColumnsOn();
    reader->SetFieldDelimiterCharacters(",");
    reader->Update();

    VTK_CREATE(vtkTable, tableOrigin);
    tableOrigin = reader->GetOutput();

    // reconstruct the origin table to two tables for male and female
    VTK_CREATE(vtkTable, tableMale);
    VTK_CREATE(vtkTable, tableFemale);

    VTK_CREATE(vtkIntArray, malecat);
    malecat->SetName("malecat");
    tableMale->AddColumn(malecat);

    VTK_CREATE(vtkIntArray, deadMale);
    deadMale->SetName("deadMale");
    tableMale->AddColumn(deadMale);

    VTK_CREATE(vtkIntArray, injureMale);
    injureMale->SetName("injureMale");
    tableMale->AddColumn(injureMale);

    VTK_CREATE(vtkIntArray, femalecat);
    femalecat->SetName("femalecat");
    tableFemale->AddColumn(femalecat);

    VTK_CREATE(vtkIntArray, deadFemale);
    deadFemale->SetName("deadFemale");
    tableFemale->AddColumn(deadFemale);

    VTK_CREATE(vtkIntArray, injureFemale);
    injureFemale->SetName("injureFemale");
    tableFemale->AddColumn(injureFemale);

    tableMale->SetNumberOfRows(4);
    tableFemale->SetNumberOfRows(4);


    for(int i = 0;i < 4;i++){
        tableMale->SetValue(i, 0, i+1);
        tableMale->SetValue(i, 1, tableOrigin->GetValue(1, 2*i+1));
        tableMale->SetValue(i, 2, tableOrigin->GetValue(2, 2*i+1));
        tableFemale->SetValue(i, 0, i+1);
        tableFemale->SetValue(i, 1, tableOrigin->GetValue(1, 2*i+2));
        tableFemale->SetValue(i, 2, tableOrigin->GetValue(2, 2*i+2));
    }

    // 3 views for male, female and legend
    VTK_CREATE(vtkRenderWindow, renwin);
    renwin->SetMultiSamples(0);
    renwin->SetSize(2000, 1000);
    VTK_CREATE(vtkRenderWindowInteractor, iren);
    iren->SetRenderWindow(renwin);

    // male
    VTK_CREATE(vtkRenderer, rendermale);
    rendermale->SetBackground(1.0, 1.0, 1.0);
    rendermale->SetViewport(0.0, 0.35, 1.0, 1.0);
    renwin->AddRenderer(rendermale);

    VTK_CREATE(vtkChartXY, chartMale);
    VTK_CREATE(vtkContextScene, malescene);
    VTK_CREATE(vtkContextActor, maleactor);

    malescene->AddItem(chartMale);
    maleactor->SetScene(malescene);
    rendermale->AddActor(maleactor);
    malescene->SetRenderer(rendermale);

    chartMale->GetAxis(vtkAxis::LEFT)->SetRange(0, 3000);
    chartMale->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);
    chartMale->GetAxis(vtkAxis::RIGHT)->SetRange(0, 60000);
    chartMale->GetAxis(vtkAxis::RIGHT)->SetVisible(true);
    chartMale->GetAxis(vtkAxis::RIGHT)->SetBehavior(vtkAxis::FIXED);
    chartMale->GetAxis(vtkAxis::BOTTOM)->SetTitle(" ");
    chartMale->GetAxis(vtkAxis::LEFT)->SetTitle(" ");
    chartMale->GetAxis(vtkAxis::BOTTOM)->SetTicksVisible(false);
    chartMale->GetAxis(vtkAxis::BOTTOM)->SetNumberOfTicks(0);
    chartMale->SetShowLegend(true);

    vtkPlot *line = 0;
    line = chartMale->AddPlot(vtkChart::BAR);
    line->SetInputData(tableMale, 0, 1);
    line->SetColor(0,0,0,255);
    chartMale->SetPlotCorner(line, 0);

    line = chartMale->AddPlot(vtkChart::BAR);
    line->SetInputData(tableMale, 0, 2);
    line->SetColor(105,105,105,255);
    chartMale->SetPlotCorner(line, 1);

    // female
    VTK_CREATE(vtkRenderer, renderfemale);
    renderfemale->SetBackground(1.0, 1.0, 1.0);
    renderfemale->SetViewport(0.0, 0.0, 1.0, 0.35);
    renwin->AddRenderer(renderfemale);

    VTK_CREATE(vtkChartXY, chartFemale);
    VTK_CREATE(vtkContextScene, femalescene);
    VTK_CREATE(vtkContextActor, femaleactor);

    femalescene->AddItem(chartFemale);
    femaleactor->SetScene(femalescene);
    renderfemale->AddActor(femaleactor);
    femalescene->SetRenderer(renderfemale);

    chartFemale->GetAxis(vtkAxis::LEFT)->SetRange(0, 1250);
    chartFemale->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);
    chartFemale->GetAxis(vtkAxis::RIGHT)->SetRange(0, 25000);
    chartFemale->GetAxis(vtkAxis::RIGHT)->SetBehavior(vtkAxis::FIXED);
    chartFemale->GetAxis(vtkAxis::RIGHT)->SetVisible(true);
    chartFemale->GetAxis(vtkAxis::BOTTOM)->SetTitle(" ");
    chartFemale->GetAxis(vtkAxis::LEFT)->SetTitle(" ");
    chartFemale->GetAxis(vtkAxis::BOTTOM)->SetTicksVisible(false);
    chartFemale->GetAxis(vtkAxis::BOTTOM)->SetNumberOfTicks(0);
    chartFemale->SetShowLegend(true);

    line = chartFemale->AddPlot(vtkChart::BAR);
    line->SetInputData(tableFemale, 0, 1);
    line->SetColor(0,0,0,255);
    chartFemale->SetPlotCorner(line, 0);

    line = chartFemale->AddPlot(vtkChart::BAR);
    line->SetInputData(tableFemale, 0, 2);
    line->SetColor(211,211,211,255);
    chartFemale->SetPlotCorner(line, 1);

    VTK_CREATE(vtkTextActor, textPedestrians);
    VTK_CREATE(vtkTextActor, textBicycle);
    VTK_CREATE(vtkTextActor, textMotorcycles);
    VTK_CREATE(vtkTextActor, textFour);
    VTK_CREATE(vtkTextActor, textMen);
    VTK_CREATE(vtkTextActor, textWomen);

    textPedestrians->SetInput("Pedestrians");
    textPedestrians->SetPosition(250, 400);
    textPedestrians->GetTextProperty()->SetFontSize(24);
    textPedestrians->GetTextProperty()->SetColor(0.0, 0.0, 0.0);
    rendermale->AddActor2D(textPedestrians);


    textBicycle->SetInput("Bicycle");
    textBicycle->SetPosition(750, 400);
    textBicycle->GetTextProperty()->SetFontSize(24);
    textBicycle->GetTextProperty()->SetColor(0.0, 0.0, 0.0);
    rendermale->AddActor2D(textBicycle);

    textMotorcycles->SetInput("Motorcycles");
    textMotorcycles->SetPosition(1050, 600);
    textMotorcycles->GetTextProperty()->SetFontSize(24);
    textMotorcycles->GetTextProperty()->SetColor(0.0, 0.0, 0.0);
    rendermale->AddActor2D(textMotorcycles);

    textFour->SetInput("Four-wheeled");
    textFour->SetPosition(1650, 500);
    textFour->GetTextProperty()->SetFontSize(24);
    textFour->GetTextProperty()->SetColor(0.0, 0.0, 0.0);
    rendermale->AddActor2D(textFour);

    textMen->SetInput("MEN");
    textMen->SetPosition(750, 300);
    textMen->GetTextProperty()->SetFontSize(24);
    textMen->GetTextProperty()->SetColor(0.0, 0.0, 0.0);
    rendermale->AddActor2D(textMen);

    textWomen->SetInput("WOMEN");
    textWomen->SetPosition(1000, 200);
    textWomen->GetTextProperty()->SetFontSize(24);
    textWomen->GetTextProperty()->SetColor(0.0, 0.0, 0.0);
    renderfemale->AddActor2D(textWomen);




    iren->Initialize();
    iren->Start();

    return EXIT_SUCCESS;
}