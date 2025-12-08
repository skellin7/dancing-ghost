#include "mainwindow.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>
#include <iostream>

void MainWindow::initialize() {
    realtime = new Realtime;
    aspectRatioWidget = new AspectRatioWidget(this);
    aspectRatioWidget->setAspectWidget(realtime, 3.f/4.f);
    QHBoxLayout *hLayout = new QHBoxLayout; // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);

    // hLayout->addLayout(vLayout);

    //create scrollable side bar
    QWidget *sideWidget = new QWidget;
    sideWidget->setLayout(vLayout);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(sideWidget);
    scrollArea->setWidgetResizable(true);

    hLayout->addWidget(scrollArea, 0);
    hLayout->addWidget(aspectRatioWidget, 1);
    this->setLayout(hLayout);

    // Create labels in sidebox
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    QLabel *tesselation_label = new QLabel(); // Parameters label
    tesselation_label->setText("Tesselation");
    tesselation_label->setFont(font);
    QLabel *camera_label = new QLabel(); // Camera label
    camera_label->setText("Camera");
    camera_label->setFont(font);
    QLabel *anim_label = new QLabel(); // Animation label
    anim_label->setText("Animation");
    anim_label->setFont(font);
    QLabel *cloth_label = new QLabel(); // Cloth label
    cloth_label->setText("Cloth");
    cloth_label->setFont(font);

    QLabel *param1_label = new QLabel(); // Parameter 1 label
    param1_label->setText("Parameter 1:");
    QLabel *param2_label = new QLabel(); // Parameter 2 label
    param2_label->setText("Parameter 2:");
    QLabel *near_label = new QLabel(); // Near plane label
    near_label->setText("Near Plane:");
    QLabel *far_label = new QLabel(); // Far plane label
    far_label->setText("Far Plane:");

    rwristCB = new QRadioButton();
    rwristCB->setText(QStringLiteral("Right Wrist"));
    rwristCB->setChecked(true);

    lwristCB = new QRadioButton();
    lwristCB->setText(QStringLiteral("Left Wrist"));

    rankleCB = new QRadioButton();
    rankleCB->setText(QStringLiteral("Right Ankle"));

    lankleCB = new QRadioButton();
    lankleCB->setText(QStringLiteral("Left Ankle"));

    renderNormals = new QRadioButton();
    renderNormals->setText(QStringLiteral("render as normals"));
    renderNormals->setChecked(true);

    renderVertices = new QRadioButton();
    renderVertices->setText(QStringLiteral("render as vertices"));

    QLabel *x_label = new QLabel(); // cloth bottom left x pos label
    x_label->setText("x pos:");
    QLabel *y_label = new QLabel(); // cloth bottom left y pos label
    y_label->setText("y pos:");
    QLabel *z_label = new QLabel(); // cloth bottom left z pos label
    z_label->setText("z pos:");

    QLabel *structural_label = new QLabel(); // structural spring k constant
    structural_label->setText("structural k:");
    QLabel *shear_label = new QLabel(); // shear spring k constant
    shear_label->setText("shear k:");
    QLabel *bend_label = new QLabel(); // bend spring k constant
    bend_label->setText("bend k:");
    QLabel *damping_label = new QLabel(); // damping constant
    damping_label->setText("damping k:");

    QLabel *cloth_width_label = new QLabel(); // cloth width
    cloth_width_label->setText("cloth width:");
    QLabel *cloth_width_step_label = new QLabel(); // cloth width step
    cloth_width_step_label->setText("cloth width step:");
    QLabel *cloth_length_label = new QLabel(); // cloth length
    cloth_length_label->setText("cloth length:");
    QLabel *cloth_length_step_label = new QLabel(); // cloth length step
    cloth_length_step_label->setText("cloth length step:");

    QLabel *static_friction_label = new QLabel(); //static friction
    static_friction_label->setText("static friction:");
    QLabel *kinetic_friction_label = new QLabel(); //kinetic friction
    kinetic_friction_label->setText("kinetic friction");

    QLabel *cloth_to_shape_correction_label = new QLabel(); //repulsion epsilon
    cloth_to_shape_correction_label->setText("cloth to shape collision (repulsion) correction");

    QLabel *vertex_radius_label = new QLabel(); //vertex radius
    vertex_radius_label->setText("cloth vertex radius");

    QLabel *cloth_to_cloth_correction_label = new QLabel(); //repulsion epsilon
    cloth_to_cloth_correction_label->setText("cloth to cloth collision (repulsion) correction");

    // Create file uploader for scene file
    uploadFile = new QPushButton();
    uploadFile->setText(QStringLiteral("Upload Scene File"));
    
    saveImage = new QPushButton();
    saveImage->setText(QStringLiteral("Save Image"));

    // Creates the boxes containing the parameter sliders and number boxes
    QGroupBox *p1Layout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *l1 = new QHBoxLayout();
    QGroupBox *p2Layout = new QGroupBox(); // horizonal slider 2 alignment
    QHBoxLayout *l2 = new QHBoxLayout();

    // Create slider controls to control parameters
    p1Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    p1Slider->setTickInterval(1);
    p1Slider->setMinimum(1);
    p1Slider->setMaximum(25);
    p1Slider->setValue(1);

    p1Box = new QSpinBox();
    p1Box->setMinimum(1);
    p1Box->setMaximum(25);
    p1Box->setSingleStep(1);
    p1Box->setValue(1);

    p2Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 2 slider
    p2Slider->setTickInterval(1);
    p2Slider->setMinimum(1);
    p2Slider->setMaximum(25);
    p2Slider->setValue(1);

    p2Box = new QSpinBox();
    p2Box->setMinimum(1);
    p2Box->setMaximum(25);
    p2Box->setSingleStep(1);
    p2Box->setValue(1);

    // Adds the slider and number box to the parameter layouts
    l1->addWidget(p1Slider);
    l1->addWidget(p1Box);
    p1Layout->setLayout(l1);

    l2->addWidget(p2Slider);
    l2->addWidget(p2Box);
    p2Layout->setLayout(l2);

    // Creates the boxes containing the camera sliders and number boxes
    QGroupBox *nearLayout = new QGroupBox(); // horizonal near slider alignment
    QHBoxLayout *lnear = new QHBoxLayout();
    QGroupBox *farLayout = new QGroupBox(); // horizonal far slider alignment
    QHBoxLayout *lfar = new QHBoxLayout();

    // Create slider controls to control near/far planes
    nearSlider = new QSlider(Qt::Orientation::Horizontal); // Near plane slider
    nearSlider->setTickInterval(1);
    nearSlider->setMinimum(1);
    nearSlider->setMaximum(1000);
    nearSlider->setValue(10);

    nearBox = new QDoubleSpinBox();
    nearBox->setMinimum(0.01f);
    nearBox->setMaximum(10.f);
    nearBox->setSingleStep(0.1f);
    nearBox->setValue(0.1f);

    farSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
    farSlider->setTickInterval(1);
    farSlider->setMinimum(1000);
    farSlider->setMaximum(10000);
    farSlider->setValue(10000);

    farBox = new QDoubleSpinBox();
    farBox->setMinimum(10.f);
    farBox->setMaximum(100.f);
    farBox->setSingleStep(0.1f);
    farBox->setValue(100.f);

    // Adds the slider and number box to the parameter layouts
    lnear->addWidget(nearSlider);
    lnear->addWidget(nearBox);
    nearLayout->setLayout(lnear);

    lfar->addWidget(farSlider);
    lfar->addWidget(farBox);
    farLayout->setLayout(lfar);

    // Creates the boxes contgaining the xyz position of cloth anchor (bottom left)
    QGroupBox *xLayout = new QGroupBox();
    QHBoxLayout *lx = new QHBoxLayout();
    QGroupBox *yLayout = new QGroupBox();
    QHBoxLayout *ly = new QHBoxLayout();
    QGroupBox *zLayout = new QGroupBox();
    QHBoxLayout *lz = new QHBoxLayout();

    // Create slider controls to control xyz position of cloth anchor (bottom left)
    xSlider = new QSlider(Qt::Orientation::Horizontal);
    xSlider->setTickInterval(1);
    xSlider->setMinimum(-1000);
    xSlider->setMaximum(1000);
    xSlider->setValue(10);

    xBox = new QDoubleSpinBox();
    xBox->setMinimum(-10.f);
    xBox->setMaximum(10.f);
    xBox->setSingleStep(0.1f);
    xBox->setValue(0.1f);

    ySlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
    ySlider->setTickInterval(1);
    ySlider->setMinimum(-1000);
    ySlider->setMaximum(1000);
    ySlider->setValue(10);

    yBox = new QDoubleSpinBox();
    yBox->setMinimum(-10.f);
    yBox->setMaximum(10.f);
    yBox->setSingleStep(0.1f);
    yBox->setValue(0.1f);

    zSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
    zSlider->setTickInterval(1);
    zSlider->setMinimum(-1000);
    zSlider->setMaximum(1000);
    zSlider->setValue(10);

    zBox = new QDoubleSpinBox();
    zBox->setMinimum(-10.f);
    zBox->setMaximum(10.f);
    zBox->setSingleStep(0.1f);
    zBox->setValue(0.1f);

    // Adds the slider and number box to the parameter layouts
    lx->addWidget(xSlider);
    lx->addWidget(xBox);
    xLayout->setLayout(lx);

    ly->addWidget(ySlider);
    ly->addWidget(yBox);
    yLayout->setLayout(ly);

    lz->addWidget(zSlider);
    lz->addWidget(zBox);
    zLayout->setLayout(lz);

    // Creates the boxes containing the spring k constants and damping constant
    QGroupBox *structuralLayout = new QGroupBox();
    QHBoxLayout *lstructural = new QHBoxLayout();
    QGroupBox *shearLayout = new QGroupBox();
    QHBoxLayout *lshear = new QHBoxLayout();
    QGroupBox *bendLayout = new QGroupBox();
    QHBoxLayout *lbend = new QHBoxLayout();
    QGroupBox *dampingLayout = new QGroupBox();
    QHBoxLayout *ldamping = new QHBoxLayout();

    // Create slider controls to control structural, shear, bend, and damping k constants
    structuralSlider = new QSlider(Qt::Orientation::Horizontal);
    structuralSlider->setTickInterval(1);
    structuralSlider->setMinimum(1);
    structuralSlider->setMaximum(20000);
    structuralSlider->setValue(10);

    structuralBox = new QDoubleSpinBox();
    structuralBox->setMinimum(0.01f);
    structuralBox->setMaximum(200.f);
    structuralBox->setSingleStep(1.f);
    structuralBox->setValue(100.f);

    shearSlider = new QSlider(Qt::Orientation::Horizontal);
    shearSlider->setTickInterval(1);
    shearSlider->setMinimum(1);
    shearSlider->setMaximum(10000);
    shearSlider->setValue(10);

    shearBox = new QDoubleSpinBox();
    shearBox->setMinimum(0.01f);
    shearBox->setMaximum(100.f);
    shearBox->setSingleStep(1.f);
    shearBox->setValue(50.f);

    bendSlider = new QSlider(Qt::Orientation::Horizontal);
    bendSlider->setTickInterval(1);
    bendSlider->setMinimum(1);
    bendSlider->setMaximum(10000);
    bendSlider->setValue(10);

    bendBox = new QDoubleSpinBox();
    bendBox->setMinimum(0.01f);
    bendBox->setMaximum(50.f);
    bendBox->setSingleStep(1.f);
    bendBox->setValue(10.f);

    dampingSlider = new QSlider(Qt::Orientation::Horizontal);
    dampingSlider->setTickInterval(1);
    dampingSlider->setMinimum(1);
    dampingSlider->setMaximum(10000);
    dampingSlider->setValue(10);

    dampingBox = new QDoubleSpinBox();
    dampingBox->setMinimum(0.01f);
    dampingBox->setMaximum(50.f);
    dampingBox->setSingleStep(1.f);
    dampingBox->setValue(10.f);

    // Adds the slider and number box to the parameter layouts
    lstructural->addWidget(structuralSlider);
    lstructural->addWidget(structuralBox);
    structuralLayout->setLayout(lstructural);

    lshear->addWidget(shearSlider);
    lshear->addWidget(shearBox);
    shearLayout->setLayout(lshear);

    lbend->addWidget(bendSlider);
    lbend->addWidget(bendBox);
    bendLayout->setLayout(lbend);

    ldamping->addWidget(dampingSlider);
    ldamping->addWidget(dampingBox);
    dampingLayout->setLayout(ldamping);

    // Creates the boxes containing the cloth width, cloth width step, cloth length, cloth length step
    QGroupBox *clothWidthLayout = new QGroupBox();
    QHBoxLayout *lclothWidth = new QHBoxLayout();
    QGroupBox *clothWidthStepLayout = new QGroupBox();
    QHBoxLayout *lclothWidthStep = new QHBoxLayout();
    QGroupBox *clothLengthLayout = new QGroupBox();
    QHBoxLayout *lclothLength = new QHBoxLayout();
    QGroupBox *clothLengthStepLayout = new QGroupBox();
    QHBoxLayout *lclothLengthStep = new QHBoxLayout();

    // Create slider controls to control cloth width, cloth width step, cloth length, cloth length step
    clothWidthSlider = new QSlider(Qt::Orientation::Horizontal);
    clothWidthSlider->setTickInterval(1);
    clothWidthSlider->setMinimum(1);
    clothWidthSlider->setMaximum(1000);
    clothWidthSlider->setValue(10);

    clothWidthBox = new QDoubleSpinBox();
    clothWidthBox->setMinimum(0.01f);
    clothWidthBox->setMaximum(10.f);
    clothWidthBox->setSingleStep(0.1f);
    clothWidthBox->setValue(0.1f);

    clothWidthStepSlider = new QSlider(Qt::Orientation::Horizontal);
    clothWidthStepSlider->setTickInterval(1);
    clothWidthStepSlider->setMinimum(1);
    clothWidthStepSlider->setMaximum(1000);
    clothWidthStepSlider->setValue(10);

    clothWidthStepBox = new QDoubleSpinBox();
    clothWidthStepBox->setMinimum(0.01f);
    clothWidthStepBox->setMaximum(10.f);
    clothWidthStepBox->setSingleStep(0.1f);
    clothWidthStepBox->setValue(0.1f);

    clothLengthSlider = new QSlider(Qt::Orientation::Horizontal);
    clothLengthSlider->setTickInterval(1);
    clothLengthSlider->setMinimum(1);
    clothLengthSlider->setMaximum(1000);
    clothLengthSlider->setValue(10);

    clothLengthBox = new QDoubleSpinBox();
    clothLengthBox->setMinimum(0.01f);
    clothLengthBox->setMaximum(10.f);
    clothLengthBox->setSingleStep(0.1f);
    clothLengthBox->setValue(0.1f);

    clothLengthStepSlider = new QSlider(Qt::Orientation::Horizontal);
    clothLengthStepSlider->setTickInterval(1);
    clothLengthStepSlider->setMinimum(1);
    clothLengthStepSlider->setMaximum(1000);
    clothLengthStepSlider->setValue(10);

    clothLengthStepBox = new QDoubleSpinBox();
    clothLengthStepBox->setMinimum(0.01f);
    clothLengthStepBox->setMaximum(10.f);
    clothLengthStepBox->setSingleStep(0.1f);
    clothLengthStepBox->setValue(0.1f);

    // Adds the slider and number box to the parameter layouts
    lclothWidth->addWidget(clothWidthSlider);
    lclothWidth->addWidget(clothWidthBox);
    clothWidthLayout->setLayout(lclothWidth);

    lclothWidthStep->addWidget(clothWidthStepSlider);
    lclothWidthStep->addWidget(clothWidthStepBox);
    clothWidthStepLayout->setLayout(lclothWidthStep);

    lclothLength->addWidget(clothLengthSlider);
    lclothLength->addWidget(clothLengthBox);
    clothLengthLayout->setLayout(lclothLength);

    lclothLengthStep->addWidget(clothLengthStepSlider);
    lclothLengthStep->addWidget(clothLengthStepBox);
    clothLengthStepLayout->setLayout(lclothLengthStep);

    // Creates the boxes containing static friction and kinetic friction
    QGroupBox *staticFrictionLayout = new QGroupBox();
    QHBoxLayout *lstaticFriction= new QHBoxLayout();
    QGroupBox *kineticFrictionLayout = new QGroupBox();
    QHBoxLayout *lkineticFriction = new QHBoxLayout();

    // Create slider controls to control static friction and kinetic friction
    staticFrictionSlider = new QSlider(Qt::Orientation::Horizontal);
    staticFrictionSlider->setTickInterval(10);
    staticFrictionSlider->setMinimum(0);
    staticFrictionSlider->setMaximum(1000);
    staticFrictionSlider->setValue(5);

    staticFrictionBox = new QDoubleSpinBox();
    //staticFrictionBox->setDecimals(2);
    staticFrictionBox->setMinimum(0.00f);
    staticFrictionBox->setMaximum(10.f);
    staticFrictionBox->setSingleStep(0.01f);
    staticFrictionBox->setValue(0.1f);

    kineticFrictionSlider = new QSlider(Qt::Orientation::Horizontal);
    kineticFrictionSlider->setTickInterval(10);
    kineticFrictionSlider->setMinimum(0);
    kineticFrictionSlider->setMaximum(1000);
    kineticFrictionSlider->setValue(5);

    kineticFrictionBox = new QDoubleSpinBox();
    //kineticFrictionBox->setDecimals(2);
    kineticFrictionBox->setMinimum(0.00f);
    kineticFrictionBox->setMaximum(10.f);
    kineticFrictionBox->setSingleStep(0.01f);
    kineticFrictionBox->setValue(0.1f);

    // Adds the slider and number box to the parameter layouts
    lstaticFriction->addWidget(staticFrictionSlider);
    lstaticFriction->addWidget(staticFrictionBox);
    staticFrictionLayout->setLayout(lstaticFriction);

    lkineticFriction->addWidget(kineticFrictionSlider);
    lkineticFriction->addWidget(kineticFrictionBox);
    kineticFrictionLayout->setLayout(lkineticFriction);

    // Creates the boxes containing repulsion epsilon
    QGroupBox *clothToShapeCorrectionLayout = new QGroupBox();
    QHBoxLayout *lclothToShapeCorrection= new QHBoxLayout();

    // Create slider controls to control repulsion epsilon
    clothToShapeCorrectionSlider = new QSlider(Qt::Orientation::Horizontal);
    clothToShapeCorrectionSlider->setTickInterval(1);
    clothToShapeCorrectionSlider->setMinimum(0);
    clothToShapeCorrectionSlider->setMaximum(10000);
    clothToShapeCorrectionSlider->setValue(100);

    clothToShapeCorrectionBox = new QDoubleSpinBox();
    clothToShapeCorrectionBox->setDecimals(4);
    clothToShapeCorrectionBox->setMinimum(0.0001f);
    clothToShapeCorrectionBox->setMaximum(1.f);
    clothToShapeCorrectionBox->setSingleStep(0.0001f);
    clothToShapeCorrectionBox->setValue(0.01f);

    // Adds the slider and number box to the parameter layouts
    lclothToShapeCorrection->addWidget(clothToShapeCorrectionSlider);
    lclothToShapeCorrection->addWidget(clothToShapeCorrectionBox);
    clothToShapeCorrectionLayout->setLayout(lclothToShapeCorrection);

    // Creates the boxes containing vertex radius
    QGroupBox *vertexRadiusLayout = new QGroupBox();
    QHBoxLayout *lvertexRadius = new QHBoxLayout();

    // Create slider controls to control vertex radius
    vertexRadiusSlider = new QSlider(Qt::Orientation::Horizontal);
    vertexRadiusSlider->setTickInterval(1);
    vertexRadiusSlider->setMinimum(0);
    vertexRadiusSlider->setMaximum(10000);
    vertexRadiusSlider->setValue(100);

    vertexRadiusBox = new QDoubleSpinBox();
    vertexRadiusBox->setDecimals(4);
    vertexRadiusBox->setMinimum(0.0001f);
    vertexRadiusBox->setMaximum(1.f);
    vertexRadiusBox->setSingleStep(0.0001f);
    vertexRadiusBox->setValue(0.01f);

    // Adds the slider and number box to the parameter layouts
    lvertexRadius->addWidget(vertexRadiusSlider);
    lvertexRadius->addWidget(vertexRadiusBox);
    vertexRadiusLayout->setLayout(lvertexRadius);

    // Creates the boxes containing repulsion epsilon
    QGroupBox *clothToClothCorrectionLayout = new QGroupBox();
    QHBoxLayout *lclothToClothCorrection= new QHBoxLayout();

    // Create slider controls to control repulsion epsilon
    clothToClothCorrectionSlider = new QSlider(Qt::Orientation::Horizontal);
    clothToClothCorrectionSlider->setTickInterval(1);
    clothToClothCorrectionSlider->setMinimum(0);
    clothToClothCorrectionSlider->setMaximum(10000);
    clothToClothCorrectionSlider->setValue(100);

    clothToClothCorrectionBox = new QDoubleSpinBox();
    clothToClothCorrectionBox->setDecimals(4);
    clothToClothCorrectionBox->setMinimum(0.0001f);
    clothToClothCorrectionBox->setMaximum(1.f);
    clothToClothCorrectionBox->setSingleStep(0.0001f);
    clothToClothCorrectionBox->setValue(0.01f);

    // Adds the slider and number box to the parameter layouts
    lclothToClothCorrection->addWidget(clothToClothCorrectionSlider);
    lclothToClothCorrection->addWidget(clothToClothCorrectionBox);
    clothToClothCorrectionLayout->setLayout(lclothToClothCorrection);

    vLayout->addWidget(uploadFile);
    vLayout->addWidget(saveImage);
    // vLayout->addWidget(tesselation_label);
    // vLayout->addWidget(param1_label);
    // vLayout->addWidget(p1Layout);
    // vLayout->addWidget(param2_label);
    // vLayout->addWidget(p2Layout);
    // vLayout->addWidget(camera_label);
    // vLayout->addWidget(near_label);
    // vLayout->addWidget(nearLayout);
    // vLayout->addWidget(far_label);
    // vLayout->addWidget(farLayout);

    vLayout->addWidget(anim_label);
    vLayout->addWidget(rwristCB);
    vLayout->addWidget(lwristCB);
    vLayout->addWidget(rankleCB);
    vLayout->addWidget(lankleCB);

    vLayout->addWidget(cloth_label);
    vLayout->addWidget(renderNormals);
    vLayout->addWidget(renderVertices);

    vLayout->addWidget(x_label);
    vLayout->addWidget(xLayout);
    vLayout->addWidget(y_label);
    vLayout->addWidget(yLayout);
    vLayout->addWidget(z_label);
    vLayout->addWidget(zLayout);

    vLayout->addWidget(structural_label);
    vLayout->addWidget(structuralLayout);
    vLayout->addWidget(shear_label);
    vLayout->addWidget(shearLayout);
    vLayout->addWidget(bend_label);
    vLayout->addWidget(bendLayout);
    vLayout->addWidget(damping_label);
    vLayout->addWidget(dampingLayout);

    vLayout->addWidget(cloth_width_label);
    vLayout->addWidget(clothWidthLayout);
    vLayout->addWidget(cloth_width_step_label);
    vLayout->addWidget(clothWidthStepLayout);
    vLayout->addWidget(cloth_length_label);
    vLayout->addWidget(clothLengthLayout);
    vLayout->addWidget(cloth_length_step_label);
    vLayout->addWidget(clothLengthStepLayout);

    vLayout->addWidget(static_friction_label);
    vLayout->addWidget(staticFrictionLayout);
    vLayout->addWidget(kinetic_friction_label);
    vLayout->addWidget(kineticFrictionLayout);

    vLayout->addWidget(cloth_to_shape_correction_label);
    vLayout->addWidget(clothToShapeCorrectionLayout);

    vLayout->addWidget(vertex_radius_label);
    vLayout->addWidget(vertexRadiusLayout);

    vLayout->addWidget(cloth_to_cloth_correction_label);
    vLayout->addWidget(clothToClothCorrectionLayout);

    connectUIElements();

    // Set default values of 5 for tesselation parameters
    onValChangeP1(25);
    onValChangeP2(25);

    // Set default values for near and far planes
    onValChangeNearBox(0.1f);
    // onValChangeFarBox(10.f);
    onValChangeFarBox(100.f);

    // Set default values for xyz position of cloth anchor (bottom left)
    onValChangexBox(-1.f);
    onValChangeyBox(2.2f);
    onValChangezBox(-1.f);
    // onValChangexBox(-1.33f);
    // onValChangeyBox(1.f);
    // onValChangezBox(-1.67f);

    // Set default values for structural, shear, bend, and damping k constants
    onValChangeStructuralBox(150.f);
    onValChangeShearBox(80.f);
    onValChangeBendBox(20.f);
    onValChangeDampingBox(10.f);

    onValChangeClothWidthBox(2.f);
    // onValChangeClothWidthStepBox(.3f);
    onValChangeClothWidthStepBox(.2f);
    onValChangeClothLengthBox(2.f);
    // onValChangeClothLengthStepBox(.3f);
    onValChangeClothLengthStepBox(.2f);
    // onValChangeClothWidthBox(2.43f);
    // onValChangeClothWidthStepBox(.2f);
    // onValChangeClothLengthBox(3.79f);
    // onValChangeClothLengthStepBox(.2f);

    onValChangeStaticFrictionBox(0.5f);
    onValChangeKineticFrictionBox(0.3f);

    onValChangeClothToShapeCorrectionBox(0.063f);
    onValChangeVertexRadiusBox(0.01f);
    onValChangeClothToClothCorrectionBox(.0001f);

}

