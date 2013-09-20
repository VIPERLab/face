#ifndef FEATUREEXTRACTOR_H
#define FEATUREEXTRACTOR_H

#include "linalg/common.h"
#include "linalg/pca.h"
#include "linalg/icaofpca.h"
#include "linalg/ldaofpca.h"
#include "linalg/normalization.h"
#include "linalg/vector.h"

class FeatureExtractor
{
public:
    virtual Vector extract(const Vector &rawData) const = 0;
    virtual int outputLen() = 0;

    QVector<Vector> batchExtract(const QVector<Vector> &rawData)
    {
        QVector<Vector> result;
        for (int i = 0; i < rawData.count(); i++)
        {
            Vector extracted = extract(rawData[i]);
            result.append(extracted);
        }
        return result;
    }

    virtual ~FeatureExtractor() {}
};

class ZScoreFeatureExtractor : public FeatureExtractor
{

};

class PCAExtractor : public FeatureExtractor
{
private:
    PCA pca;

public:
    PCAExtractor(const PCA &pca) : pca(pca) {}

    Vector extract(const Vector &rawData) const
    {
        return pca.project(rawData);
    }

    int outputLen() { return pca.getModes(); }

    virtual ~PCAExtractor() {}
};

class ZScorePCAExtractor : public ZScoreFeatureExtractor
{
public:
    PCA pca;
    ZScoreNormalizationResult normParams;

    ZScorePCAExtractor() {}

    ZScorePCAExtractor(const PCA &pca, const QVector<Vector> &rawData ) : pca(pca)
    {
        QVector<Vector> projectedData = pca.batchProject(rawData);
        normParams = Normalization::zScoreNormalization(projectedData);
    }

    ZScorePCAExtractor(const QString &pcaPath, const QString &normParamsPath);

    void serialize(const QString &pcaPath, const QString &normParamsPath) const;

    Vector extract(const Vector &rawData) const
    {
        Vector projected = pca.project(rawData);
        Normalization::zScoreNormalization(projected, normParams);
        return projected;
    }

    int outputLen() { return pca.getModes(); }

    virtual ~ZScorePCAExtractor() {}
};

class NormPCAExtractor : public FeatureExtractor
{
private:
    PCA pca;

public:
    NormPCAExtractor(const PCA &pca) : pca(pca) {}

    Vector extract(const Vector &rawData) const
    {
        return pca.scaledProject(rawData);
    }

    int outputLen() { return pca.getModes(); }

    virtual ~NormPCAExtractor() {}
};

class ICAofPCAExtractor : public FeatureExtractor
{
private:
    ICAofPCA icaOfpca;

public:
    ICAofPCAExtractor(const ICAofPCA &icaOfpca) : icaOfpca(icaOfpca) {}

    Vector extract(const Vector &rawData) const
    {
        return icaOfpca.project(rawData);
    }

    int outputLen() { return icaOfpca.ica.getModes(); }

    virtual ~ICAofPCAExtractor() {}
};

class ZScoreICAofPCAExtractor : public ZScoreFeatureExtractor
{
private:
    ICAofPCA icaOfpca;
    ZScoreNormalizationResult normParams;

public:
    ZScoreICAofPCAExtractor(const ICAofPCA &icaOfpca, const QVector<Vector> &rawData )
    {
        this->icaOfpca = icaOfpca;
        QVector<Vector> projectedData = icaOfpca.batchProject(rawData);
        normParams = Normalization::zScoreNormalization(projectedData);
    }

    Vector extract(const Vector &rawData) const
    {
        Vector projected = icaOfpca.project(rawData);
        Normalization::zScoreNormalization(projected, normParams);
        return projected;
    }

    int outputLen() { return icaOfpca.ica.getModes(); }

    virtual ~ZScoreICAofPCAExtractor() {}
};

class ICAofPCAWhiteningExtractor : public FeatureExtractor
{
private:
    ICAofPCA icaOfpca;

public:
    ICAofPCAWhiteningExtractor(const ICAofPCA &icaOfpca) : icaOfpca(icaOfpca) {}

    Vector extract(const Vector &rawData) const
    {
        Vector pcaProjected = icaOfpca.pca.project(rawData);
        return icaOfpca.ica.whiten(pcaProjected);
    }

    int outputLen() { return icaOfpca.ica.getModes(); }

    virtual ~ICAofPCAWhiteningExtractor() {}
};

class ZScoreICAofPCAWhiteningExtractor : public ZScoreFeatureExtractor
{
private:
    ICAofPCA icaOfpca;
    ZScoreNormalizationResult normParams;

public:
    ZScoreICAofPCAWhiteningExtractor(const ICAofPCA &icaOfpca, const QVector<Vector> &rawData )
    {
        this->icaOfpca = icaOfpca;
        QVector<Vector> projectedData = icaOfpca.whiten(rawData);
        normParams = Normalization::zScoreNormalization(projectedData);
    }

