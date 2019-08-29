#include <vtkXMLTreeReader.h>
#include <vtkTreeRingView.h>
#include <vtkSmartPointer.h>
#include <vtkViewTheme.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTreeLayoutStrategy.h>

int main(int argc, char* argv[]){
    // If argument count < 3, we need more files to complete the graph drawing.
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " InputTreeFile(.vtp) Number" << std::endl;
        return EXIT_FAILURE;
    }

    std::string treeFileName, graphFileName;
    // The first file is for tree read and the second one is for graph read.
    if (argc > 2){
        treeFileName = argv[1];
        graphFileName = argv[2];
    }

    // Two XMLReader
    vtkSmartPointer<vtkXMLTreeReader> treeReader = vtkSmartPointer<vtkXMLTreeReader>::New();
    treeReader->SetFileName(treeFileName.c_str());
    treeReader->SetEdgePedigreeIdArrayName("tree edge");
    treeReader->GenerateVertexPedigreeIdsOff();
    treeReader->SetVertexPedigreeIdArrayName("id");

    vtkSmartPointer<vtkXMLTreeReader> graphReader = vtkSmartPointer<vtkXMLTreeReader>::New();
    graphReader->SetFileName(graphFileName.c_str());
    graphReader->SetEdgePedigreeIdArrayName("graph edge");
    graphReader->GenerateVertexPedigreeIdsOff();
    graphReader->SetVertexPedigreeIdArrayName("id");

    treeReader->Update();
    graphReader->Update();

    vtkSmartPointer<vtkTreeLayoutStrategy> treeStrat = vtkSmartPointer<vtkTreeLayoutStrategy>::New();
    treeStrat->RadialOn();
    treeStrat->SetAngle(360);
    treeStrat->SetLogSpacingValue(1);

    vtkSmartPointer<vtkTreeRingView> view = vtkSmartPointer<vtkTreeRingView>::New();
    view->SetTreeFromInputConnection(treeReader->GetOutputPort());
    view->SetGraphFromInputConnection(graphReader->GetOutputPort());
    view->SetAreaLabelArrayName("id");
    view->SetAreaColorArrayName("VertexDegree");
    view->SetAreaHoverArrayName("id");
    view->SetAreaLabelVisibility(true);
    view->SetShrinkPercentage(0.02);
    view->SetBundlingStrength(.75);
    view->Update();
    view->SetEdgeColorArrayName("graph edge");
    view->SetColorEdges(true);

    vtkViewTheme* const theme = vtkViewTheme::CreateMellowTheme();
    theme->SetLineWidth(2);
    theme->SetPointSize(10);
    theme->SetSelectedCellColor(1, 1, 1);
    theme->SetSelectedPointColor(1, 1, 1);
    view->ApplyViewTheme(theme);
    theme->FastDelete();

    view->GetRenderWindow()->SetSize(1028, 1028);
    view->ResetCamera();
    view->Render();

    view->GetInteractor()->Start();

    return EXIT_SUCCESS;
}