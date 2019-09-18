#include <vtkSmartPointer.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageData.h>
#include <vtkImageFFT.h>
#include <vtkImageButterworthHighPass.h>
#include <vtkImageButterworthLowPass.h>
#include <vtkImageRFFT.h>
#include <vtkBMPWriter.h>
#include <vtkImageCast.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageBlend.h>
#include <vtkImageExtractComponents.h>

int main(int argc, char* argv[]) {
    // If argument count < 3, we need more files to complete hybrid image.
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " InputFiles Number" << std::endl;
        return EXIT_FAILURE;
    }

    std::string highName, lowName;
    // The first file is for tree read and the second one is for graph read.
    if (argc > 2) {
        highName = argv[1];
        lowName = argv[2];
    }
    // Read data
    vtkSmartPointer<vtkImageReader2Factory> highReaderFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkSmartPointer<vtkImageReader2> highReader;
    highReader.TakeReference(highReaderFactory->CreateImageReader2(highName.c_str()));
    highReader->SetFileName(highName.c_str());
    highReader->Update();

    vtkSmartPointer<vtkImageReader2Factory> lowReaderFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkSmartPointer<vtkImageReader2> lowReader;
    lowReader.TakeReference(lowReaderFactory->CreateImageReader2(lowName.c_str()));
    lowReader->SetFileName(lowName.c_str());
    lowReader->Update();

    // Split into 3 channels
    vtkSmartPointer<vtkImageData> highColors[3];
    int* dim = highReader->GetOutput()->GetDimensions();
    highColors[0] = vtkSmartPointer<vtkImageData>::New();
    highColors[0]->SetDimensions(dim);
    highColors[0]->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
    highColors[1] = vtkSmartPointer<vtkImageData>::New();
    highColors[1]->SetDimensions(dim);
    highColors[1]->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
    highColors[2] = vtkSmartPointer<vtkImageData>::New();
    highColors[2]->SetDimensions(dim);
    highColors[2]->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

    for(int x = 0;x < dim[0];x++){
        for(int y = 0;y < dim[1];y++){
            double dataR = highReader->GetOutput()->GetScalarComponentAsDouble(x, y, 0, 0);
            double dataG = highReader->GetOutput()->GetScalarComponentAsDouble(x, y, 0, 1);
            double dataB = highReader->GetOutput()->GetScalarComponentAsDouble(x, y, 0, 2);
            highColors[0]->SetScalarComponentFromDouble(x, y, 0, 0, dataR);
            highColors[1]->SetScalarComponentFromDouble(x, y, 0, 0, dataG);
            highColors[2]->SetScalarComponentFromDouble(x, y, 0, 0, dataB);
        }
    }

    vtkSmartPointer<vtkImageFFT> RHfft = vtkSmartPointer<vtkImageFFT>::New();
    RHfft->SetInputData(highColors[0]);
    RHfft->Update();

    vtkSmartPointer<vtkImageButterworthHighPass> RHFilter = vtkSmartPointer<vtkImageButterworthHighPass>::New();
    RHFilter->SetInputConnection(RHfft->GetOutputPort());
    RHFilter->SetXCutOff(0.01);
    RHFilter->SetYCutOff(0.01);
    RHFilter->Update();

    vtkSmartPointer<vtkImageRFFT> RHrfftFilter = vtkSmartPointer<vtkImageRFFT>::New();
    RHrfftFilter->SetInputConnection(RHFilter->GetOutputPort());
    RHrfftFilter->Update();

    vtkSmartPointer<vtkImageExtractComponents> RrfftFilter = vtkSmartPointer<vtkImageExtractComponents>::New();
    RrfftFilter->SetInputConnection(RHrfftFilter->GetOutputPort());
    RrfftFilter->SetComponents(0);
    RrfftFilter->Update();

    vtkSmartPointer<vtkImageCast> RHcastFilter = vtkSmartPointer<vtkImageCast>::New();
    RHcastFilter->SetInputConnection(RrfftFilter->GetOutputPort());
    RHcastFilter->SetOutputScalarTypeToUnsignedChar();
    RHcastFilter->Update();

    vtkSmartPointer<vtkImageFFT> GHfft = vtkSmartPointer<vtkImageFFT>::New();
    GHfft->SetInputData(highColors[1]);
    GHfft->Update();

    vtkSmartPointer<vtkImageButterworthHighPass> GHFilter = vtkSmartPointer<vtkImageButterworthHighPass>::New();
    GHFilter->SetInputConnection(GHfft->GetOutputPort());
    GHFilter->SetXCutOff(0.01);
    GHFilter->SetYCutOff(0.01);
    GHFilter->Update();

    vtkSmartPointer<vtkImageRFFT> GHrfftFilter = vtkSmartPointer<vtkImageRFFT>::New();
    GHrfftFilter->SetInputConnection(GHFilter->GetOutputPort());
    GHrfftFilter->Update();

    vtkSmartPointer<vtkImageExtractComponents> GrfftFilter = vtkSmartPointer<vtkImageExtractComponents>::New();
    GrfftFilter->SetInputConnection(GHrfftFilter->GetOutputPort());
    GrfftFilter->SetComponents(0);
    GrfftFilter->Update();

    vtkSmartPointer<vtkImageCast> GHcastFilter = vtkSmartPointer<vtkImageCast>::New();
    GHcastFilter->SetInputConnection(GrfftFilter->GetOutputPort());
    GHcastFilter->SetOutputScalarTypeToUnsignedChar();
    GHcastFilter->Update();

    vtkSmartPointer<vtkImageFFT> BHfft = vtkSmartPointer<vtkImageFFT>::New();
    BHfft->SetInputData(highColors[2]);
    BHfft->Update();

    vtkSmartPointer<vtkImageButterworthHighPass> BHFilter = vtkSmartPointer<vtkImageButterworthHighPass>::New();
    BHFilter->SetInputConnection(BHfft->GetOutputPort());
    BHFilter->SetXCutOff(0.01);
    BHFilter->SetYCutOff(0.01);
    BHFilter->Update();

    vtkSmartPointer<vtkImageRFFT> BHrfftFilter = vtkSmartPointer<vtkImageRFFT>::New();
    BHrfftFilter->SetInputConnection(BHFilter->GetOutputPort());
    BHrfftFilter->Update();

    vtkSmartPointer<vtkImageExtractComponents> BrfftFilter = vtkSmartPointer<vtkImageExtractComponents>::New();
    BrfftFilter->SetInputConnection(BHrfftFilter->GetOutputPort());
    BrfftFilter->SetComponents(0);
    BrfftFilter->Update();

    vtkSmartPointer<vtkImageCast> BHcastFilter = vtkSmartPointer<vtkImageCast>::New();
    BHcastFilter->SetInputConnection(BrfftFilter->GetOutputPort());
    BHcastFilter->SetOutputScalarTypeToUnsignedChar();
    BHcastFilter->Update();

    vtkImageData* R = RHcastFilter->GetOutput();
    vtkImageData* G = GHcastFilter->GetOutput();
    vtkImageData* B = BHcastFilter->GetOutput();
    vtkSmartPointer<vtkImageData> mergedRfftImage = vtkSmartPointer<vtkImageData>::New();
    mergedRfftImage->SetDimensions(dim);
    mergedRfftImage->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

    for (int x = 0; x < dim[0]; x++){
        for(int y = 0; y < dim[1]; y++){
            double r = R->GetScalarComponentAsDouble(x, y, 0, 0);
            double g = G->GetScalarComponentAsDouble(x, y, 0, 0);
            double b = B->GetScalarComponentAsDouble(x, y, 0, 0);

            mergedRfftImage->SetScalarComponentFromDouble(x, y, 0, 0, r);
            mergedRfftImage->SetScalarComponentFromDouble(x, y, 0, 1, g);
            mergedRfftImage->SetScalarComponentFromDouble(x, y, 0, 2, b);
        }
    }

    vtkSmartPointer<vtkImageGaussianSmooth> gsFilter = vtkSmartPointer<vtkImageGaussianSmooth>::New();
    gsFilter->SetInputData(mergedRfftImage);
    gsFilter->Update();

    vtkSmartPointer<vtkBMPWriter> writer1 = vtkSmartPointer<vtkBMPWriter>::New();
    writer1->SetFileName("HIGH-PASS.bmp");
    writer1->SetInputConnection(gsFilter->GetOutputPort());
    writer1->Update();
    writer1->Write();

    vtkSmartPointer<vtkImageGaussianSmooth> gsFilter2 = vtkSmartPointer<vtkImageGaussianSmooth>::New();
    gsFilter2->SetInputConnection(lowReader->GetOutputPort());
    gsFilter2->Update();

    vtkSmartPointer<vtkImageCast> lpcastFilter = vtkSmartPointer<vtkImageCast>::New();
    lpcastFilter->SetInputConnection(gsFilter2->GetOutputPort());
    lpcastFilter->SetOutputScalarTypeToUnsignedChar();
    lpcastFilter->Update();

    vtkSmartPointer<vtkBMPWriter> writer2 = vtkSmartPointer<vtkBMPWriter>::New();
    writer2->SetFileName("LOW-PASS.bmp");
    writer2->SetInputConnection(lpcastFilter->GetOutputPort());
    writer2->Write();

    vtkSmartPointer<vtkImageBlend> imageBlend = vtkSmartPointer<vtkImageBlend>::New();
    imageBlend->AddInputData(lpcastFilter->GetOutput());
    imageBlend->AddInputData(mergedRfftImage);
    imageBlend->SetOpacity(0, 0.5);
    imageBlend->SetOpacity(1, 0.5);
    imageBlend->Update();

    vtkSmartPointer<vtkBMPWriter> writer3 = vtkSmartPointer<vtkBMPWriter>::New();
    writer3->SetFileName("Hybrid.bmp");
    writer3->SetInputConnection(imageBlend->GetOutputPort());
    writer3->Write();

    return EXIT_SUCCESS;
}


