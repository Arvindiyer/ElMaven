#include "background_peaks_update.h"

/** @mainpage ProjectX: bla, bla, bla
*   @par Description:
*   - Point 1 bla, bla, bla
*   - Point 2 bla, bla, bla
*   - Point 3 bla, bla, bla
*
*   @par Next paragraph - more information:
*    bla, bla, bla
*    bla, bla, bla
*/

/**
 * @file background_peaks_update.cpp
 * Write description of source file here for dOxygen.
 *
 * @brief Can use "brief" tag to explicitly generate comments for file documentation.
 *
 * @author Me
   [Note: depricated practice as CM systems like Subversion will maintain author info and blame logs.]
 * @version 1.69
   [Note: depricated practice as CM systems like Subversion will maintain file history and revision numbers.]
 */



/// TODO: every file should define some comments on top of that 
/// File: ProjectXXX.suffix
/// @Author: John Doe
/// E-Mail: John.Doe@MegaCorp.com
/// Date: 26th july 2016
/// Description: Header file for Project X
///              This file contains the defined types for Project X
///              This is sometimes called the "Abstract" and may be
///              followed by a section called "Notes".


/// TODO: class name should reflect file name as you shouldn't use
/// '_' in filename. like (background_peaks_update.cpp should be backgroundPeaksUpdate.cpp).

/// TODO:  Comment on variable goes here. 
/// like that function :
/// void class::function(int para1 /**< Comment on variable goes here. */
///                       int para2 /**< Comment on variable goes here. */)
/// {

/// }

/** TODO: Write description of function here. 
    The function should follow these comments.
    The function arguments listed with "param" will be compared
    to the declaration and verified.
    @param[in]     _inArg1 Description of first function argument.
    @param[out]    _outArg2 Description of second function argument.
    @param[in,out] _inoutArg3 Description of third function argument.
    @return Description of returned value.
*/

/// TODO: Single line comment for dOxygen in inside every function.

/// TODO: add a blank line at the end of the file with at least one blank space.

BackgroundPeakUpdate::BackgroundPeakUpdate(QWidget*) {
        mainwindow = NULL;
        _stopped = true;
        //	setTerminationEnabled(false);
        runFunction = "computeKnowsPeaks";
}

BackgroundPeakUpdate::~BackgroundPeakUpdate() {
        mavenParameters->cleanup(); //remove allgroups
}

/**
 * BackgroundPeakUpdate::run This function starts the thread. This function is
 * called by start() internally in QTThread. start() function will be called
 * where the thread starts.
 */
void BackgroundPeakUpdate::run(void) {
        //Making sure that instance of the mainwindow is present so that the
        //peakdetection process can be ran
        if (mainwindow == NULL) {
                quit();
                return;
        }
        _stopped = false;

        //populating the maven setting insatnces with with the samples
        // if (mavenParameters->samples.size() == 0) {
        //         mavenParameters->samples = mainwindow->getSamples();
        // }
        //Getting the classification model
        //mavenParameters->clsf = mainwindow->getClassifier();

        //Setting the ionization mode if the user specifies the ionization mode
        //then its given the priority else the ionization mode is taken from the
        //sample
        //TODO: See how the ionization mode is effected if the user selects
        //Neutral or autodetect
        // if (mainwindow->getIonizationMode()) {
        //         mavenParameters->ionizationMode = mainwindow->getIonizationMode();
        // } else {
        //         mavenParameters->setIonizationMode();
        // }
        if (runFunction == "findPeaksQQQ") {
                findPeaksQQQ();
        } else if (runFunction == "processSlices") {
                processSlices();
        } else if (runFunction == "processMassSlices") {
                processMassSlices();
        } else if (runFunction == "pullIsotopes") {
                pullIsotopes(mavenParameters->_group);
        } else if (runFunction == "computePeaks") {
                computePeaks();
        } else {
                qDebug() << "Unknown Function " << runFunction.c_str();
        }

        quit();
        return;
}




