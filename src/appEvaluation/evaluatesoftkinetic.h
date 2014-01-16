#ifndef EVALUATESOFTKINETIC_H
#define EVALUATESOFTKINETIC_H

#include "facelib/facealigner.h"
#include "biometrics/facetemplate.h"
#include "facelib/surfaceprocessor.h"
#include "biometrics/multibiomertricsautotuner.h"

class EvaluateSoftKinetic
{
public:
    class MeshProcessor
    {
    public:
        virtual void process(Face::FaceData::Mesh &input) = 0;
    };

    class PassProcess : public MeshProcessor
    {
    public:
        virtual void process(Face::FaceData::Mesh &) {}
    };

    class Smooth : public MeshProcessor
    {
        int iterations;
        double alpha;

    public:
        Smooth(int iterations, double alpha) : iterations(iterations), alpha(alpha) { }

        void process(Face::FaceData::Mesh &input)
        {
            Face::FaceData::SurfaceProcessor::smooth(input, alpha, iterations);
        }
    };

    class ZSmooth : public MeshProcessor
    {
        int iterations;
        double alpha;

    public:
        ZSmooth(int iterations, double alpha) : iterations(iterations), alpha(alpha) { }

        void process(Face::FaceData::Mesh &input)
        {
            Face::FaceData::SurfaceProcessor::zsmooth(input, alpha, iterations);
        }
    };

    class AnisoSmooth : public MeshProcessor
    {
        Face::FaceData::SurfaceProcessor::AnisotropicDiffusionType type;
        double edgeThresh;
        int steps;
        double dt;

    public:
        AnisoSmooth(Face::FaceData::SurfaceProcessor::AnisotropicDiffusionType type, double edgeThresh, int steps, double dt) :
            type(type), edgeThresh(edgeThresh), steps(steps), dt(dt) { }

        void process(Face::FaceData::Mesh &input)
        {
            Face::FaceData::SurfaceProcessor::anisotropicDiffusionSmooth(input, type, edgeThresh, steps, dt);
        }
    };

    static void evaluateSmoothing()
    {
        Face::FaceData::FaceAligner aligner(Face::FaceData::Mesh::fromOBJ("../../test/meanForAlign.obj", false));
        Face::Biometrics::FaceClassifier faceClassifier("../../test/frgc/classifiers/");
        QVector<Face::FaceData::Mesh> meshes;
        QVector<int> ids;

        QVector<QString> binFiles = Face::LinAlg::Loader::listFiles("../../test/softKinetic/02/", "*.binz",
                                                                    Face::LinAlg::Loader::AbsoluteFull);
        foreach(const QString &path, binFiles)
        {
            ids << QFileInfo(path).baseName().split("-")[0].toInt();
            meshes << Face::FaceData::Mesh::fromBINZ(path);
            aligner.icpAlign(meshes.last(), 10, Face::FaceData::FaceAligner::NoseTipDetection);
        }

        QVector<MeshProcessor *> processors;
        //processors << new PassProcess();
        processors << new ZSmooth(5, 0.5); //!!!
        //processors << new ZSmooth(10, 0.5);
        //processors << new ZSmooth(5, 1);
        //processors << new ZSmooth(10, 1);
        //processors << new AnisoSmooth(SurfaceProcessor::PeronaMalic, 5, 8, 0.04); //!!!
        //processors << new AnisoSmooth(SurfaceProcessor::PeronaMalic, 10, 16, 0.02);
        //processors << new AnisoSmooth(SurfaceProcessor::Linear, 0.5, 12, 0.04);

        foreach(MeshProcessor *p, processors)
        {
            QVector<Face::Biometrics::Face3DTemplate *> templates;

            for (int i = 0; i < meshes.count(); i++)
            {
                Face::FaceData::Mesh in(meshes[i]);
                p->process(in);
                templates << new Face::Biometrics::Face3DTemplate(ids[i], in, faceClassifier);
            }

            Face::Biometrics::FaceClassifier c;
            faceClassifier.relearnFinalFusion(templates, c, false);

            qDeleteAll(templates);
            templates.clear();

            for (int i = 0; i < meshes.count(); i++)
            {
                Face::FaceData::Mesh in(meshes[i]);
                p->process(in);
                templates << new Face::Biometrics::Face3DTemplate(ids[i], in, c);
            }
            Face::Biometrics::Evaluation e = c.evaluate(templates);
            qDebug() << e.eer << e.fnmrAtFmr(0.01) << e.fnmrAtFmr(0.001);
            qDeleteAll(templates);

            c.serialize("test");
        }
    }

