/*
 Copyright © 2021  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.
    
    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

//#pragma once
#include <QProcess>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tpstrategywind.h"
//#include "tokicolor.cpp"
#include "Collecter.cpp"
#include "Pages.cpp"

#include "mcmap.cpp"
#include "Page0.cpp"
#include "Page1.cpp"
#include "Page2.cpp"
#include "Page3.cpp"
#include "Page4.cpp"
#include "Page5.cpp"
#include "Page6.cpp"
#include "Page7.cpp"
#include "Page8.cpp"

ColorSet* TokiColor::Allowed=NULL;
ColorSet*TokiColor::Basic=NULL;
short TokiColor::DepthIndexEnd[4]={63,127,191,255};
unsigned char TokiColor::DepthCount[4]={64,64,64,64};
bool TokiColor::needFindSide=false;
Matrix<float,2,3> DitherMapLR,DitherMapRL;
const short WaterColumnSize[3]={11,6,1};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug("成功setupUi");
    Collected=false;
    Data.step=0;
    connect(ui->menuStart,SIGNAL(triggered(QAction*)),this,SLOT(turnToPage0()));
    connect(ui->menuImPic,SIGNAL(triggered(QAction*)),this,SLOT(turnToPage1()));
    connect(ui->menuType,SIGNAL(triggered(QAction*)),this,SLOT(turnToPage2()));
    connect(ui->menuBL,SIGNAL(triggered(QAction*)),this,SLOT(turnToPage3()));
    connect(ui->menuAdjPic,SIGNAL(triggered(QAction*)),this,SLOT(turnToPage4()));
    connect(ui->menuExLite,SIGNAL(triggered()),this,SLOT(turnToPage5()));
    //connect(ui->menuExMcF,SIGNAL(triggered()),this,SLOT(turnToPage6()));
    connect(ui->menuExData,SIGNAL(triggered()),this,SLOT(turnToPage7()));
    connect(ui->actionAboutSlopeCraft,SIGNAL(triggered()),this,SLOT(turnToPage8()));    
    connect(ui->actionChinese,SIGNAL(triggered()),this,SLOT(turnCh()));
    connect(ui->actionEnglish,SIGNAL(triggered()),this,SLOT(turnEn()));
    connect(ui->actionToki,SIGNAL(triggered()),this,SLOT(contactG()));
    connect(ui->actionDoki,SIGNAL(triggered()),this,SLOT(contactB()));

    qDebug("成功connect所有的菜单");


    connect(ui->NextPage,SIGNAL(clicked()),this,SLOT(turnToPage2()));
    connect(ui->NextPage2,SIGNAL(clicked()),this,SLOT(turnToPage3()));
    connect(ui->NextPage3,SIGNAL(clicked()),this,SLOT(turnToPage4()));
    connect(ui->ExLite,SIGNAL(clicked()),this,SLOT(turnToPage5()));
    //connect(ui->ExMcF,SIGNAL(clicked()),this,SLOT(turnToPage6()));
    connect(ui->ExData,SIGNAL(clicked()),this,SLOT(turnToPage7()));
    connect(ui->FinishExLite,SIGNAL(clicked()),this,SLOT(turnToPage8()));    
    connect(ui->FinshExData,SIGNAL(clicked()),this,SLOT(turnToPage8()));
    connect(ui->Exit,SIGNAL(clicked()),this,SLOT(close()));
    qDebug("成功connect所有的翻页按钮");

    connect(ui->isGame12,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame13,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame14,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame15,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame16,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame17,SIGNAL(clicked()),this,SLOT(grabGameVersion()));

    turnToPage(0);

    Data.parent=this;
    Data.ExLitestep=-1;
    Data.ExMcFstep=-1;
    //checkBlockIds();
    //QString FolderPath=QFileDialog::getExistingDirectory(this,tr("请选择导出的文件夹"));
    //qDebug()<<FolderPath;
    /*TokiColor::Allowed=&Data.Allowed;
    TokiColor::Basic=&Data.Basic;*/
    transSubWind=nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showPreview()
{
    if(Data.ExLitestep<2)return;
    if(Data.step<5)return;

    PreviewWind*preWind=new PreviewWind(this);
    preWind->Src.resize(61);
    preWind->BlockCount.resize(61);

    for(int i=0;i<61;i++)
    {
        preWind->BlockCount[i]=0;
        preWind->Src[i]=Blocks[i][Data.SelectedBlockList[i]];
    }


    for(int y=0;y<Data.size3D[1];y++)
        for(int z=0;z<Data.size3D[2];z++)
        {
            for(int x=0;x<Data.size3D[0];x++)
            {
                if(Data.Build(x,y,z)<=0)continue;
                preWind->BlockCount[Data.Build(x,y,z)-1]++;
            }
        }
    qDebug()<<"去重前有："<<preWind->Src.size()<<"个元素";
    auto iS=preWind->Src.begin();
    for(auto ib=preWind->BlockCount.begin();ib!=preWind->BlockCount.end();)
    {
        if(*ib>0)
        {
            ib++;
            iS++;
            continue;
        }
        ib=preWind->BlockCount.erase(ib);
        iS=preWind->Src.erase(iS);
    }

    preWind->size[0]=Data.size3D[0];
    preWind->size[1]=Data.size3D[1];
    preWind->size[2]=Data.size3D[2];

    qDebug()<<"去重后有："<<preWind->Src.size()<<"个元素";
    preWind->Water=Blocks[12][0];

    //preWind->Src[1]=Blocks[1][0];preWind->BlockCount[1]=1919810;

    preWind->ShowMaterialList();
    preWind->show();
}


