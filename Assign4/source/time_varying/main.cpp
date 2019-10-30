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
#include <vtkPolyData.h>
#include <vtkIdTypeArray.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderView.h>
#include <vtkRegularPolygonSource.h>
# define VTK_CREATE(type, name) vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

int main(int argc, char* argv[]){
    if(argc != 3){
        std::cout << "Usage: " << argv[0] << "Filename" << std::endl;
        return EXIT_FAILURE;
    }

    VTK_CREATE(vtkRenderView, rw);
    rw->SetInteractionMode(vtkRenderView::INTERACTION_MODE_2D);

    std::string lineFilename = argv[1];
    std::string symptomFilename = argv[2];

    VTK_CREATE(vtkDelimitedTextReader, symreader);
    symreader->SetFileName(symptomFilename.c_str());
    symreader->DetectNumericColumnsOn();
    symreader->SetFieldDelimiterCharacters(",");
    symreader->Update();

    VTK_CREATE(vtkTable, table);
    table = symreader->GetOutput();


    VTK_CREATE(vtkPolyDataReader, linereader);
    linereader->SetFileName(lineFilename.c_str());
    linereader->Update();

    VTK_CREATE(vtkPolyData, polydata);
    polydata = linereader->GetOutput();

    int x[4] = {0, -1, 0, 1};
    int y[4] = {-1, 0, 1, 0};
    VTK_CREATE(vtkPoints, point_new);
    for(int i = 0;i < polydata->GetNumberOfPoints();i++) {
        double p[3];
        polydata->GetPoint(i, p);
        if (i % 2 == 0) {
            point_new->InsertNextPoint(p[0], p[1], p[2]);
        } else {
            point_new->InsertNextPoint(p[0], log(p[1]) / log(2) * 4, p[2]);
            for (int j = 0; j < 4; j++) {
                VTK_CREATE(vtkRegularPolygonSource, ps);
                ps->SetNumberOfSides(50);
                double radius = table->GetValue((i + 1) / 2, 0).ToDouble() / 100;
                ps->SetRadius(radius);
                ps->SetCenter(p[0] + x[j] * radius, log(p[1]) / log(2) * 4 + y[j] * radius, p[2]);

                VTK_CREATE(vtkPolyDataMapper, m);
                m->SetInputConnection(ps->GetOutputPort());

                VTK_CREATE(vtkActor, a);
                a->SetMapper(m);

                rw->GetRenderer()->AddActor(a);
            }
        }
    }

    VTK_CREATE(vtkCellArray, lines);
    lines = polydata->GetLines();

    VTK_CREATE(vtkPolyData, polydata_new);
    polydata_new->SetPoints(point_new);
    polydata_new->SetLines(lines);

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputData(polydata_new);
    mapper->SetScalarVisibility(1);

    VTK_CREATE(vtkActor, actor);
    actor->SetMapper(mapper);



    rw->GetRenderer()->AddActor(actor);

    rw->Update();
    rw->ResetCamera();
    rw->GetInteractor()->Start();

    return EXIT_SUCCESS;
}