    static int checkAligning(int argc, char *argv[])
    {
        Face::FaceData::FaceAligner aligner(Face::FaceData::Mesh::fromOBJ("../../test/meanForAlign.obj", false));
        QString dir("../../test/softKinetic/03/DS32528233700098_radim/");

        QVector<QString> fileNames = Face::LinAlg::Loader::listFiles(dir, "*.binz", Face::LinAlg::Loader::Filename);
        foreach(const QString &fileName, fileNames) {
            Face::FaceData::Mesh m = Face::FaceData::Mesh::fromBINZ(dir + fileName);
            Face::FaceData::SurfaceProcessor::mdenoising(m, 0.02f, 10, 10);
            aligner.icpAlign(m, 100, Face::FaceData::FaceAligner::TemplateMatching);
            Face::FaceData::MapConverter mc;
            Face::FaceData::Map texture = Face::FaceData::SurfaceProcessor::depthmap(m, mc, cv::Point(-100,-100), cv::Point(100,100),
                                                                                     1.0, Face::FaceData::SurfaceProcessor::Texture_I);
            Matrix mat = texture.toMatrix();
            cv::circle(mat, cv::Point(100,100), 2, 1.0);
            cv::circle(mat, cv::Point(100,100), 1, 0.0);
            cv::imshow("face", mat);
            cv::waitKey();
        }
        return 0;
    }

    static void loadBinZMeshes(const QString &dir, const Face::FaceData::FaceAligner &aligner, QVector<int> &ids,
                               QVector<Face::FaceData::Mesh> &meshes, int icpIterations, int smoothIterations, float smoothCoef)
    {
        const QVector<QString> fileNames = Face::LinAlg::Loader::listFiles(dir, "*.binz", Face::LinAlg::Loader::Filename);
        int n = fileNames.count();
        ids.resize(n);
        meshes.resize(n);

        //foreach(const QString &fileName, fileNames)
        //#pragma omp parallel for
        for (int i = 0; i < n; i++)
        {
            Face::FaceData::Mesh m = Face::FaceData::Mesh::fromBINZ(dir + fileNames[i]);
            Face::FaceData::SurfaceProcessor::mdenoising(m, smoothCoef, smoothIterations, smoothIterations); // 0.02f, 10, 10);
            aligner.icpAlign(m, icpIterations, Face::FaceData::FaceAligner::TemplateMatching); // 20

            ids[i] = fileNames[i].split('-')[0].toInt();
            meshes[i] = m;
        }
    }

    static Face::Biometrics::Evaluation evaluateMultiExtractor(const Face::Biometrics::MultiExtractor &extractor,
                                                               const Face::FaceData::FaceAligner &aligner, const QString &evalPath,
                                                               int icpIterations, int smoothIterations, float smoothCoef)
    {
        QVector<int> ids;
        QVector<Face::FaceData::Mesh> meshes;
        loadBinZMeshes(evalPath, aligner, ids, meshes, icpIterations, smoothIterations, smoothCoef);
        int n = ids.count();
        QVector<Face::Biometrics::MultiTemplate> templates;
        for (int i = 0; i < n; i++)
        {
            templates << extractor.extract(meshes[i], 1, ids[i]);
        }

        extractor.createPerSubjectScoreCharts(templates, "scoremap");
        return extractor.evaluate(templates);
    }

