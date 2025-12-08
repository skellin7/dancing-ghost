#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QRadioButton>
#include "realtime.h"
#include "utils/aspectratiowidget/aspectratiowidget.hpp"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void initialize();
    void finish();

private:
    void connectUIElements();
    void connectParam1();
    void connectParam2();
    void connectNear();
    void connectFar();
    void connectx();
    void connecty();
    void connectz();
    void connectStructural();
    void connectShear();
    void connectBend();
    void connectDamping();
    void connectClothWidth();
    void connectClothWidthStep();
    void connectClothLength();
    void connectClothLengthStep();
    void connectStaticFriction();
    void connectKineticFriction();
    void connectClothToShapeCorrection();
    void connectVertexRadius();
    void connectClothToClothCorrection();

    void connectRenderNormals();
    void connectRenderVertices();

    void connectUploadFile();
    void connectSaveImage();
    void connectExtraCredit();

    Realtime *realtime;
    AspectRatioWidget *aspectRatioWidget;

    QPushButton *uploadFile;
    QPushButton *saveImage;
    QSlider *p1Slider;
    QSlider *p2Slider;
    QSpinBox *p1Box;
    QSpinBox *p2Box;
    QSlider *nearSlider;
    QSlider *farSlider;
    QDoubleSpinBox *nearBox;
    QDoubleSpinBox *farBox;

    QRadioButton *renderNormals;
    QRadioButton *renderVertices;

    QSlider *xSlider;
    QSlider *ySlider;
    QSlider *zSlider;
    QDoubleSpinBox *xBox;
    QDoubleSpinBox *yBox;
    QDoubleSpinBox *zBox;

    QSlider *structuralSlider;
    QSlider *shearSlider;
    QSlider *bendSlider;
    QSlider *dampingSlider;
    QDoubleSpinBox *structuralBox;
    QDoubleSpinBox *shearBox;
    QDoubleSpinBox *bendBox;
    QDoubleSpinBox *dampingBox;

    QSlider *clothWidthSlider;
    QSlider *clothWidthStepSlider;
    QSlider *clothLengthSlider;
    QSlider *clothLengthStepSlider;
    QDoubleSpinBox *clothWidthBox;
    QDoubleSpinBox *clothWidthStepBox;
    QDoubleSpinBox *clothLengthBox;
    QDoubleSpinBox *clothLengthStepBox;

    QSlider *staticFrictionSlider;
    QSlider *kineticFrictionSlider;
    QDoubleSpinBox *staticFrictionBox;
    QDoubleSpinBox *kineticFrictionBox;

    QSlider *clothToShapeCorrectionSlider;
    QDoubleSpinBox *clothToShapeCorrectionBox;

    QSlider *vertexRadiusSlider;
    QDoubleSpinBox *vertexRadiusBox;
    QSlider *clothToClothCorrectionSlider;
    QDoubleSpinBox *clothToClothCorrectionBox;


private slots:

    void onUploadFile();
    void onSaveImage();

    void onValChangeP1(int newValue);
    void onValChangeP2(int newValue);

    void onValChangeNearSlider(int newValue);
    void onValChangeFarSlider(int newValue);

    void onRenderNormalsChange();
    void onRenderVerticesChange();

    void onValChangexSlider(int newValue);
    void onValChangeySlider(int newValue);
    void onValChangezSlider(int newValue);

    void onValChangeStructuralSlider(int newValue);
    void onValChangeShearSlider(int newValue);
    void onValChangeBendSlider(int newValue);
    void onValChangeDampingSlider(int newValue);

    void onValChangeClothWidthSlider(int newValue);
    void onValChangeClothWidthStepSlider(int newValue);
    void onValChangeClothLengthSlider(int newValue);
    void onValChangeClothLengthStepSlider(int newValue);

    void onValChangeStaticFrictionSlider(int newValue);
    void onValChangeKineticFrictionSlider(int newValue);

    void onValChangeClothToShapeCorrectionSlider(int newValue);
    void onValChangeVertexRadiusSlider(int newValue);
    void onValChangeClothToClothCorrectionSlider(int newValue);




    void onValChangeNearBox(double newValue);
    void onValChangeFarBox(double newValue);

    void onValChangexBox(double newValue);
    void onValChangeyBox(double newValue);
    void onValChangezBox(double newValue);

    void onValChangeStructuralBox(double newValue);
    void onValChangeShearBox(double newValue);
    void onValChangeBendBox(double newValue);
    void onValChangeDampingBox(double newValue);

    void onValChangeClothWidthBox(double newValue);
    void onValChangeClothWidthStepBox(double newValue);
    void onValChangeClothLengthBox(double newValue);
    void onValChangeClothLengthStepBox(double newValue);

    void onValChangeStaticFrictionBox(double newValue);
    void onValChangeKineticFrictionBox(double newValue);

    void onValChangeClothToShapeCorrectionBox(double newValue);
    void onValChangeVertexRadiusBox(double newValue);
    void onValChangeClothToClothCorrectionBox(double newValue);

};