void BackgroundPeakUpdate::writeCSVRep(string setName /*!< Detailed description after the member */) {

        //write reports
        CSVReports* csvreports = NULL;
        if (mavenParameters->writeCSVFlag) {
                string groupfilename = mavenParameters->outputdir + setName + ".csv";
                csvreports = new CSVReports(mavenParameters->samples);
                csvreports->setUserQuantType(mainwindow->getUserQuantType());
                csvreports->openGroupReport(groupfilename);
        }

        for (int j = 0; j < mavenParameters->allgroups.size(); j++) {
                PeakGroup& group = mavenParameters->allgroups[j];
                Compound* compound = group.compound;

                // TODO: Break up long Boolean expressions and parameter lists onto separate lines with appropriate indentation for alignment.
                if (mavenParameters->pullIsotopesFlag && !group.isIsotope())
                        pullIsotopes(&group);
                if (csvreports != NULL)
                        csvreports->addGroup(&group);

                if (compound) {
                        if (!compound->hasGroup()
                            || group.groupRank < compound->getPeakGroup()->groupRank)
                                compound->setPeakGroup(group);
                }

                if (mavenParameters->keepFoundGroups) {

//			cerr << "GROUPS IS  " << mavenParameters->allgroups.size() << endl;
                        emit(newPeakGroup(&(mavenParameters->allgroups[j])));
                        QCoreApplication::processEvents();
                }

                if (mavenParameters->showProgressFlag
                    && mavenParameters->pullIsotopesFlag && j % 10 == 0) {
                        emit(updateProgressBar("Calculating Isotopes", j,
                                               mavenParameters->allgroups.size()));
                }
        }

        if (csvreports != NULL) {
                csvreports->closeFiles();
                delete (csvreports);
                csvreports = NULL;
        }
        emit(updateProgressBar("Done", 1, 1));
}

void BackgroundPeakUpdate::getPullIsotopeSettings() {
        if (mainwindow) {
                QSettings* settings = mainwindow->getSettings();
                if (settings) {
                        mavenParameters->maxIsotopeScanDiff = settings->value(
                                "maxIsotopeScanDiff").toDouble();
                        mavenParameters->minIsotopicCorrelation = settings->value(
                                "minIsotopicCorrelation").toDouble();
                        mavenParameters->maxNaturalAbundanceErr = settings->value(
                                "maxNaturalAbundanceErr").toDouble();
                        mavenParameters->C13Labeled =
                                settings->value("C13Labeled").toBool();
                        mavenParameters->N15Labeled =
                                settings->value("N15Labeled").toBool();
                        mavenParameters->S34Labeled =
                                settings->value("S34Labeled").toBool();
                        mavenParameters->D2Labeled = settings->value("D2Labeled").toBool();
                        QSettings* settings = mainwindow->getSettings();
                        mavenParameters->eic_smoothingAlgorithm = settings->value(
                                "eic_smoothingAlgorithm").toInt();

                        //Feng note: assign labeling state to sample
                        mavenParameters->samples[0]->_C13Labeled =
                                mavenParameters->C13Labeled;
                        mavenParameters->samples[0]->_N15Labeled =
                                mavenParameters->N15Labeled;
                        mavenParameters->samples[0]->_S34Labeled =
                                mavenParameters->S34Labeled;
                        mavenParameters->samples[0]->_D2Labeled =
                                mavenParameters->D2Labeled;
                        //End Feng addition
                }
        }
}

void BackgroundPeakUpdate::processSlices() {
        processSlices(mavenParameters->_slices, "sliceset");
}

void BackgroundPeakUpdate::processSlice(mzSlice& slice) {
        vector<mzSlice*> slices;
        slices.push_back(&slice);
        processSlices(slices, "sliceset");
}