void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {
    connectUploadFile();
    connectSaveImage();
    connectParam1();
    connectParam2();
    connectNear();
    connectFar();
    connectRWrist();
    connectLWrist();
    connectRAnkle();
    connectLAnkle();
    connectx();
    connecty();
    connectz();
    connectStructural();
    connectShear();
    connectBend();
    connectDamping();
    connectClothWidth();
    connectClothWidthStep();
    connectClothLength();
    connectClothLengthStep();
    connectStaticFriction();
    connectKineticFriction();
    connectClothToShapeCorrection();
    connectVertexRadius();
    connectClothToClothCorrection();
    connectRenderNormals();
    connectRenderVertices();
}


void MainWindow::connectUploadFile() {
    connect(uploadFile, &QPushButton::clicked, this, &MainWindow::onUploadFile);
}

void MainWindow::connectSaveImage() {
    connect(saveImage, &QPushButton::clicked, this, &MainWindow::onSaveImage);
}

void MainWindow::connectParam1() {
    connect(p1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP1);
    connect(p1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP1);
}

void MainWindow::connectParam2() {
    connect(p2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP2);
    connect(p2Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP2);
}

void MainWindow::connectNear() {
    connect(nearSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNearSlider);
    connect(nearBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeNearBox);
}

void MainWindow::connectFar() {
    connect(farSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeFarSlider);
    connect(farBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeFarBox);
}

