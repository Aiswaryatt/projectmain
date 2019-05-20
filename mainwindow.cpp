#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Brouse_clicked()
{
     ui->Result->setText(" ");
    QString mFilename;
    mFilename = QFileDialog::getOpenFileName();
    bool Status=true;
    Mat Img_1,PatchImage;

    Img_1=imread(mFilename.toStdString());
    if((Img_1.rows>1000)||(Img_1.cols>1200))
    {
          cv::resize(Img_1,Img_1,Size(Img_1.cols/2,Img_1.rows/2));
    }
    ////////////////////////////Use imread ////////////////////////////////////
    // PatchImage=imread("E:\\2019Database\\countonme\\035patch.jpg");
    ////////////////////////////////or/////////////////////////////////////////
    vector<Rect2d> ObjRegion;

    QMessageBox::information(this,"Patch Selection","Select a region using Mouse and press Space and Esc");

    selectROI("crop your Object",Img_1,ObjRegion);

    imshow("selected region",Img_1(ObjRegion[0]));

    PatchImage=Img_1(ObjRegion[0]).clone();
    /////////////////////////////////////////////////////////////////////
    if(PatchImage.empty()||Img_1.empty())
    {
        cout<<"images not loaded correctly"<<endl;
        QMessageBox::warning(this,"Images not loaded correctly","enter all detailes");
    }
    //imshow("input1",Img_1);
    //imshow("input2",PatchImage);

    //initiallise the structure parameters
    TemplatePatches Input;
    Input.Image=Img_1.clone();
    Input.Patches.push_back(PatchImage);
    Input.PatchNumber=0;
    Input.InitialPatch=PatchImage.clone();

    UpdatePatches Match;
    while(Status)
    {
        int PreviousPatchSize=Input.Patches.size();
        Status=Match.TemplateMatching(Input);
        cout<<"size of Rectangles"<<Input.candidatePatches.size()<<endl;
        if(Status)
        {
            for(int Index=0;Index<Input.candidatePatches.size();Index++)
            {
                //similarity check
                Input.Patches.push_back(Input.candidatePatches[Index]);

            }
            Input.candidatePatches.clear();
            Input.PatchNumber+=PreviousPatchSize;
            //Input.Image=Img_1.clone();

        }
        else
        {
            break;
        }
    }
    Match.RemoveOverlapedRois(Input.RoiOfPatches,.5);
    for(int loc=0;loc<Input.RoiOfPatches.size();loc++)
    {
        rectangle(Img_1,Input.RoiOfPatches[loc],Scalar(0,100,255),2);
    }

    imshow("resized input to see entire image",Img_1);
    cout<<"countRoi="<<Input.RoiOfPatches.size()<<endl;
    cout<<"countPatch="<<Input.Patches.size()<<endl;

    int Count=Input.RoiOfPatches.size();
    ui->Result->setText(QString(QString::number(Count)));

}
