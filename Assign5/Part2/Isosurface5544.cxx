#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkSliderWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkWidgetEvent.h>
#include <vtkCallbackCommand.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSliderRepresentation3D.h>
#include <vtkProperty.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vtkContourFilter.h>
#include <vtkNamedColors.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>
#include <vtkClipPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkLookupTable.h>
#include <vtkPlane.h>
#include <vtkFeatureEdges.h>
#include <vtkStripper.h>
#include <vtkScalarBarActor.h>

#include <iostream>

class vtkSliderCallback : public vtkCommand
{
public:
  static vtkSliderCallback *New()
  {
    return new vtkSliderCallback;
  }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
    vtkSliderWidget *sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue();
    double color[3];
    this->func->GetColor(value, color);
    this->actor->GetProperty()->SetDiffuseColor(color);
    this->source->SetValue(0, value);
  }
  vtkContourFilter *source;
  vtkActor *actor;
  vtkColorTransferFunction *func;
};

class SliderCallbackX: public vtkCommand {
public:
    static SliderCallbackX *New() {
        return new SliderCallbackX;
    }
    virtual void Execute(vtkObject *caller, unsigned long, void*){
        vtkSliderWidget *sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
        double value = static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())->GetValue();
        this->source->SetValue(value);
        this->source->Update();
    }
    vtkClipPolyData *source;
};

class SliderCallbackY: public vtkCommand {
public:
    static SliderCallbackY *New() {
        return new SliderCallbackY;
    }
    virtual void Execute(vtkObject *caller, unsigned long, void*){
        vtkSliderWidget *sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
        double value = static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())->GetValue();
        this->source->SetValue(value);
        this->source->Update();
    }
    vtkClipPolyData *source;
};

class SliderCallbackZ: public vtkCommand {
public:
    static SliderCallbackZ *New() {
        return new SliderCallbackZ;
    }
    virtual void Execute(vtkObject *caller, unsigned long, void*){
        vtkSliderWidget *sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
        double value = static_cast<vtkSliderRepresentation2D*>(sliderWidget->GetRepresentation())->GetValue();
        this->source->SetValue(value);
        this->source->Update();
    }
    vtkClipPolyData *source;
};