void MainWindow::InitializeAll()
{
    if(!Collected)
    {
        Collect();
        qDebug("Collected");

        Collected=true;
        qDebug("StartWithSlope中的初始化部分完成");
    }
    static bool needInitialize=true;
    if(needInitialize)
    {

        GetBLCreative(BLCreative);
        GetBLCheaper(BLCheaper);
        GetBLBetter(BLBetter);
        GetBLGlowing(BLGlowing);
        qDebug("成功初始化四个预设方块列表");
        qDebug()<<"当前运行路径："<<QCoreApplication::applicationDirPath();
        //QString DirPath=QCoreApplication::applicationDirPath()+'/';
        QDir::setCurrent(QCoreApplication::applicationDirPath());
        readFromFile("./RGB.Toki",Data.Basic._RGB);
        readFromFile("./HSV.Toki",Data.Basic.HSV);
        readFromFile("./Lab.Toki",Data.Basic.Lab);
        readFromFile("./XYZ.Toki",Data.Basic.XYZ);
        qDebug("成功载入颜色");

        qDebug("导入图片按钮处的初始化部分完成");


        showColorColors();
        qDebug("成功为Colors赋予颜色");
        needInitialize=false;
#ifdef dispDerivative
    //checkBlockIds();
    makeImage(1);
#endif
    }
}


void MainWindow::contactG()
{
    static QString Toki="";
    if(Toki=="")
    {
        const short size3D[]={926, 460, 460, 105, 546, 57, 230, 230, 281, 2085, 460, 926, 925, 102, 292, 404, 84, 136, 230, 268, 84, 101, 2085, 897, 84, 329, 925, 281, 230, 60, 225, 84, 105, 238, 190, 169, 396, 24, 460};
        Toki=this->Noder(size3D,sizeof(size3D)/2);
    }
    QDesktopServices::openUrl(QUrl(Toki));
}

void MainWindow::contactB()
{
    static QString Toki="";
    if(Toki=="")
    {
        const short sizePic[]={926, 460, 460, 105, 546, 57, 230, 230, 546, 105, 396, 404, 238, 292, 102, 2085, 225, 2085, 102, 2085, 225, 2085, 292, 404, 84, 136, 230, 368, 285, 1082, 301, 542, 266, 542, 368, 1082};
        Toki=this->Noder(sizePic,sizeof(sizePic)/2);
    }
    QDesktopServices::openUrl(QUrl(Toki));
}

#ifndef tpSDestroyer
#define tpSDestroyer
tpS::~tpS()
{

}
#endif


#ifdef dispDerivative
void MainWindow::checkBlockIds()
{
    Collect();
    qDebug("开始检查方块列表");
    fstream out("D:/check.mcfunction",ios::out);
    int offset[3]={-80,1,-80};
    //QString command="";
    if(out.is_open())qDebug("文件正常打开");else return;
    for(short r=0;r<64;r++)//x
        for(short c=0;c<9;c++)//z
        {
            if(Blocks[r][c]==NULL)continue;
            out<<"setblock ";
            out<<c+offset[2]<<' '<<offset[1]<<' '<<r+offset[0];
            out<<' '<<Data.BlockId[r][c].toLocal8Bit().data()<<'\n';
            //out<<command;
        }
    out.close();
}
void MainWindow::makeImage(int unitL)
{
    qDebug("开始makeImage");
    if(unitL<=0)unitL=16;
    MatrixXi PMat;
    PMat.setZero(4*unitL,60*unitL);
    int mapColor=0,index=0;
    QRgb CurrentColor;
    int cIndex;
    for(int Base=1;Base<61;Base++)
    {
        for(int depth=0;depth<4;depth++)
        {
            mapColor=4*Base+depth;
            index=mcMap::mapColor2Index(mapColor);
            CurrentColor=qRgb(255*Data.Basic._RGB(index,0),255*Data.Basic._RGB(index,1),255*Data.Basic._RGB(index,2));
            switch (depth)
            {
            case 0:
                cIndex=2;break;
            case 1:
                cIndex=1;break;
            case 2:
                cIndex=0;break;
            case 3:
                cIndex=3;break;
            }

            PMat.block(unitL*cIndex,unitL*(Base-1),unitL,unitL).array()=CurrentColor;

            //qDebug("rua!");
        }
    }
    qDebug("图像mat构建完毕");
    QRgb*CL=nullptr;

    //QSize size(PMat.rows(),PMat.cols());
    QSize size(PMat.cols(),PMat.rows());
    QImage Pic(size,QImage::Format_ARGB32);
    for(int r=0;r<PMat.rows();r++)
    {
        CL=(QRgb*)Pic.scanLine(r);
        for(int c=0;c<PMat.cols();c++)
        {
            CL[c]=PMat(r,c);
        }
    }
    Pic.save("D:\\240Colors.png");
}
#endif