void MainWindow::connectx() {
    connect(xSlider, &QSlider::valueChanged, this, &MainWindow::onValChangexSlider);
    connect(xBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangexBox);
}

void MainWindow::connecty() {
    connect(ySlider, &QSlider::valueChanged, this, &MainWindow::onValChangeySlider);
    connect(yBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeyBox);
}

void MainWindow::connectz() {
    connect(zSlider, &QSlider::valueChanged, this, &MainWindow::onValChangezSlider);
    connect(zBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangezBox);
}

void MainWindow::connectStructural() {
    connect(structuralSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeStructuralSlider);
    connect(structuralBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeStructuralBox);
}

void MainWindow::connectShear() {
    connect(shearSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeShearSlider);
    connect(shearBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeShearBox);
}

void MainWindow::connectBend() {
    connect(bendSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeBendSlider);
    connect(bendBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeBendBox);
}

void MainWindow::connectDamping() {
    connect(dampingSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeDampingSlider);
    connect(dampingBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeDampingBox);
}

void MainWindow::connectClothWidth() {
    connect(clothWidthSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeClothWidthSlider);
    connect(clothWidthBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeClothWidthBox);
}

void MainWindow::connectClothWidthStep() {
    connect(clothWidthStepSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeClothWidthStepSlider);
    connect(clothWidthStepBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeClothWidthStepBox);
}

