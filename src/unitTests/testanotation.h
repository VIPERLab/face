#ifndef TESTANOTATION_H
#define TESTANOTATION_H

#include <QString>

#include "facelib/facefeaturesanotation.h"

class TestAnotation
{
public:
    static void test(const QString &path)
    {
        FaceFeaturesAnotation::anotateBINs(path, true, false);
    }
};

#endif // TESTANOTATION_H