    Vector extract(const Vector &rawData) const
    {
        Vector projected = icaOfpca.whiten(rawData);
        Normalization::zScoreNormalization(projected, normParams);
        return projected;
    }

    int outputLen() { return icaOfpca.ica.getModes(); }

    virtual ~ZScoreICAofPCAWhiteningExtractor() {}
};

class LDAofPCAExtractor : public FeatureExtractor
{
private:
    LDAofPCA ldaOfpca;

public:
    LDAofPCAExtractor(const LDAofPCA &ldaOfpca) : ldaOfpca(ldaOfpca) {}

    Vector extract(const Vector &rawData) const
    {
        return ldaOfpca.project(rawData);
    }

    int outputLen() { return ldaOfpca.lda.Wt.rows; }

    virtual ~LDAofPCAExtractor() {}
};

class ZScoreLDAofPCAExtractor : public ZScoreFeatureExtractor
{
private:
    LDAofPCA ldaOfpca;
    ZScoreNormalizationResult normParams;

public:
    ZScoreLDAofPCAExtractor(const LDAofPCA &ldaOfpca, const QVector<Vector> &rawData )
    {
        this->ldaOfpca = ldaOfpca;
        QVector<Vector> projectedData = ldaOfpca.batchProject(rawData);
        normParams = Normalization::zScoreNormalization(projectedData);
    }

    Vector extract(const Vector &rawData) const
    {
        Vector projected = ldaOfpca.project(rawData);
        Normalization::zScoreNormalization(projected, normParams);
        return projected;
    }

    int outputLen() { return ldaOfpca.lda.Wt.rows; }

    virtual ~ZScoreLDAofPCAExtractor() {}
};

class LDAExtractor : public FeatureExtractor
{
private:
    LDA lda;

public:
    LDAExtractor(const LDA &lda) : lda(lda) {}

    Vector extract(const Vector &rawData) const
    {
        return lda.project(rawData);
    }

    int outputLen() { return lda.Wt.rows; }

    virtual ~LDAExtractor() {}
};

class ZScoreLDAExtractor : public ZScoreFeatureExtractor
{
private:
    LDA lda;
    ZScoreNormalizationResult normParams;

public:
    ZScoreLDAExtractor(const LDA &lda, const QVector<Vector> &rawData )
    {
        this->lda = lda;
        QVector<Vector> projectedData = lda.batchProject(rawData);
        normParams = Normalization::zScoreNormalization(projectedData);
    }

    Vector extract(const Vector &rawData) const
    {
        Vector projected = lda.project(rawData);
        Normalization::zScoreNormalization(projected, normParams);
        return projected;
    }

    int outputLen() { return lda.Wt.rows; }

    virtual ~ZScoreLDAExtractor() {}
};

class PassExtractor : public FeatureExtractor
{
public:
    Vector extract(const Vector &rawData) const
    {
        return Vector(rawData);
    }

    int outputLen() { return -1; }
};

class ZScorePassExtractor : public ZScoreFeatureExtractor
{
private:
    ZScoreNormalizationResult normParams;

public:
    ZScorePassExtractor() {}

    ZScorePassExtractor(const QString &normParamsPath);

    ZScorePassExtractor(const QVector<Vector> &rawData)
    {
        QVector<Vector> projectedData;
        for (int i = 0; i < rawData.count(); i++)
        {
             projectedData.append(Vector(rawData[i]));
        }
        normParams = Normalization::zScoreNormalization(projectedData);
    }

    Vector extract(const Vector &rawData) const
    {
        Vector projected(rawData);
        Normalization::zScoreNormalization(projected, normParams);
        return projected;
    }

    int outputLen() { return normParams.mean.count(); }

    void serialize(const QString &normParamsPath);
};

class MeanNormalizationExtractor : public FeatureExtractor
{
private:
    MeanNormalizationResult params;

public:
    MeanNormalizationExtractor(const QVector<Vector> &rawData)
    {
        QVector<Vector> projectedData;
        for (int i = 0; i < rawData.count(); i++)
        {
             projectedData.append(Vector(rawData[i]));
        }
        params = Normalization::meanNormalization(projectedData);
    }

    Vector extract(const Vector &rawData) const
    {
        Vector projected(rawData);
        Normalization::meanNormalization(projected, params);
        return projected;
    }

    int outputLen() { return params.mean.count(); }
};

#endif // FEATUREEXTRACTOR_H