void MainWindow::connectClothLength() {
    connect(clothLengthSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeClothLengthSlider);
    connect(clothLengthBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeClothLengthBox);
}

void MainWindow::connectClothLengthStep() {
    connect(clothLengthStepSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeClothLengthStepSlider);
    connect(clothLengthStepBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeClothLengthStepBox);
}

void MainWindow::connectStaticFriction() {
    connect(staticFrictionSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeStaticFrictionSlider);
    connect(staticFrictionBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeStaticFrictionBox);
}

void MainWindow::connectKineticFriction() {
    connect(kineticFrictionSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeKineticFrictionSlider);
    connect(kineticFrictionBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeKineticFrictionBox);
}

void MainWindow::connectClothToShapeCorrection() {
    connect(clothToShapeCorrectionSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeClothToShapeCorrectionSlider);
    connect(clothToShapeCorrectionBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeClothToShapeCorrectionBox);
}

void MainWindow::connectVertexRadius() {
    connect(vertexRadiusSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeVertexRadiusSlider);
    connect(vertexRadiusBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeVertexRadiusBox);
}

void MainWindow::connectClothToClothCorrection() {
    connect(clothToClothCorrectionSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeClothToClothCorrectionSlider);
    connect(clothToClothCorrectionBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeClothToClothCorrectionBox);
}