    static void createMultiExtractor(int argc, char *argv[])
    {
        if (argc != 13)
        {
            qDebug() << "usage:" << argv[0] << "meanForAlign.obj unitsFile FRGCdir targetSoftKineticDir evaluationSoftKineticDir";
            qDebug() << "   trainICPiters testICPiters trainSmoothCoef testSmoothCoef trainSmoothIters testSmoothIters outputDir";
            return;
        }

        QString meanFaceForAlign = argv[1];
        QString unitsFile = argv[2];
        QString frgcDirectory = argv[3];
        QString targetSoftKineticDir = argv[4];
        QString evaluationSoftKineticDir = argv[5];
        int trainICPiters = QString(argv[6]).toInt();
        int testICPiters = QString(argv[7]).toInt();
        float trainSmoothCoef = QString(argv[8]).toFloat();
        float testSmoothCoef = QString(argv[9]).toFloat();
        int trainSmoothIters = QString(argv[10]).toInt();
        int testSmoothIters = QString(argv[11]).toInt();
        QString outputDir = argv[12];

        Face::FaceData::FaceAligner aligner(Face::FaceData::Mesh::fromOBJ(meanFaceForAlign, false));
        QVector<int> ids;
        QVector<Face::FaceData::Mesh> meshes;
        loadBinZMeshes(targetSoftKineticDir, aligner, ids, meshes, trainICPiters, trainSmoothIters, trainSmoothCoef);

        qDebug() << "loading FRGC";
        Face::Biometrics::MultiBiomertricsAutoTuner::Input frgcData =
                Face::Biometrics::MultiBiomertricsAutoTuner::Input::fromDirectoryWithExportedCurvatureImages(frgcDirectory, "d", 300);

        qDebug() << "SoftKinetic data";
        Face::Biometrics::MultiBiomertricsAutoTuner::Input softKineticData =
                Face::Biometrics::MultiBiomertricsAutoTuner::Input::fromAlignedMeshes(ids, meshes);

        Face::Biometrics::MultiBiomertricsAutoTuner::Settings settings(Face::Biometrics::MultiBiomertricsAutoTuner::Settings::FCT_SVM, unitsFile);
        Face::Biometrics::MultiExtractor extractor = Face::Biometrics::MultiBiomertricsAutoTuner::train(frgcData, softKineticData, settings);

        extractor.serialize(outputDir);

        Face::Biometrics::Evaluation eval = evaluateMultiExtractor(extractor, aligner, evaluationSoftKineticDir,
                                                                   testICPiters, testSmoothIters, testSmoothCoef);
        qDebug() << eval.eer;
        eval.outputResults(outputDir + "/eval", 10);
    }

    static void evaluateMultiExtractor()
    {
        Face::Biometrics::MultiExtractor extractor("../../test/softKinetic/out1");
        Face::FaceData::FaceAligner aligner(Face::FaceData::Mesh::fromOBJ("../../test/meanForAlign.obj", false));
        QString dir("../../test/softKinetic/03/stepan/");
        //QString dir("../../test/softKinetic/03/radim/");
        //QString dir("../../test/softKinetic/03/honza/");

        qDebug() << evaluateMultiExtractor(extractor, aligner, dir, 100, 10, 0.02).eer;
    }

    static void evaluateAging()
    {
        Face::Biometrics::MultiExtractor extractor("../../test/softKinetic/out1");
        Face::FaceData::FaceAligner aligner(Face::FaceData::Mesh::fromOBJ("../../test/meanForAlign.obj", false));
        QString dir("../../test/softKinetic/03/stepan/");
        const QVector<QString> fileNames = Face::LinAlg::Loader::listFiles(dir, "1000*.binz", Face::LinAlg::Loader::Filename);

        Face::Biometrics::MultiTemplate reference;
        for (int i = 0; i < fileNames.count(); i++)
        {
            Face::FaceData::Mesh m = Face::FaceData::Mesh::fromBINZ(dir + fileNames[i]);
            Face::FaceData::SurfaceProcessor::mdenoising(m, 0.02f, 10, 10);
            aligner.icpAlign(m, 100, Face::FaceData::FaceAligner::TemplateMatching);

            if (i == 0)
            {
                reference = extractor.extract(m, 1, 1);
                continue;
            }

            Face::Biometrics::MultiTemplate probe = extractor.extract(m, 1, 1);
            qDebug() << extractor.compare(reference, probe);
        }
    }
};

#endif // EVALUATESOFTKINETIC_H
