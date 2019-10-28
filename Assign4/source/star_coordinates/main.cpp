#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkDelimitedTextReader.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkPlotPoints.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>

# define VTK_CREATE(type, name) vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

int main(int argc, char* argv[]){
    if(argc != 2){
        std::cout << "Usage: " << argv[0] << "Filename" << std::endl;
        return EXIT_FAILURE;
    }
    std::string inputFilename = argv[1];
    VTK_CREATE(vtkDelimitedTextReader, reader);
    reader->SetFileName(inputFilename.c_str());
    reader->DetectNumericColumnsOn();
    reader->SetFieldDelimiterCharacters(",");
    reader->Update();

    VTK_CREATE(vtkTable, table);
    table = reader->GetOutput();

    VTK_CREATE(vtkContextView, view);
    view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
    view->GetRenderWindow()->SetSize(400, 300);

    VTK_CREATE(vtkChartXY, chart);
    view->GetScene()->AddItem(chart);

    vtkPlot *points = chart->AddPlot(vtkChart::POINTS);
    points->SetInputData(table, 0, 1);
    points->SetColor(0, 0, 255, 255);
    points->SetWidth(1.0);
    vtkPlotPoints::SafeDownCast(points)->SetMarkerStyle(vtkPlotPoints::CIRCLE);

    view->GetRenderWindow()->SetMultiSamples(0);
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();

    return EXIT_SUCCESS;
}