void MainWindow::onUploadFile() {
    // Get abs path of scene file
    QString configFilePath = QFileDialog::getOpenFileName(this, tr("Upload File"),
                                                          QDir::currentPath()
                                                              .append(QDir::separator())
                                                              .append("scenefiles")
                                                              .append(QDir::separator())
                                                              .append("realtime")
                                                              .append(QDir::separator())
                                                              .append("required"), tr("Scene Files (*.json)"));
    if (configFilePath.isNull()) {
        std::cout << "Failed to load null scenefile." << std::endl;
        return;
    }

    settings.sceneFilePath = configFilePath.toStdString();

    std::cout << "Loaded scenefile: \"" << configFilePath.toStdString() << "\"." << std::endl;

    realtime->sceneChanged();
}

void MainWindow::onSaveImage() {
    if (settings.sceneFilePath.empty()) {
        std::cout << "No scene file loaded." << std::endl;
        return;
    }
    std::string sceneName = settings.sceneFilePath.substr(0, settings.sceneFilePath.find_last_of("."));
    sceneName = sceneName.substr(sceneName.find_last_of("/")+1);
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Image"),
                                                    QDir::currentPath()
                                                        .append(QDir::separator())
                                                        .append("student_outputs")
                                                        .append(QDir::separator())
                                                        .append("realtime")
                                                        .append(QDir::separator())
                                                        .append("required")
                                                        .append(QDir::separator())
                                                        .append(sceneName), tr("Image Files (*.png)"));
    std::cout << "Saving image to: \"" << filePath.toStdString() << "\"." << std::endl;
    realtime->saveViewportImage(filePath.toStdString());
}