int main (int argc, char *argv[])
{
    if(argc != 2){
        std::cout << "Usage: " << argv[0] << "Filename" << std::endl;
        return EXIT_FAILURE;
    }

    std::string inputFilename = argv[1];

    vtkSmartPointer<vtkStructuredPointsReader> reader = vtkSmartPointer<vtkStructuredPointsReader>::New();
    reader->SetFileName(inputFilename.c_str());
    reader->Update();

    vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

    // Create the RenderWindow, Renderer
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(ren1);
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    double* range;
    range = reader->GetOutput()->GetScalarRange();

    vtkSmartPointer<vtkContourFilter> iso = vtkSmartPointer<vtkContourFilter>::New();

    iso->SetValue(0, (range[0]+range[1])/2);
    iso->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();

    colorTransferFunction->AddRGBPoint(range[0], 1, 0, 0);
    colorTransferFunction->AddRGBPoint((range[0]+range[1])/2, 0, 1, 0);

    auto planeX = vtkSmartPointer<vtkPlane>::New();
    planeX->SetOrigin(0.0, 0.0, 0.0);
    planeX->SetNormal(1.0, 0.0, 0.0);

    auto clipperX = vtkSmartPointer<vtkClipPolyData>::New();
    clipperX->SetInputConnection(iso->GetOutputPort());
    clipperX->SetClipFunction(planeX);
    clipperX->Update();

    auto planeY = vtkSmartPointer<vtkPlane>::New();
    planeY->SetOrigin(0.0, 0.0, 0.0);
    planeY->SetNormal(0.0, 1.0, 0.0);

    auto clipperY = vtkSmartPointer<vtkClipPolyData>::New();
    clipperY->SetInputConnection(clipperX->GetOutputPort());
    clipperY->SetClipFunction(planeY);
    clipperY->Update();

    auto planeZ = vtkSmartPointer<vtkPlane>::New();
    planeZ->SetOrigin(0.0, 0.0, 0.0);
    planeZ->SetNormal(0.0, 0.0, 1.0);

    auto clipperZ = vtkSmartPointer<vtkClipPolyData>::New();
    clipperZ->SetInputConnection(clipperY->GetOutputPort());
    clipperZ->SetClipFunction(planeZ);
    clipperZ->Update();

    vtkSmartPointer<vtkPolyDataMapper> isoMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    isoMapper->SetInputConnection(clipperZ->GetOutputPort());
    isoMapper->SetLookupTable(colorTransferFunction);
    isoMapper->SetScalarModeToUsePointData();
    isoMapper->SetColorModeToMapScalars();

    vtkSmartPointer<vtkActor> isoActor = vtkSmartPointer<vtkActor>::New();
    isoActor->SetMapper(isoMapper);

    auto scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->SetLookupTable(isoMapper->GetLookupTable());
    scalarBar->SetTitle("Isovalues");
    scalarBar->SetLabelFormat("%3.0f");
    scalarBar->SetNumberOfLabels(6);
    scalarBar->SetLookupTable(colorTransferFunction);

    vtkSmartPointer<vtkSliderRepresentation2D> sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
    sliderRep->SetMinimumValue(range[0]);
    sliderRep->SetMaximumValue(range[1]);
    sliderRep->SetValue((range[0] + range[1]) / 2);
    sliderRep->SetLabelFormat("%3.0f");
    sliderRep->SetTitleText("Isosurface Value");

    sliderRep->SetRenderer(ren1);
    sliderRep->GetPoint1Coordinate()->SetValue(0.05, 0.7);
    sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedViewport();
    sliderRep->GetPoint2Coordinate()->SetValue(0.3, 0.7);
    sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedViewport();

    double tubeWidth(.005);
    double sliderLength(.02);
    double titleHeight(.02);
    double labelHeight(.02);

    sliderRep->SetTubeWidth(tubeWidth);
    sliderRep->SetSliderLength(sliderLength);
    sliderRep->SetTitleHeight(titleHeight);
    sliderRep->SetLabelHeight(labelHeight);
    sliderRep->SetEndCapLength(tubeWidth*1.5);
    sliderRep->SetSliderWidth(tubeWidth*1.5);
    sliderRep->GetSliderProperty()->SetColor(0.0, 0.0, 0.0);
    sliderRep->BuildRepresentation();

    vtkSmartPointer<vtkSliderWidget> sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
    sliderWidget->SetInteractor(iren);
    sliderWidget->SetRepresentation(sliderRep);
    sliderWidget->SetAnimationModeToAnimate();
    sliderWidget->EnabledOn();

    vtkSmartPointer<vtkSliderCallback> callback = vtkSmartPointer<vtkSliderCallback>::New();
    callback->actor = isoActor;
    callback->func = colorTransferFunction;
    callback->source = iso;
    sliderWidget->AddObserver(vtkCommand::InteractionEvent,callback);

    vtkSmartPointer<vtkSliderRepresentation2D> sliderRepX = vtkSmartPointer<vtkSliderRepresentation2D>::New();
    sliderRepX->SetMinimumValue(0);
    sliderRepX->SetMaximumValue(200);
    sliderRepX->SetValue(0);
    sliderRepX->SetLabelFormat("%3.0f");
    sliderRepX->SetTitleText("X");

    sliderRepX->SetRenderer(ren1);
    sliderRepX->GetPoint1Coordinate()->SetValue(0.05, 0.5);
    sliderRepX->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedViewport();
    sliderRepX->GetPoint2Coordinate()->SetValue(0.3, 0.5);
    sliderRepX->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedViewport();

    sliderRepX->SetTubeWidth(tubeWidth);
    sliderRepX->SetSliderLength(sliderLength);
    sliderRepX->SetTitleHeight(titleHeight);
    sliderRepX->SetLabelHeight(labelHeight);
    sliderRepX->SetEndCapLength(tubeWidth*1.5);
    sliderRepX->SetSliderWidth(tubeWidth*1.5);
    sliderRepX->GetSliderProperty()->SetColor(0.0, 0.0, 0.0);
    sliderRepX->BuildRepresentation();

    vtkSmartPointer<vtkSliderWidget> sliderWidgetX = vtkSmartPointer<vtkSliderWidget>::New();
    sliderWidgetX->SetInteractor(iren);
    sliderWidgetX->SetRepresentation(sliderRepX);
    sliderWidgetX->SetAnimationModeToAnimate();
    sliderWidgetX->EnabledOn();

    vtkSmartPointer<SliderCallbackX> callbackX = vtkSmartPointer<SliderCallbackX>::New();
    callbackX->source = clipperX;
    sliderWidgetX->AddObserver(vtkCommand::InteractionEvent,callbackX);

    vtkSmartPointer<vtkSliderRepresentation2D> sliderRepY = vtkSmartPointer<vtkSliderRepresentation2D>::New();
    sliderRepY->SetMinimumValue(0);
    sliderRepY->SetMaximumValue(300);
    sliderRepY->SetValue(0);
    sliderRepY->SetLabelFormat("%3.0f");
    sliderRepY->SetTitleText("Y");

    sliderRepY->SetRenderer(ren1);
    sliderRepY->GetPoint1Coordinate()->SetValue(0.05, 0.3);
    sliderRepY->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedViewport();
    sliderRepY->GetPoint2Coordinate()->SetValue(0.3, 0.3);
    sliderRepY->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedViewport();

    sliderRepY->SetTubeWidth(tubeWidth);
    sliderRepY->SetSliderLength(sliderLength);
    sliderRepY->SetTitleHeight(titleHeight);
    sliderRepY->SetLabelHeight(labelHeight);
    sliderRepY->SetEndCapLength(tubeWidth*1.5);
    sliderRepY->SetSliderWidth(tubeWidth*1.5);
    sliderRepY->GetSliderProperty()->SetColor(0.0, 0.0, 0.0);
    sliderRepY->BuildRepresentation();

    vtkSmartPointer<vtkSliderWidget> sliderWidgetY = vtkSmartPointer<vtkSliderWidget>::New();
    sliderWidgetY->SetInteractor(iren);
    sliderWidgetY->SetRepresentation(sliderRepY);
    sliderWidgetY->SetAnimationModeToAnimate();
    sliderWidgetY->EnabledOn();

    vtkSmartPointer<SliderCallbackY> callbackY = vtkSmartPointer<SliderCallbackY>::New();
    callbackY->source = clipperY;
    sliderWidgetY->AddObserver(vtkCommand::InteractionEvent,callbackY);

    vtkSmartPointer<vtkSliderRepresentation2D> sliderRepZ = vtkSmartPointer<vtkSliderRepresentation2D>::New();
    sliderRepZ->SetMinimumValue(0);
    sliderRepZ->SetMaximumValue(500);
    sliderRepZ->SetValue(0);
    sliderRepZ->SetLabelFormat("%3.0f");
    sliderRepZ->SetTitleText("Z");

    sliderRepZ->SetRenderer(ren1);
    sliderRepZ->GetPoint1Coordinate()->SetValue(0.05, 0.1);
    sliderRepZ->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedViewport();
    sliderRepZ->GetPoint2Coordinate()->SetValue(0.3, 0.1);
    sliderRepZ->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedViewport();

    sliderRepZ->SetTubeWidth(tubeWidth);
    sliderRepZ->SetSliderLength(sliderLength);
    sliderRepZ->SetTitleHeight(titleHeight);
    sliderRepZ->SetLabelHeight(labelHeight);
    sliderRepZ->SetEndCapLength(tubeWidth*1.5);
    sliderRepZ->SetSliderWidth(tubeWidth*1.5);
    sliderRepZ->GetSliderProperty()->SetColor(0.0, 0.0, 0.0);
    sliderRepZ->BuildRepresentation();

    vtkSmartPointer<vtkSliderWidget> sliderWidgetZ = vtkSmartPointer<vtkSliderWidget>::New();
    sliderWidgetZ->SetInteractor(iren);
    sliderWidgetZ->SetRepresentation(sliderRepZ);
    sliderWidgetZ->SetAnimationModeToAnimate();
    sliderWidgetZ->EnabledOn();

    vtkSmartPointer<SliderCallbackZ> callbackZ = vtkSmartPointer<SliderCallbackZ>::New();
    callbackZ->source = clipperZ;
    sliderWidgetX->AddObserver(vtkCommand::InteractionEvent,callbackZ);

    ren1->AddActor(isoActor);
    ren1->AddActor2D(scalarBar);
    ren1->ResetCamera();
    ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
    ren1->ResetCameraClippingRange();
    renWin->SetSize(1200, 600);

    iren->Initialize();
    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}