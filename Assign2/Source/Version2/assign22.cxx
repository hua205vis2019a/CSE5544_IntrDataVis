#include <vtkImageData.h>
#include <vtkImageFFT.h>
#include <vtkBMPReader.h>
#include <vtkBMPWriter.h>
#include <vtkSmartPointer.h>
#include <vtkImageRFFT.h>
#include <vtkImageCast.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageButterworthHighPass.h>
#include <vtkImageButterworthLowPass.h>
#include <vtkImageBlend.h>


int main(int argc, char* argv[]) {
    // If argument count < 3, we need more files to complete hybrid image.
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " InputFiles Number" << std::endl;
        return EXIT_FAILURE;
    }

    std::string ffName, sfName;
    // The first file is for tree read and the second one is for graph read.
    if (argc > 2){
        ffName = argv[1];
        sfName = argv[2];
    }

    vtkSmartPointer<vtkBMPReader> reader1 = vtkSmartPointer<vtkBMPReader>::New();
    reader1->SetFileName(ffName.c_str());
    reader1->Update();

    vtkSmartPointer<vtkBMPReader> reader2 = vtkSmartPointer<vtkBMPReader>::New();
    reader2->SetFileName(sfName.c_str());
    reader2->Update();

    // high-pass filter
    vtkSmartPointer<vtkImageFFT> hpfft = vtkSmartPointer<vtkImageFFT>::New();
    hpfft->SetInputConnection(reader1->GetOutputPort());
    hpfft->Update();

    vtkSmartPointer<vtkImageButterworthHighPass> hpFilter = vtkSmartPointer<vtkImageButterworthHighPass>::New();
    hpFilter->SetInputConnection(hpfft->GetOutputPort());
    hpFilter->SetXCutOff(0.01);
    hpFilter->SetYCutOff(0.01);
    hpFilter->Update();

    vtkSmartPointer<vtkImageRFFT> hprfftFilter = vtkSmartPointer<vtkImageRFFT>::New();
    hprfftFilter->SetInputConnection(hpFilter->GetOutputPort());
    hprfftFilter->Update();

    vtkSmartPointer<vtkImageExtractComponents> hpifftExtractReal = vtkSmartPointer<vtkImageExtractComponents>::New();
    hpifftExtractReal->SetInputConnection(hprfftFilter->GetOutputPort());
    hpifftExtractReal->SetComponents(0);
    hpifftExtractReal->Update();

    vtkSmartPointer<vtkImageCast> hpcastFilter = vtkSmartPointer<vtkImageCast>::New();
    hpcastFilter->SetInputConnection(hprfftFilter->GetOutputPort());
    hpcastFilter->SetOutputScalarTypeToUnsignedChar();
    hpcastFilter->Update();

    vtkSmartPointer<vtkBMPWriter> writer1 = vtkSmartPointer<vtkBMPWriter>::New();
    writer1->SetFileName("HIGH-PASS.bmp");
    writer1->SetInputConnection(hpcastFilter->GetOutputPort());
    writer1->Write();

    // low-pass filter
    vtkSmartPointer<vtkImageFFT> lpfft = vtkSmartPointer<vtkImageFFT>::New();
    lpfft->SetInputConnection(reader2->GetOutputPort());
    lpfft->Update();

    vtkSmartPointer<vtkImageButterworthLowPass> lpFilter = vtkSmartPointer<vtkImageButterworthLowPass>::New();
    lpFilter->SetInputConnection(lpfft->GetOutputPort());
    lpFilter->SetXCutOff(0.05);
    lpFilter->SetYCutOff(0.05);
    lpFilter->Update();

    vtkSmartPointer<vtkImageRFFT> lprfftFilter = vtkSmartPointer<vtkImageRFFT>::New();
    lprfftFilter->SetInputConnection(lpFilter->GetOutputPort());
    lprfftFilter->Update();

    vtkSmartPointer<vtkImageExtractComponents> lpifftExtractReal = vtkSmartPointer<vtkImageExtractComponents>::New();
    lpifftExtractReal->SetInputConnection(lprfftFilter->GetOutputPort());
    lpifftExtractReal->SetComponents(0);

    vtkSmartPointer<vtkImageCast> lpcastFilter = vtkSmartPointer<vtkImageCast>::New();
    lpcastFilter->SetInputConnection(lpifftExtractReal->GetOutputPort());
    lpcastFilter->SetOutputScalarTypeToUnsignedChar();
    lpcastFilter->Update();

    vtkSmartPointer<vtkBMPWriter> writer2 = vtkSmartPointer<vtkBMPWriter>::New();
    writer2->SetFileName("LOW-PASS.bmp");
    writer2->SetInputConnection(lpcastFilter->GetOutputPort());
    writer2->Write();

    vtkSmartPointer<vtkImageBlend> imageBlend = vtkSmartPointer<vtkImageBlend>::New();
    imageBlend->AddInputData(lpcastFilter->GetOutput());
    imageBlend->AddInputData(hpcastFilter->GetOutput());
    imageBlend->SetOpacity(0, 0.6);
    imageBlend->SetOpacity(1, 0.6);
    imageBlend->Update();

    vtkSmartPointer<vtkBMPWriter> writer3 = vtkSmartPointer<vtkBMPWriter>::New();
    writer3->SetFileName("Hybrid.bmp");
    writer3->SetInputConnection(imageBlend->GetOutputPort());
    writer3->Write();

    return EXIT_SUCCESS;
}

