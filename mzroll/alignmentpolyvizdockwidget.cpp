#include "alignmentpolyvizdockwidget.h"

AlignmentPolyVizDockWidget::AlignmentPolyVizDockWidget(MainWindow *mw) :
    QDockWidget(mw),
    ui(new Ui::AlignmentPolyVizDockWidget)
{
    this->_mw= mw;
    ui->setupUi(this);
    setObjectName("AlignmentPolyVizDockWidget");
    setWindowTitle("AlignmentPolyVizWidget");

    QToolBar *toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    QWidget* spacer1 = new QWidget();
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer1);

    QToolButton *btnResetZoom = new QToolButton(toolBar);
    btnResetZoom->setIcon(QIcon(rsrcPath + "/resetzoom.png"));
    connect(btnResetZoom, SIGNAL(clicked()), SLOT(refresh()));
    toolBar->addWidget(btnResetZoom);

    QWidget* spacer2 = new QWidget();
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer2);

    QToolButton *btnHide = new QToolButton(toolBar);
    btnHide->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    connect(btnHide, SIGNAL(clicked()),SLOT(hide()));
    toolBar->addWidget(btnHide);

    setTitleBarWidget(toolBar);
}

AlignmentPolyVizDockWidget::~AlignmentPolyVizDockWidget()
{
    delete ui;
}

void AlignmentPolyVizDockWidget::plotGraph() {

        intialSetup();

        // plot individual graphs here
        Q_FOREACH(mzSample* sample, _mw->getSamples()) {
            if(sample->isSelected) {
                plotIndividualGraph(sample);
            }
        }

        refresh();

}

void AlignmentPolyVizDockWidget::intialSetup() {

    _mw->alignmentPolyVizPlot->clearPlottables();
    setXAxis();
    setYAxis();

}

void AlignmentPolyVizDockWidget::setXAxis() {

    _mw->alignmentPolyVizPlot->xAxis->setTicks(true);
    _mw->alignmentPolyVizPlot->xAxis->setSubTicks(true);
    _mw->alignmentPolyVizPlot->xAxis->setVisible(true);
    _mw->alignmentPolyVizPlot->xAxis->setLabel("Retention Time");
}

void AlignmentPolyVizDockWidget::setYAxis() {

    _mw->alignmentPolyVizPlot->yAxis->setTicks(true);
    _mw->alignmentPolyVizPlot->yAxis->setSubTicks(true);
    _mw->alignmentPolyVizPlot->yAxis->setVisible(true);
    _mw->alignmentPolyVizPlot->yAxis->setLabel("Retention Time Deviation");

}

void AlignmentPolyVizDockWidget::plotIndividualGraph(mzSample* sample) {

    QVector<double> xAxis;
    QVector<double> yAxis;
    vector<double> coefficients;
    double degree;

    if (degreeMap.empty()) return;
    if (coefficientMap.empty()) return;

    degree = degreeMap[sample];
    coefficients = coefficientMap[sample];

    double *coe = &coefficients[0];

    for(unsigned int i=0; i < sample->scans.size(); i++ ) {
        double rt = sample->scans[i]->rt;
        xAxis.push_back(rt);

        double y = 0;

        y = leasev(coe, degree, rt);

        yAxis.push_back(y - rt);
    }

    QColor color = _mw->projectDockWidget->storeSampleColors[sample];

    QPen pen;
    pen.setColor(color);

    _mw->alignmentPolyVizPlot->addGraph();
    _mw->alignmentPolyVizPlot->graph()->setPen(pen);
    _mw->alignmentPolyVizPlot->graph()->setLineStyle(QCPGraph::lsLine);

    _mw->alignmentPolyVizPlot->graph()->setData(xAxis, yAxis);

    _mw->alignmentPolyVizPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |  QCP::iSelectPlottables);

}

void AlignmentPolyVizDockWidget::refresh() {
    _mw->alignmentPolyVizPlot->rescaleAxes();
    _mw->alignmentPolyVizPlot->replot();
}