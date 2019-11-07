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
    /*
    Fill this part
    */
    double color[3];
    this->func->GetColor(value, color);
    this->actor->GetProperty()->SetDiffuseColor(color);
    this->source->SetValue(0, value);
    std::cout << "Current iso value: "<< value << std::endl;
  }
  /*
    Fill this part
  */
  vtkContourFilter *source;
  vtkActor *actor;
  vtkColorTransferFunction *func;
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
    /*
    Fill this part
    */
    iso->SetValue(0, (range[0]+range[1])/2);
    iso->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    /*
    Fill this part
    */
    colorTransferFunction->AddRGBPoint(range[0], 1, 0, 0);
    colorTransferFunction->AddRGBPoint((range[0]+range[1])/2, 0, 1, 0);

    vtkSmartPointer<vtkPolyDataMapper> isoMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    /*
    Fill this part
    */
    isoMapper->SetInputConnection(iso->GetOutputPort());
    isoMapper->ScalarVisibilityOff();

    vtkSmartPointer<vtkActor> isoActor = vtkSmartPointer<vtkActor>::New();
    /*
    Fill this part
    */
    isoActor->SetMapper(isoMapper);
    isoActor->GetProperty()->SetOpacity(.5);



    //----------------Slider Start----------------
    vtkSmartPointer<vtkSliderRepresentation2D> sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
    sliderRep->SetMinimumValue(range[0]);
    sliderRep->SetMaximumValue(range[1]);
    sliderRep->SetValue((range[0] + range[1]) / 2);
    sliderRep->SetTitleText("Isosurface Value");

    /*
    Fill this part (setting sliderRep properties)
    */
    sliderRep->SetRenderer(ren1);
    sliderRep->GetPoint1Coordinate()->SetValue(0.1, 0.1);
    sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedViewport();
    sliderRep->GetPoint2Coordinate()->SetValue(0.4, 0.1);
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
    /*
    Fill this part
    */
    callback->actor = isoActor;
    callback->func = colorTransferFunction;
    callback->source = iso;

    sliderWidget->AddObserver(vtkCommand::InteractionEvent,callback);
    //----------------Slider End----------------

    ren1->AddActor(isoActor);
    ren1->ResetCamera();
    ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
    ren1->ResetCameraClippingRange();
    renWin->SetSize(1200, 600);

    iren->Initialize();
    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}