//TODO: kiran Make a function which tell that its from option
//window and should be called where the settings are been called
void BackgroundPeakUpdate::getProcessSlicesSettings() {
        QSettings* settings = mainwindow->getSettings();
        mavenParameters->amuQ1 = settings->value("amuQ1").toDouble();
        mavenParameters->amuQ3 = settings->value("amuQ3").toDouble();
        // To Do: Are these lines required. The same is already being done in PeakDetectionDialog.cpp
        // mavenParameters->baseline_smoothingWindow = settings->value(
        //         "baseline_smoothing").toInt();
        // mavenParameters->baseline_dropTopX =
        //         settings->value("baseline_quantile").toInt();

}

void BackgroundPeakUpdate::align() {
        if (mavenParameters->alignSamplesFlag) {
                //		emit(updateProgressBar("Aligning Samples", 1, 100));
                vector<PeakGroup*> groups(mavenParameters->allgroups.size());
                for (int i = 0; i < mavenParameters->allgroups.size(); i++)
                        groups[i] = &mavenParameters->allgroups[i];
                Aligner aligner;
                aligner.setMaxItterations(
                        mainwindow->alignmentDialog->maxItterations->value());
                aligner.setPolymialDegree(
                        mainwindow->alignmentDialog->polynomialDegree->value());
                aligner.doAlignment(groups);
        }
}

void BackgroundPeakUpdate::processSlices(vector<mzSlice*>&slices,
                                         string setName) {

        getProcessSlicesSettings();
        peakDetector.processSlices(slices, setName);

        align();

        if (mavenParameters->showProgressFlag
            && mavenParameters->pullIsotopesFlag) {
                emit(updateProgressBar("Calculation Isotopes", 1, 100));
        }

        writeCSVRep(setName);
}

void BackgroundPeakUpdate::processCompounds(vector<Compound*> set,
                                            string setName) {

        if (set.size() == 0)
                return;

        vector<mzSlice*> slices = peakDetector.processCompounds(set, setName);
        processSlices(slices, setName);
        delete_all(slices);
}

void BackgroundPeakUpdate::processMassSlices() {
        emit (updateProgressBar("Computing Mass Slices", 0, 10));
        peakDetector.processMassSlices();
        //cerr << "BPU IS " << mavenParameters->allgroups.size() << endl;

        align();

        if (mavenParameters->showProgressFlag
            && mavenParameters->pullIsotopesFlag) {
                emit(updateProgressBar("Calculation Isotopes", 1, 100));
        }

        writeCSVRep("allslices");
}

void BackgroundPeakUpdate::computePeaks() {
        if (mavenParameters->compounds.size() == 0)
                return;

        processCompounds(mavenParameters->compounds, "compounds");
}

void BackgroundPeakUpdate::findPeaksQQQ() {
        if (mainwindow == NULL)
                return;
        vector<mzSlice*> slices = mainwindow->getSrmSlices();
        processSlices(slices, "QQQ Peaks");
        delete_all(slices);
}

/**
 * BackgroundPeakUpdate::setRunFunction Getting the function that has tobe ran
 * as a thread and updating it inside a variable
 * @param functionName [description]
 */
void BackgroundPeakUpdate::setRunFunction(QString functionName) {
        runFunction = functionName.toStdString();
}

void BackgroundPeakUpdate::pullIsotopes(PeakGroup* parentgroup) {
        getPullIsotopeSettings();
        peakDetector.pullIsotopes(parentgroup);
}

bool BackgroundPeakUpdate::covertToMzXML(QString filename, QString outfile) {

        QFile test(outfile);
        if (test.exists())
                return true;

        QString command = QString("ReAdW.exe --centroid --mzXML \"%1\" \"%2\"").arg(
                filename).arg(outfile);

        qDebug() << command;

        QProcess *process = new QProcess();
        //connect(process, SIGNAL(finished(int)), this, SLOT(doVideoCreated(int)));
        process->start(command);

        if (!process->waitForStarted()) {
                process->kill();
                return false;
        }

        while (!process->waitForFinished()) {
        };
        QFile testOut(outfile);
        return testOut.exists();
}