void MainWindow::onValChangeP1(int newValue) {
    p1Slider->setValue(newValue);
    p1Box->setValue(newValue);
    settings.shapeParameter1 = p1Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeP2(int newValue) {
    p2Slider->setValue(newValue);
    p2Box->setValue(newValue);
    settings.shapeParameter2 = p2Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearSlider(int newValue) {
    //nearSlider->setValue(newValue);
    nearBox->setValue(newValue/100.f);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarSlider(int newValue) {
    //farSlider->setValue(newValue);
    farBox->setValue(newValue/100.f);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::connectRWrist()
{
    connect(rwristCB, &QRadioButton::clicked, this, &MainWindow::onRWristChange);
}

void MainWindow::onRWristChange()
{
    settings.endjoint = EndJoint::RWRIST;
    realtime->settingsChanged();
}

void MainWindow::connectLWrist()
{
    connect(lwristCB, &QRadioButton::clicked, this, &MainWindow::onLWristChange);
}

void MainWindow::onLWristChange()
{
    settings.endjoint = EndJoint::LWRIST;
    realtime->settingsChanged();
}

void MainWindow::connectRAnkle()
{
    connect(rankleCB, &QRadioButton::clicked, this, &MainWindow::onRAnkleChange);
}

void MainWindow::onRAnkleChange()
{
    settings.endjoint = EndJoint::RANKLE;
    realtime->settingsChanged();
}

void MainWindow::connectLAnkle()
{
    connect(lankleCB, &QRadioButton::clicked, this, &MainWindow::onLAnkleChange);
}

void MainWindow::onLAnkleChange()
{
    settings.endjoint = EndJoint::LANKLE;
    realtime->settingsChanged();
}

void MainWindow::connectRenderNormals()
{
    connect(renderNormals, &QRadioButton::clicked, this, &MainWindow::onRenderNormalsChange);
}

void MainWindow::onRenderNormalsChange()
{
    settings.renderVertices = false;
    realtime->settingsChanged();
}

void MainWindow::connectRenderVertices()
{
    connect(renderVertices, &QRadioButton::clicked, this, &MainWindow::onRenderVerticesChange);
}

void MainWindow::onRenderVerticesChange()
{
    settings.renderVertices = true;
    realtime->settingsChanged();
}

void MainWindow::onValChangexSlider(int newValue) {
    xBox->setValue(newValue/100.f);
    settings.x_clothBottomLeft = xBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeySlider(int newValue) {
    yBox->setValue(newValue/100.f);
    settings.y_clothBottomLeft = yBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangezSlider(int newValue) {
    zBox->setValue(newValue/100.f);
    settings.z_clothBottomLeft = zBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeStructuralSlider(int newValue) {
    structuralBox->setValue(newValue/100.f);
    settings.structuralK = structuralBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeShearSlider(int newValue) {
    shearBox->setValue(newValue/100.f);
    settings.shearK = shearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeBendSlider(int newValue) {
    bendBox->setValue(newValue/100.f);
    settings.bendK = bendBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeDampingSlider(int newValue) {
    dampingBox->setValue(newValue/100.f);
    settings.damping = dampingBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeClothWidthSlider(int newValue) {
    clothWidthBox->setValue(newValue/100.f);
    settings.cloth_width = clothWidthBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeClothWidthStepSlider(int newValue) {
    clothWidthStepBox->setValue(newValue/100.f);
    settings.cloth_width_step = clothWidthStepBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeClothLengthSlider(int newValue) {
    clothLengthBox->setValue(newValue/100.f);
    settings.cloth_length = clothLengthBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeClothLengthStepSlider(int newValue) {
    clothLengthStepBox->setValue(newValue/100.f);
    settings.cloth_length_step = clothLengthStepBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeStaticFrictionSlider(int newValue) {
    staticFrictionBox->setValue(newValue/100.f);
    settings.mu_static = staticFrictionBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeKineticFrictionSlider(int newValue) {
    kineticFrictionBox->setValue(newValue/100.f);
    settings.mu_kinetic = kineticFrictionBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeClothToShapeCorrectionSlider(int newValue) {
    clothToShapeCorrectionBox->setValue(newValue/10000.f);
    settings.clothToShapeCollisionCorrection = clothToShapeCorrectionBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeVertexRadiusSlider(int newValue) {
    vertexRadiusBox->setValue(newValue/10000.f);
    settings.clothVertexRadius = vertexRadiusBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeClothToClothCorrectionSlider(int newValue) {
    clothToClothCorrectionBox->setValue(newValue/10000.f);
    settings.clothToShapeCollisionCorrection = clothToClothCorrectionBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearBox(double newValue) {
    nearSlider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarBox(double newValue) {
    farSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangexBox(double newValue) {
    xSlider->setValue(int(newValue*100.f));
    settings.x_clothBottomLeft = xBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeyBox(double newValue) {
    ySlider->setValue(int(newValue*100.f));
    settings.y_clothBottomLeft = yBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangezBox(double newValue) {
    zSlider->setValue(int(newValue*100.f));
    settings.z_clothBottomLeft = zBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeStructuralBox(double newValue) {
    structuralSlider->setValue(int(newValue*100.f));
    settings.structuralK = structuralBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeShearBox(double newValue) {
    shearSlider->setValue(int(newValue*100.f));
    settings.shearK = shearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeBendBox(double newValue) {
    bendSlider->setValue(int(newValue*100.f));
    settings.bendK = bendBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeDampingBox(double newValue) {
    dampingSlider->setValue(int(newValue*100.f));
    settings.damping = dampingBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeClothWidthBox(double newValue) {
    clothWidthSlider->setValue(int(newValue*100.f));
    settings.cloth_width = clothWidthBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeClothWidthStepBox(double newValue) {
    clothWidthStepSlider->setValue(int(newValue*100.f));
    settings.cloth_width_step = clothWidthStepBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeClothLengthBox(double newValue) {
    clothLengthSlider->setValue(int(newValue*100.f));
    settings.cloth_length = clothLengthBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeClothLengthStepBox(double newValue) {
    clothLengthStepSlider->setValue(int(newValue*100.f));
    settings.cloth_length_step = clothLengthStepBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeStaticFrictionBox(double newValue) {
    staticFrictionSlider->setValue(int(newValue*100.f));
    settings.mu_static = staticFrictionBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeKineticFrictionBox(double newValue) {
    kineticFrictionSlider->setValue(int(newValue*100.f));
    settings.mu_kinetic = kineticFrictionBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeClothToShapeCorrectionBox(double newValue) {
    clothToShapeCorrectionSlider->setValue(int(newValue*10000.f));
    settings.clothToShapeCollisionCorrection = clothToShapeCorrectionBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeVertexRadiusBox(double newValue) {
    vertexRadiusSlider->setValue(int(newValue*10000.f));
    settings.clothVertexRadius = vertexRadiusBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeClothToClothCorrectionBox(double newValue) {
    clothToClothCorrectionSlider->setValue(int(newValue*10000.f));
    settings.clothToClothCollisionCorrection = clothToClothCorrectionBox->value();
    realtime->settingsChanged();
}


